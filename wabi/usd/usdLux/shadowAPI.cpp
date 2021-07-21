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
#include "wabi/usd/usdLux/shadowAPI.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/tokens.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdLuxShadowAPI, TfType::Bases<UsdAPISchemaBase>>();
}

TF_DEFINE_PRIVATE_TOKENS(_schemaTokens, (ShadowAPI));

/* virtual */
UsdLuxShadowAPI::~UsdLuxShadowAPI()
{}

/* static */
UsdLuxShadowAPI UsdLuxShadowAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage)
  {
    TF_CODING_ERROR("Invalid stage");
    return UsdLuxShadowAPI();
  }
  return UsdLuxShadowAPI(stage->GetPrimAtPath(path));
}

/* virtual */
UsdSchemaKind UsdLuxShadowAPI::GetSchemaKind() const
{
  return UsdLuxShadowAPI::schemaKind;
}

/* static */
UsdLuxShadowAPI UsdLuxShadowAPI::Apply(const UsdPrim &prim)
{
  if (prim.ApplyAPI<UsdLuxShadowAPI>())
  {
    return UsdLuxShadowAPI(prim);
  }
  return UsdLuxShadowAPI();
}

/* static */
const TfType &UsdLuxShadowAPI::GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdLuxShadowAPI>();
  return tfType;
}

/* static */
bool UsdLuxShadowAPI::IsTypedSchema()
{
  static bool isTyped = GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdLuxShadowAPI::GetType() const
{
  return GetStaticTfType();
}

UsdAttribute UsdLuxShadowAPI::GetShadowEnableAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->inputsShadowEnable);
}

UsdAttribute UsdLuxShadowAPI::CreateShadowEnableAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->inputsShadowEnable,
                                    SdfValueTypeNames->Bool,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdLuxShadowAPI::GetShadowColorAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->inputsShadowColor);
}

UsdAttribute UsdLuxShadowAPI::CreateShadowColorAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->inputsShadowColor,
                                    SdfValueTypeNames->Color3f,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdLuxShadowAPI::GetShadowDistanceAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->inputsShadowDistance);
}

UsdAttribute UsdLuxShadowAPI::CreateShadowDistanceAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->inputsShadowDistance,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdLuxShadowAPI::GetShadowFalloffAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->inputsShadowFalloff);
}

UsdAttribute UsdLuxShadowAPI::CreateShadowFalloffAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->inputsShadowFalloff,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdLuxShadowAPI::GetShadowFalloffGammaAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->inputsShadowFalloffGamma);
}

UsdAttribute UsdLuxShadowAPI::CreateShadowFalloffGammaAttr(VtValue const &defaultValue,
                                                           bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->inputsShadowFalloffGamma,
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
const TfTokenVector &UsdLuxShadowAPI::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdLuxTokens->inputsShadowEnable,
    UsdLuxTokens->inputsShadowColor,
    UsdLuxTokens->inputsShadowDistance,
    UsdLuxTokens->inputsShadowFalloff,
    UsdLuxTokens->inputsShadowFalloffGamma,
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

#include "wabi/usd/usdShade/connectableAPI.h"

WABI_NAMESPACE_BEGIN

UsdLuxShadowAPI::UsdLuxShadowAPI(const UsdShadeConnectableAPI &connectable)
  : UsdLuxShadowAPI(connectable.GetPrim())
{}

UsdShadeConnectableAPI UsdLuxShadowAPI::ConnectableAPI() const
{
  return UsdShadeConnectableAPI(GetPrim());
}

UsdShadeOutput UsdLuxShadowAPI::CreateOutput(const TfToken &name, const SdfValueTypeName &typeName)
{
  return UsdShadeConnectableAPI(GetPrim()).CreateOutput(name, typeName);
}

UsdShadeOutput UsdLuxShadowAPI::GetOutput(const TfToken &name) const
{
  return UsdShadeConnectableAPI(GetPrim()).GetOutput(name);
}

std::vector<UsdShadeOutput> UsdLuxShadowAPI::GetOutputs(bool onlyAuthored) const
{
  return UsdShadeConnectableAPI(GetPrim()).GetOutputs(onlyAuthored);
}

UsdShadeInput UsdLuxShadowAPI::CreateInput(const TfToken &name, const SdfValueTypeName &typeName)
{
  return UsdShadeConnectableAPI(GetPrim()).CreateInput(name, typeName);
}

UsdShadeInput UsdLuxShadowAPI::GetInput(const TfToken &name) const
{
  return UsdShadeConnectableAPI(GetPrim()).GetInput(name);
}

std::vector<UsdShadeInput> UsdLuxShadowAPI::GetInputs(bool onlyAuthored) const
{
  return UsdShadeConnectableAPI(GetPrim()).GetInputs(onlyAuthored);
}

WABI_NAMESPACE_END
