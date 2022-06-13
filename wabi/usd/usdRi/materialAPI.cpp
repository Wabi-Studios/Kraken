/*
 * Copyright 2021 Pixar. All Rights Reserved.
 *
 * Portions of this file are derived from original work by Pixar
 * distributed with Universal Scene Description, a project of the
 * Academy Software Foundation (ASWF). https://www.aswf.io/
 *
 * Licensed under the Apache License, Version 2.0 (the "Apache License")
 * with the following modification; you may not use this file except in
 * compliance with the Apache License and the following modification:
 * Section 6. Trademarks. is deleted and replaced with:
 *
 * 6. Trademarks. This License does not grant permission to use the trade
 *    names, trademarks, service marks, or product names of the Licensor
 *    and its affiliates, except as required to comply with Section 4(c)
 *    of the License and to reproduce the content of the NOTICE file.
 *
 * You may obtain a copy of the Apache License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Apache License with the above modification is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the Apache License for the
 * specific language governing permissions and limitations under the
 * Apache License.
 *
 * Modifications copyright (C) 2020-2021 Wabi.
 */
#include "wabi/usd/usdRi/materialAPI.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/tokens.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdRiMaterialAPI, TfType::Bases<UsdAPISchemaBase>>();
}

TF_DEFINE_PRIVATE_TOKENS(_schemaTokens, (RiMaterialAPI));

/* virtual */
UsdRiMaterialAPI::~UsdRiMaterialAPI() {}

/* static */
UsdRiMaterialAPI UsdRiMaterialAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdRiMaterialAPI();
  }
  return UsdRiMaterialAPI(stage->GetPrimAtPath(path));
}

/* virtual */
UsdSchemaKind UsdRiMaterialAPI::_GetSchemaKind() const
{
  return UsdRiMaterialAPI::schemaKind;
}

/* static */
UsdRiMaterialAPI UsdRiMaterialAPI::Apply(const UsdPrim &prim)
{
  if (prim.ApplyAPI<UsdRiMaterialAPI>()) {
    return UsdRiMaterialAPI(prim);
  }
  return UsdRiMaterialAPI();
}

/* static */
const TfType &UsdRiMaterialAPI::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdRiMaterialAPI>();
  return tfType;
}

/* static */
bool UsdRiMaterialAPI::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdRiMaterialAPI::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdRiMaterialAPI::GetSurfaceAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->outputsRiSurface);
}

UsdAttribute UsdRiMaterialAPI::CreateSurfaceAttr(VtValue const &defaultValue,
                                                 bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->outputsRiSurface,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiMaterialAPI::GetDisplacementAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->outputsRiDisplacement);
}

UsdAttribute UsdRiMaterialAPI::CreateDisplacementAttr(VtValue const &defaultValue,
                                                      bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->outputsRiDisplacement,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiMaterialAPI::GetVolumeAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->outputsRiVolume);
}

UsdAttribute UsdRiMaterialAPI::CreateVolumeAttr(VtValue const &defaultValue,
                                                bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->outputsRiVolume,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

namespace
{
  static inline TfTokenVector _ConcatenateAttributeNames(const TfTokenVector &left,
                                                         const TfTokenVector &right)
  {
    TfTokenVector result;
    result.reserve(left.size() + right.size());
    result.insert(result.end(), left.begin(), left.end());
    result.insert(result.end(), right.begin(), right.end());
    return result;
  }
}  // namespace

/*static*/
const TfTokenVector &UsdRiMaterialAPI::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdRiTokens->outputsRiSurface,
    UsdRiTokens->outputsRiDisplacement,
    UsdRiTokens->outputsRiVolume,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(
    UsdAPISchemaBase::GetSchemaAttributeNames(true),
    localNames);

  if (includeInherited)
    return allNames;
  else
    return localNames;
}

WABI_NAMESPACE_END

// ===================================================================== //
// Feel free to add custom code below this line. It will be preserved by
// the code generator.
//
// Just remember to wrap code in the appropriate delimiters:
// 'WABI_NAMESPACE_BEGIN', 'WABI_NAMESPACE_END'.
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--

#include "wabi/base/tf/envSetting.h"
#include "wabi/usd/usdShade/connectableAPI.h"
#include "wabi/usd/usdShade/material.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_PRIVATE_TOKENS(_tokens,

                         ((defaultOutputName, "outputs:out"))

                         // These tokens are required for backwards compatibility. They're
                         // redefined here so we can stop relying on UsdRiLookAPI entirely.
                         (ri)((riLookDisplacement, "riLook:displacement"))((riLookSurface, "riLook:surface"))((riLookVolume,
                                                                                                               "riLook:volume"))

                         // deprecated tokens for handling backwards compatibility.
                         ((bxdfOutputName, "ri:bxdf"))((bxdfOutputAttrName, "outputs:ri:bxdf"))((riLookBxdf, "riLook:bxdf")));

TF_DEFINE_ENV_SETTING(USD_RI_WRITE_BXDF_OUTPUT,
                      false,
                      "If set to false, then \"ri:surface\" output is created instead of the "
                      "\"ri:bxdf\" output, when UsdRiMaterialAPI::SetSurfaceSource() is "
                      "invoked.");

UsdShadeShader UsdRiMaterialAPI::_GetSourceShaderObject(const UsdShadeOutput &output,
                                                        bool ignoreBaseMaterial) const
{
  // If output doesn't have a valid attribute, return an invalid shader.
  if (!output.GetAttr()) {
    return UsdShadeShader();
  }

  if (ignoreBaseMaterial && UsdShadeConnectableAPI::IsSourceConnectionFromBaseMaterial(output)) {
    return UsdShadeShader();
  }

  UsdShadeConnectableAPI source;
  TfToken sourceName;
  UsdShadeAttributeType sourceType;

  if (UsdShadeConnectableAPI::GetConnectedSource(output, &source, &sourceName, &sourceType)) {
    return source;
  }

  return UsdShadeShader();
}

UsdShadeOutput UsdRiMaterialAPI::_GetBxdfOutput(const UsdPrim &materialPrim) const
{
  // Check if the deprecated bxdf output is present.
  if (const UsdAttribute bxdfAttr = materialPrim.GetAttribute(_tokens->bxdfOutputAttrName)) {
    return UsdShadeOutput(bxdfAttr);
  }
  return UsdShadeOutput();
}

UsdShadeShader UsdRiMaterialAPI::GetSurface(bool ignoreBaseMaterial) const
{
  if (UsdShadeShader surface = _GetSourceShaderObject(GetSurfaceOutput(), ignoreBaseMaterial)) {
    return surface;
  }

  if (UsdShadeOutput bxdfOutput = _GetBxdfOutput(GetPrim())) {
    return _GetSourceShaderObject(bxdfOutput, ignoreBaseMaterial);
  }

  return UsdShadeShader();
}

UsdShadeShader UsdRiMaterialAPI::GetDisplacement(bool ignoreBaseMaterial) const
{
  return _GetSourceShaderObject(GetDisplacementOutput(), ignoreBaseMaterial);
}

UsdShadeShader UsdRiMaterialAPI::GetVolume(bool ignoreBaseMaterial) const
{
  return _GetSourceShaderObject(GetVolumeOutput(), ignoreBaseMaterial);
}

UsdShadeOutput UsdRiMaterialAPI::GetSurfaceOutput() const
{
  return UsdShadeMaterial(GetPrim()).GetSurfaceOutput(_tokens->ri);
}

UsdShadeOutput UsdRiMaterialAPI::GetDisplacementOutput() const
{
  return UsdShadeMaterial(GetPrim()).GetDisplacementOutput(_tokens->ri);
}

UsdShadeOutput UsdRiMaterialAPI::GetVolumeOutput() const
{
  return UsdShadeMaterial(GetPrim()).GetVolumeOutput(_tokens->ri);
}

bool UsdRiMaterialAPI::SetSurfaceSource(const SdfPath &surfacePath) const
{
  static const bool writeBxdfOutput = TfGetEnvSetting(USD_RI_WRITE_BXDF_OUTPUT);
  if (writeBxdfOutput) {
    if (UsdShadeOutput bxdfOutput = UsdShadeMaterial(GetPrim()).CreateOutput(
          _tokens->bxdfOutputName,
          SdfValueTypeNames->Token)) {
      const SdfPath sourcePath = surfacePath.IsPropertyPath() ?
                                   surfacePath :
                                   surfacePath.AppendProperty(_tokens->defaultOutputName);
      return UsdShadeConnectableAPI::ConnectToSource(bxdfOutput, sourcePath);
    }
    return false;
  }

  UsdShadeOutput surfaceOutput = UsdShadeMaterial(GetPrim()).CreateSurfaceOutput(
    /*purpose*/ _tokens->ri);
  return UsdShadeConnectableAPI::ConnectToSource(
    surfaceOutput,
    surfacePath.IsPropertyPath() ? surfacePath :
                                   surfacePath.AppendProperty(_tokens->defaultOutputName));
}

bool UsdRiMaterialAPI::SetDisplacementSource(const SdfPath &displacementPath) const
{
  UsdShadeOutput displacementOutput = UsdShadeMaterial(GetPrim()).CreateDisplacementOutput(
    /*purpose*/ _tokens->ri);
  return UsdShadeConnectableAPI::ConnectToSource(
    displacementOutput,
    displacementPath.IsPropertyPath() ?
      displacementPath :
      displacementPath.AppendProperty(_tokens->defaultOutputName));
}

bool UsdRiMaterialAPI::SetVolumeSource(const SdfPath &volumePath) const
{
  UsdShadeOutput volumeOutput = UsdShadeMaterial(GetPrim()).CreateVolumeOutput(
    /*purpose*/ _tokens->ri);
  return UsdShadeConnectableAPI::ConnectToSource(
    volumeOutput,
    volumePath.IsPropertyPath() ? volumePath :
                                  volumePath.AppendProperty(_tokens->defaultOutputName));
}

UsdShadeNodeGraph::InterfaceInputConsumersMap UsdRiMaterialAPI::ComputeInterfaceInputConsumersMap(
  bool computeTransitiveConsumers) const
{
  return UsdShadeNodeGraph(GetPrim()).ComputeInterfaceInputConsumersMap(
    computeTransitiveConsumers);
}

WABI_NAMESPACE_END
