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

#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"
#include "wabi/usd/usdUI/backdrop.h"

WABI_NAMESPACE_BEGIN

/**
 * Register the schema with the TfType system. */
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdUIBackdrop, TfType::Bases<UsdTyped>>();
  /**
   * Register the usd prim typename as an alias under UsdSchemaBase.
   * This enables one to call:
   * TfType::Find<UsdSchemaBase>().FindDerivedByName("Backdrop")
   * To find TfType<UsdUIBackdrop>, which is how IsA queries are
   * answered. */
  TfType::AddAlias<UsdSchemaBase, UsdUIBackdrop>("Backdrop");
}

/* virtual */
UsdUIBackdrop::~UsdUIBackdrop()
{}

/* static */
UsdUIBackdrop UsdUIBackdrop::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdUIBackdrop();
  }
  return UsdUIBackdrop(stage->GetPrimAtPath(path));
}

/* static */
UsdUIBackdrop UsdUIBackdrop::Define(const UsdStagePtr &stage, const SdfPath &path)
{
  static TfToken usdPrimTypeName("Backdrop");
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdUIBackdrop();
  }
  return UsdUIBackdrop(stage->DefinePrim(path, usdPrimTypeName));
}
/* virtual */
UsdSchemaKind UsdUIBackdrop::_GetSchemaKind() const
{
  return UsdUIBackdrop::schemaKind;
}

/* virtual */
UsdSchemaKind UsdUIBackdrop::_GetSchemaType() const
{
  return UsdUIBackdrop::schemaType;
}

/* static */
const TfType &UsdUIBackdrop::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdUIBackdrop>();
  return tfType;
}

/* static */
bool UsdUIBackdrop::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdUIBackdrop::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdUIBackdrop::GetDescriptionAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiDescription);
}

UsdAttribute UsdUIBackdrop::CreateDescriptionAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdUITokens->uiDescription,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityUniform,
                                    defaultValue,
                                    writeSparsely);
}

namespace {
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
const TfTokenVector &UsdUIBackdrop::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdUITokens->uiDescription,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(UsdTyped::GetSchemaAttributeNames(true),
                                                             localNames);

  if (includeInherited)
    return allNames;
  else
    return localNames;
}

WABI_NAMESPACE_END

/* clang-format off */

  /**
   * ======================================================================
   *   Feel free to add custom code below this line. It will be preserved
   *   by the code generator.
   *
   *   Just remember to wrap code in the appropriate delimiters:
   *     - 'WABI_NAMESPACE_BEGIN', 'WABI_NAMESPACE_END'.
   * ======================================================================
   * --(BEGIN CUSTOM CODE)-- */

/* clang-format on */
