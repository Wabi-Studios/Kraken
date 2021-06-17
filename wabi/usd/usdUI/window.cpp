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

/* clang-format off */

#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/usdUI/window.h"
#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN
 
/**
 * Register the schema with the TfType system. */
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdUIWindow, TfType::Bases<UsdTyped>>();
  /**
   * Register the usd prim typename as an alias under UsdSchemaBase.
   * This enables one to call:
   * TfType::Find<UsdSchemaBase>().FindDerivedByName("Window")
   * To find TfType<UsdUIWindow>, which is how IsA queries are
   * answered. */
  TfType::AddAlias<UsdSchemaBase, UsdUIWindow>("Window");
}

/* virtual */
UsdUIWindow::~UsdUIWindow()
{}

/* static */
UsdUIWindow UsdUIWindow::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdUIWindow();
  }
  return UsdUIWindow(stage->GetPrimAtPath(path));
}

/* static */
UsdUIWindow UsdUIWindow::Define(const UsdStagePtr &stage, const SdfPath &path)
{
  static TfToken usdPrimTypeName("Window");
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdUIWindow();
  }
  return UsdUIWindow(stage->DefinePrim(path, usdPrimTypeName));
}
/* virtual */
UsdSchemaKind UsdUIWindow::_GetSchemaKind() const
{
  return UsdUIWindow::schemaKind;
}

/* virtual */
UsdSchemaKind UsdUIWindow::_GetSchemaType() const
{
  return UsdUIWindow::schemaType;
}

/* static */
const TfType &UsdUIWindow::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdUIWindow>();
  return tfType;
}

/* static */
bool UsdUIWindow::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdUIWindow::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdUIWindow::GetTitleAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowTitle);
}

UsdAttribute UsdUIWindow::CreateTitleAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowTitle,
    SdfValueTypeNames->Token,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetIconAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowIcon);
}

UsdAttribute UsdUIWindow::CreateIconAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowIcon,
    SdfValueTypeNames->Asset,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetTypeAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowType);
}

UsdAttribute UsdUIWindow::CreateTypeAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowType,
    SdfValueTypeNames->Token,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetStateAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowState);
}

UsdAttribute UsdUIWindow::CreateStateAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowState,
    SdfValueTypeNames->Token,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetDpiAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowDpi);
}

UsdAttribute UsdUIWindow::CreateDpiAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowDpi,
    SdfValueTypeNames->Float,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetDpifacAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowDpifac);
}

UsdAttribute UsdUIWindow::CreateDpifacAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowDpifac,
    SdfValueTypeNames->Float,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetScaleAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowScale);
}

UsdAttribute UsdUIWindow::CreateScaleAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowScale,
    SdfValueTypeNames->Float,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetPixelszAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowPixelsz);
}

UsdAttribute UsdUIWindow::CreatePixelszAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowPixelsz,
    SdfValueTypeNames->Float,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetWidgetunitAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowWidgetunit);
}

UsdAttribute UsdUIWindow::CreateWidgetunitAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowWidgetunit,
    SdfValueTypeNames->Float,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetLinewidthAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowLinewidth);
}

UsdAttribute UsdUIWindow::CreateLinewidthAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowLinewidth,
    SdfValueTypeNames->Float,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetCursorAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowCursor);
}

UsdAttribute UsdUIWindow::CreateCursorAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowCursor,
    SdfValueTypeNames->Token,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetPosAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowPos);
}

UsdAttribute UsdUIWindow::CreatePosAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowPos,
    SdfValueTypeNames->Float2,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdAttribute UsdUIWindow::GetSizeAttr() const
{
  return GetPrim().GetAttribute(UsdUITokens->uiWindowSize);
}

UsdAttribute UsdUIWindow::CreateSizeAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(
    UsdUITokens->uiWindowSize,
    SdfValueTypeNames->Float2,
    /* custom = */ false,
    SdfVariabilityUniform,
    defaultValue,
    writeSparsely);
}

UsdRelationship UsdUIWindow::GetUiWindowWorkspaceRel() const
{
  return GetPrim().GetRelationship(
    UsdUITokens->uiWindowWorkspace);
}

UsdRelationship UsdUIWindow::CreateUiWindowWorkspaceRel() const
{
  return GetPrim().CreateRelationship(
    UsdUITokens->uiWindowWorkspace,
    /* custom = */ false);
}

namespace {
static inline TfTokenVector _ConcatenateAttributeNames(const TfTokenVector& left,
                           const TfTokenVector& right)
{
  TfTokenVector result;
  result.reserve(left.size() + right.size());
  result.insert(result.end(), left.begin(), left.end());
  result.insert(result.end(), right.begin(), right.end());
  return result;
}
}  /* anonymous */

/*static*/
const TfTokenVector& UsdUIWindow::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdUITokens->uiWindowTitle,
    UsdUITokens->uiWindowIcon,
    UsdUITokens->uiWindowType,
    UsdUITokens->uiWindowState,
    UsdUITokens->uiWindowDpi,
    UsdUITokens->uiWindowDpifac,
    UsdUITokens->uiWindowScale,
    UsdUITokens->uiWindowPixelsz,
    UsdUITokens->uiWindowWidgetunit,
    UsdUITokens->uiWindowLinewidth,
    UsdUITokens->uiWindowCursor,
    UsdUITokens->uiWindowPos,
    UsdUITokens->uiWindowSize,
  };
  static TfTokenVector allNames =
    _ConcatenateAttributeNames(UsdTyped::GetSchemaAttributeNames(true), localNames);

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
