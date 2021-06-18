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
#include "wabi/usd/usdLux/diskLight.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdLuxDiskLight, TfType::Bases<UsdLuxLight>>();

  // Register the usd prim typename as an alias under UsdSchemaBase. This
  // enables one to call
  // TfType::Find<UsdSchemaBase>().FindDerivedByName("DiskLight")
  // to find TfType<UsdLuxDiskLight>, which is how IsA queries are
  // answered.
  TfType::AddAlias<UsdSchemaBase, UsdLuxDiskLight>("DiskLight");
}

/* virtual */
UsdLuxDiskLight::~UsdLuxDiskLight()
{}

/* static */
UsdLuxDiskLight UsdLuxDiskLight::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage)
  {
    TF_CODING_ERROR("Invalid stage");
    return UsdLuxDiskLight();
  }
  return UsdLuxDiskLight(stage->GetPrimAtPath(path));
}

/* static */
UsdLuxDiskLight UsdLuxDiskLight::Define(const UsdStagePtr &stage, const SdfPath &path)
{
  static TfToken usdPrimTypeName("DiskLight");
  if (!stage)
  {
    TF_CODING_ERROR("Invalid stage");
    return UsdLuxDiskLight();
  }
  return UsdLuxDiskLight(stage->DefinePrim(path, usdPrimTypeName));
}

/* virtual */
UsdSchemaKind UsdLuxDiskLight::_GetSchemaKind() const
{
  return UsdLuxDiskLight::schemaKind;
}

/* virtual */
UsdSchemaKind UsdLuxDiskLight::_GetSchemaType() const
{
  return UsdLuxDiskLight::schemaType;
}

/* static */
const TfType &UsdLuxDiskLight::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdLuxDiskLight>();
  return tfType;
}

/* static */
bool UsdLuxDiskLight::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdLuxDiskLight::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdLuxDiskLight::GetRadiusAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->inputsRadius);
}

UsdAttribute UsdLuxDiskLight::CreateRadiusAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->inputsRadius,
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
const TfTokenVector &UsdLuxDiskLight::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdLuxTokens->inputsRadius,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(UsdLuxLight::GetSchemaAttributeNames(true),
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
