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
#include "wabi/usd/usdRi/lightFilterAPI.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/tokens.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdRiLightFilterAPI, TfType::Bases<UsdAPISchemaBase>>();
}

TF_DEFINE_PRIVATE_TOKENS(_schemaTokens, (RiLightFilterAPI));

/* virtual */
UsdRiLightFilterAPI::~UsdRiLightFilterAPI()
{}

/* static */
UsdRiLightFilterAPI UsdRiLightFilterAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage)
  {
    TF_CODING_ERROR("Invalid stage");
    return UsdRiLightFilterAPI();
  }
  return UsdRiLightFilterAPI(stage->GetPrimAtPath(path));
}

/* virtual */
UsdSchemaKind UsdRiLightFilterAPI::_GetSchemaKind() const
{
  return UsdRiLightFilterAPI::schemaKind;
}

/* virtual */
UsdSchemaKind UsdRiLightFilterAPI::_GetSchemaType() const
{
  return UsdRiLightFilterAPI::schemaType;
}

/* static */
UsdRiLightFilterAPI UsdRiLightFilterAPI::Apply(const UsdPrim &prim)
{
  if (prim.ApplyAPI<UsdRiLightFilterAPI>())
  {
    return UsdRiLightFilterAPI(prim);
  }
  return UsdRiLightFilterAPI();
}

/* static */
const TfType &UsdRiLightFilterAPI::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdRiLightFilterAPI>();
  return tfType;
}

/* static */
bool UsdRiLightFilterAPI::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdRiLightFilterAPI::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdRiLightFilterAPI::GetRiCombineModeAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riCombineMode);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiCombineModeAttr(VtValue const &defaultValue,
                                                          bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riCombineMode,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiLightFilterAPI::GetRiDensityAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riDensity);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiDensityAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riDensity,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiLightFilterAPI::GetRiInvertAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riInvert);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiInvertAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riInvert,
                                    SdfValueTypeNames->Bool,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiLightFilterAPI::GetRiIntensityAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riIntensity);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiIntensityAttr(VtValue const &defaultValue,
                                                        bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riIntensity,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiLightFilterAPI::GetRiExposureAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riExposure);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiExposureAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riExposure,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiLightFilterAPI::GetRiDiffuseAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riDiffuse);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiDiffuseAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riDiffuse,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdRiLightFilterAPI::GetRiSpecularAttr() const
{
  return GetPrim().GetAttribute(UsdRiTokens->riSpecular);
}

UsdAttribute UsdRiLightFilterAPI::CreateRiSpecularAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdRiTokens->riSpecular,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

namespace
{
static inline TfTokenVector _ConcatenateAttributeNames(const TfTokenVector &left, const TfTokenVector &right)
{
  TfTokenVector result;
  result.reserve(left.size() + right.size());
  result.insert(result.end(), left.begin(), left.end());
  result.insert(result.end(), right.begin(), right.end());
  return result;
}
}  // namespace

/*static*/
const TfTokenVector &UsdRiLightFilterAPI::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdRiTokens->riCombineMode,
    UsdRiTokens->riDensity,
    UsdRiTokens->riInvert,
    UsdRiTokens->riIntensity,
    UsdRiTokens->riExposure,
    UsdRiTokens->riDiffuse,
    UsdRiTokens->riSpecular,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(UsdAPISchemaBase::GetSchemaAttributeNames(true),
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
