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
#include "wabi/usd/usdUI/workspace.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/types.h"
#include "wabi/usd/sdf/assetPath.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdUIWorkspace, TfType::Bases<UsdTyped>>();

  // Register the usd prim typename as an alias under UsdSchemaBase. This
  // enables one to call
  // TfType::Find<UsdSchemaBase>().FindDerivedByName("Workspace")
  // to find TfType<UsdUIWorkspace>, which is how IsA queries are
  // answered.
  TfType::AddAlias<UsdSchemaBase, UsdUIWorkspace>("Workspace");
}

/* virtual */
UsdUIWorkspace::~UsdUIWorkspace() {}

/* static */
UsdUIWorkspace UsdUIWorkspace::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdUIWorkspace();
  }
  return UsdUIWorkspace(stage->GetPrimAtPath(path));
}

/* static */
UsdUIWorkspace UsdUIWorkspace::Define(const UsdStagePtr &stage, const SdfPath &path)
{
  static TfToken usdPrimTypeName("Workspace");
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdUIWorkspace();
  }
  return UsdUIWorkspace(stage->DefinePrim(path, usdPrimTypeName));
}

/* virtual */
UsdSchemaKind UsdUIWorkspace::_GetSchemaKind() const
{
  return UsdUIWorkspace::schemaKind;
}

/* static */
const TfType &UsdUIWorkspace::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdUIWorkspace>();
  return tfType;
}

/* static */
bool UsdUIWorkspace::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdUIWorkspace::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdUIWorkspace::GetNameAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWorkspaceName);
}

UsdAttribute UsdUIWorkspace::CreateNameAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdUITokens->uiWorkspaceName,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityUniform,
                                    defaultValue,
                                    writeSparsely);
}

UsdRelationship UsdUIWorkspace::GetScreenRel() const
{
  return GetPrim().GetRelationship(UsdUITokens->uiWorkspaceScreen);
}

UsdRelationship UsdUIWorkspace::CreateScreenRel() const
{
  return GetPrim().CreateRelationship(UsdUITokens->uiWorkspaceScreen,
                                      /* custom = */ false);
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
const TfTokenVector &UsdUIWorkspace::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdUITokens->uiWorkspaceName,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(
    UsdTyped::GetSchemaAttributeNames(true),
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
