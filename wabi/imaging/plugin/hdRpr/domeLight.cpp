/************************************************************************
Copyright 2020 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#include "domeLight.h"
#include "renderParam.h"
#include "rprApi.h"

#include "wabi/imaging/rprUsd/debugCodes.h"

#include "wabi/base/gf/matrix4d.h"
#include "wabi/imaging/hd/light.h"
#include "wabi/imaging/hd/sceneDelegate.h"
#include "wabi/usd/ar/resolver.h"
#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/usdLux/blackbody.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_PRIVATE_TOKENS(_tokens, ((backgroundOverride, "rpr:backgroundOverride")));

static void removeFirstSlash(std::string &string)
{
  // Don't need this for *nix/Mac
#ifdef _WIN32
  if (string[0] == '/' || string[0] == '\\') {
    string.erase(0, 1);
  }
#endif
}

static float computeLightIntensity(float intensity, float exposure)
{
  return intensity * exp2(exposure);
}

void HdRprDomeLight::Sync(HdSceneDelegate *sceneDelegate,
                          HdRenderParam *renderParam,
                          HdDirtyBits *dirtyBits)
{

  auto rprRenderParam = static_cast<HdRprRenderParam *>(renderParam);
  auto rprApi = rprRenderParam->AcquireRprApiForEdit();

  SdfPath const &id = GetId();
  HdDirtyBits bits = *dirtyBits;

  if (bits & HdLight::DirtyTransform) {
    m_transform = GfMatrix4f(
      sceneDelegate->GetLightParamValue(id, HdTokens->transform).Get<GfMatrix4d>());
    m_transform *= GfMatrix4f(1.0).SetScale(GfVec3f(1.0f, 1.0f, -1.0f));
  }

  bool newLight = false;
  if (bits & HdLight::DirtyParams) {
    if (m_rprLight) {
      rprApi->Release(m_rprLight);
      m_rprLight = nullptr;
    }

    bool isVisible = sceneDelegate->GetVisible(id);
    if (!isVisible) {
      *dirtyBits = HdLight::Clean;
      return;
    }

    VtValue const &backgroundOverride = sceneDelegate->GetLightParamValue(
      id,
      _tokens->backgroundOverride);

    float intensity = sceneDelegate->GetLightParamValue(id, HdLightTokens->intensity).Get<float>();
    float exposure = sceneDelegate->GetLightParamValue(id, HdLightTokens->exposure).Get<float>();
    float computedIntensity = computeLightIntensity(intensity, exposure);

    std::string texturePath;
    VtValue texturePathValue = sceneDelegate->GetLightParamValue(id, HdLightTokens->textureFile);
    if (texturePathValue.IsHolding<SdfAssetPath>()) {
      auto &assetPath = texturePathValue.UncheckedGet<SdfAssetPath>();
      if (assetPath.GetResolvedPath().empty()) {
        texturePath = ArGetResolver().Resolve(assetPath.GetAssetPath());
      } else {
        texturePath = assetPath.GetResolvedPath();
      }
      // XXX: Why?
      removeFirstSlash(texturePath);
    } else if (texturePathValue.IsHolding<std::string>()) {
      // XXX: Is it even possible?
      texturePath = texturePathValue.UncheckedGet<std::string>();
    }

    if (texturePath.empty()) {
      GfVec3f color =
        sceneDelegate->GetLightParamValue(id, HdPrimvarRoleTokens->color).Get<GfVec3f>();
      if (sceneDelegate->GetLightParamValue(id, HdLightTokens->enableColorTemperature)
            .Get<bool>()) {
        GfVec3f temperatureColor = UsdLuxBlackbodyTemperatureAsRgb(
          sceneDelegate->GetLightParamValue(id, HdLightTokens->colorTemperature).Get<float>());
        color[0] *= temperatureColor[0];
        color[1] *= temperatureColor[1];
        color[2] *= temperatureColor[2];
      }

      m_rprLight = rprApi->CreateEnvironmentLight(color, computedIntensity, backgroundOverride);
    } else {
      m_rprLight = rprApi->CreateEnvironmentLight(texturePath,
                                                  computedIntensity,
                                                  backgroundOverride);
    }

    if (m_rprLight) {
      newLight = true;

      if (RprUsdIsLeakCheckEnabled()) {
        rprApi->SetName(m_rprLight, id.GetText());
      }
    }
  }

  if (newLight || ((bits & HdLight::DirtyTransform) && m_rprLight)) {
    rprApi->SetTransform(m_rprLight, m_transform);
  }

  *dirtyBits = HdLight::Clean;
}

HdDirtyBits HdRprDomeLight::GetInitialDirtyBitsMask() const
{
  return HdLight::AllDirty;
}

void HdRprDomeLight::Finalize(HdRenderParam *renderParam)
{
  auto rprRenderParam = static_cast<HdRprRenderParam *>(renderParam);

  if (m_rprLight) {
    rprRenderParam->AcquireRprApiForEdit()->Release(m_rprLight);
    m_rprLight = nullptr;
  }

  HdSprim::Finalize(renderParam);
}

WABI_NAMESPACE_END
