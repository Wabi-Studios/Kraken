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
#include "wabi/usd/usdGeom/cylinder.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdGeomCylinder, TfType::Bases<UsdGeomGprim>>();

  // Register the usd prim typename as an alias under UsdSchemaBase. This
  // enables one to call
  // TfType::Find<UsdSchemaBase>().FindDerivedByName("Cylinder")
  // to find TfType<UsdGeomCylinder>, which is how IsA queries are
  // answered.
  TfType::AddAlias<UsdSchemaBase, UsdGeomCylinder>("Cylinder");
}

/* virtual */
UsdGeomCylinder::~UsdGeomCylinder()
{}

/* static */
UsdGeomCylinder UsdGeomCylinder::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdGeomCylinder();
  }
  return UsdGeomCylinder(stage->GetPrimAtPath(path));
}

/* static */
UsdGeomCylinder UsdGeomCylinder::Define(const UsdStagePtr &stage, const SdfPath &path)
{
  static TfToken usdPrimTypeName("Cylinder");
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdGeomCylinder();
  }
  return UsdGeomCylinder(stage->DefinePrim(path, usdPrimTypeName));
}

/* virtual */
UsdSchemaKind UsdGeomCylinder::_GetSchemaKind() const
{
  return UsdGeomCylinder::schemaKind;
}

/* virtual */
UsdSchemaKind UsdGeomCylinder::_GetSchemaType() const
{
  return UsdGeomCylinder::schemaType;
}

/* static */
const TfType &UsdGeomCylinder::_GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdGeomCylinder>();
  return tfType;
}

/* static */
bool UsdGeomCylinder::_IsTypedSchema()
{
  static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdGeomCylinder::_GetTfType() const
{
  return _GetStaticTfType();
}

UsdAttribute UsdGeomCylinder::GetHeightAttr() const
{
  return GetPrim().GetAttribute(UsdGeomTokens->height);
}

UsdAttribute UsdGeomCylinder::CreateHeightAttr(VtValue const &defaultValue,
                                               bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdGeomTokens->height,
                                    SdfValueTypeNames->Double,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdGeomCylinder::GetRadiusAttr() const
{
  return GetPrim().GetAttribute(UsdGeomTokens->radius);
}

UsdAttribute UsdGeomCylinder::CreateRadiusAttr(VtValue const &defaultValue,
                                               bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdGeomTokens->radius,
                                    SdfValueTypeNames->Double,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdGeomCylinder::GetAxisAttr() const
{
  return GetPrim().GetAttribute(UsdGeomTokens->axis);
}

UsdAttribute UsdGeomCylinder::CreateAxisAttr(VtValue const &defaultValue, bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdGeomTokens->axis,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityUniform,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdGeomCylinder::GetExtentAttr() const
{
  return GetPrim().GetAttribute(UsdGeomTokens->extent);
}

UsdAttribute UsdGeomCylinder::CreateExtentAttr(VtValue const &defaultValue,
                                               bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdGeomTokens->extent,
                                    SdfValueTypeNames->Float3Array,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

namespace {
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
const TfTokenVector &UsdGeomCylinder::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
      UsdGeomTokens->height,
      UsdGeomTokens->radius,
      UsdGeomTokens->axis,
      UsdGeomTokens->extent,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(
      UsdGeomGprim::GetSchemaAttributeNames(true), localNames);

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

#include "wabi/base/tf/registryManager.h"
#include "wabi/usd/usdGeom/boundableComputeExtent.h"

WABI_NAMESPACE_BEGIN

static bool _ComputeExtentMax(double height, double radius, const TfToken &axis, GfVec3f *max)
{
  if (axis == UsdGeomTokens->x) {
    *max = GfVec3f(height * 0.5, radius, radius);
  }
  else if (axis == UsdGeomTokens->y) {
    *max = GfVec3f(radius, height * 0.5, radius);
  }
  else if (axis == UsdGeomTokens->z) {
    *max = GfVec3f(radius, radius, height * 0.5);
  }
  else {
    return false;  // invalid axis
  }

  return true;
}

bool UsdGeomCylinder::ComputeExtent(double height,
                                    double radius,
                                    const TfToken &axis,
                                    VtVec3fArray *extent)
{
  // Create Sized Extent
  extent->resize(2);

  GfVec3f max;
  if (!_ComputeExtentMax(height, radius, axis, &max)) {
    return false;
  }

  (*extent)[0] = -max;
  (*extent)[1] = max;

  return true;
}

bool UsdGeomCylinder::ComputeExtent(double height,
                                    double radius,
                                    const TfToken &axis,
                                    const GfMatrix4d &transform,
                                    VtVec3fArray *extent)
{
  // Create Sized Extent
  extent->resize(2);

  GfVec3f max;
  if (!_ComputeExtentMax(height, radius, axis, &max)) {
    return false;
  }

  GfBBox3d bbox   = GfBBox3d(GfRange3d(-max, max), transform);
  GfRange3d range = bbox.ComputeAlignedRange();
  (*extent)[0]    = GfVec3f(range.GetMin());
  (*extent)[1]    = GfVec3f(range.GetMax());

  return true;
}

static bool _ComputeExtentForCylinder(const UsdGeomBoundable &boundable,
                                      const UsdTimeCode &time,
                                      const GfMatrix4d *transform,
                                      VtVec3fArray *extent)
{
  const UsdGeomCylinder cylinderSchema(boundable);
  if (!TF_VERIFY(cylinderSchema)) {
    return false;
  }

  double height;
  if (!cylinderSchema.GetHeightAttr().Get(&height, time)) {
    return false;
  }

  double radius;
  if (!cylinderSchema.GetRadiusAttr().Get(&radius, time)) {
    return false;
  }

  TfToken axis;
  if (!cylinderSchema.GetAxisAttr().Get(&axis, time)) {
    return false;
  }

  if (transform) {
    return UsdGeomCylinder::ComputeExtent(height, radius, axis, *transform, extent);
  }
  else {
    return UsdGeomCylinder::ComputeExtent(height, radius, axis, extent);
  }
}

TF_REGISTRY_FUNCTION(UsdGeomBoundable)
{
  UsdGeomRegisterComputeExtentFunction<UsdGeomCylinder>(_ComputeExtentForCylinder);
}

WABI_NAMESPACE_END
