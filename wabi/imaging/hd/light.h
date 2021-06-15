//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef WABI_IMAGING_HD_LIGHT_H
#define WABI_IMAGING_HD_LIGHT_H

#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/sprim.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/staticTokens.h"
#include "wabi/base/vt/dictionary.h"
#include "wabi/base/vt/value.h"

#include <vector>

WABI_NAMESPACE_BEGIN

#define HD_LIGHT_TOKENS \
  (angle)(color)(colorTemperature)(enableColorTemperature)(exposure)(height)(intensity)(radius)(length)(( \
    textureFile, "texture:file"))(( \
    textureFormat, "texture:format"))(width)(diffuse)(specular)(normalize)((shapingFocus, \
                                                                            "shaping:focus"))(( \
    shapingFocusTint, "shaping:focusTint"))((shapingConeAngle, "shaping:cone:angle"))(( \
    shapingConeSoftness, "shaping:cone:softness"))((shapingIesFile, "shaping:ies:file"))(( \
    shapingIesAngleScale, "shaping:ies:angleScale"))((shapingIesNormalize, "shaping:ies:normalize"))(( \
    shadowEnable, "shadow:enable"))((shadowColor, "shadow:color"))((shadowDistance, "shadow:distance"))(( \
    shadowFalloff, "shadow:falloff"))((shadowFalloffGamma, "shadow:falloffGamma")) \
\
    (params)(shadowCollection)(shadowParams)

TF_DECLARE_PUBLIC_TOKENS(HdLightTokens, HD_API, HD_LIGHT_TOKENS);

class HdSceneDelegate;
typedef std::vector<class HdLight const *> HdLightPtrConstVector;

/// \class HdLight
///
/// A light model, used in conjunction with HdRenderPass.
///
class HdLight : public HdSprim {
 public:
  HD_API
  HdLight(SdfPath const &id);
  HD_API
  virtual ~HdLight();

  // Change tracking for HdLight
  enum DirtyBits : HdDirtyBits {
    Clean = 0,
    DirtyTransform = 1 << 0,
    DirtyParams = 1 << 1,
    DirtyShadowParams = 1 << 2,
    DirtyCollection = 1 << 3,
    DirtyResource = 1 << 4,
    AllDirty = (DirtyTransform | DirtyParams | DirtyShadowParams | DirtyCollection | DirtyResource)
  };
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_LIGHT_H
