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
#include "wabi/usdImaging/usdImaging/coneAdapter.h"

#include "wabi/usdImaging/usdImaging/delegate.h"
#include "wabi/usdImaging/usdImaging/implicitSurfaceMeshUtils.h"
#include "wabi/usdImaging/usdImaging/indexProxy.h"
#include "wabi/usdImaging/usdImaging/tokens.h"

#include "wabi/imaging/hd/mesh.h"
#include "wabi/imaging/hd/meshTopology.h"
#include "wabi/imaging/hd/perfLog.h"
#include "wabi/imaging/hd/tokens.h"

#include "wabi/usd/usdGeom/cone.h"
#include "wabi/usd/usdGeom/xformCache.h"

#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN


TF_REGISTRY_FUNCTION(TfType)
{
  typedef UsdImagingConeAdapter Adapter;
  TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter>>();
  t.SetFactory<UsdImagingPrimAdapterFactory<Adapter>>();
}

UsdImagingConeAdapter::~UsdImagingConeAdapter() {}

bool UsdImagingConeAdapter::IsSupported(UsdImagingIndexProxy const *index) const
{
  return index->IsRprimTypeSupported(HdPrimTypeTokens->mesh);
}

SdfPath UsdImagingConeAdapter::Populate(UsdPrim const &prim,
                                        UsdImagingIndexProxy *index,
                                        UsdImagingInstancerContext const *instancerContext)
{
  return _AddRprim(HdPrimTypeTokens->mesh,
                   prim,
                   index,
                   GetMaterialUsdPath(prim),
                   instancerContext);
}

void UsdImagingConeAdapter::TrackVariability(
  UsdPrim const &prim,
  SdfPath const &cachePath,
  HdDirtyBits *timeVaryingBits,
  UsdImagingInstancerContext const *instancerContext) const
{
  BaseAdapter::TrackVariability(prim, cachePath, timeVaryingBits, instancerContext);

  // Check DirtyPoints before doing variability checks, in case we can skip
  // any of them...
  if ((*timeVaryingBits & HdChangeTracker::DirtyPoints) == 0) {
    _IsVarying(prim,
               UsdGeomTokens->height,
               HdChangeTracker::DirtyPoints,
               UsdImagingTokens->usdVaryingPrimvar,
               timeVaryingBits,
               /*inherited*/ false);
  }
  if ((*timeVaryingBits & HdChangeTracker::DirtyPoints) == 0) {
    _IsVarying(prim,
               UsdGeomTokens->radius,
               HdChangeTracker::DirtyPoints,
               UsdImagingTokens->usdVaryingPrimvar,
               timeVaryingBits,
               /*inherited*/ false);
  }
  if ((*timeVaryingBits & HdChangeTracker::DirtyPoints) == 0) {
    _IsVarying(prim,
               UsdGeomTokens->axis,
               HdChangeTracker::DirtyPoints,
               UsdImagingTokens->usdVaryingPrimvar,
               timeVaryingBits,
               /*inherited*/ false);
  }
}

HdDirtyBits UsdImagingConeAdapter::ProcessPropertyChange(UsdPrim const &prim,
                                                         SdfPath const &cachePath,
                                                         TfToken const &propertyName)
{
  if (propertyName == UsdGeomTokens->height || propertyName == UsdGeomTokens->radius ||
      propertyName == UsdGeomTokens->axis) {
    return HdChangeTracker::DirtyPoints;
  }

  // Allow base class to handle change processing.
  return BaseAdapter::ProcessPropertyChange(prim, cachePath, propertyName);
}

/*virtual*/
VtValue UsdImagingConeAdapter::GetPoints(UsdPrim const &prim, UsdTimeCode time) const
{
  return GetMeshPoints(prim, time);
}

static GfMatrix4d _GetImplicitGeomScaleTransform(UsdPrim const &prim, UsdTimeCode time)
{
  UsdGeomCone cone(prim);

  double height = 2.0;
  if (!cone.GetHeightAttr().Get(&height, time)) {
    TF_WARN("Could not evaluate double-valued height attribute on prim %s",
            prim.GetPath().GetText());
  }
  double radius = 1.0;
  if (!cone.GetRadiusAttr().Get(&radius, time)) {
    TF_WARN("Could not evaluate double-valued radius attribute on prim %s",
            prim.GetPath().GetText());
  }
  TfToken axis = UsdGeomTokens->z;
  if (!cone.GetAxisAttr().Get(&axis, time)) {
    TF_WARN("Could not evaluate token-valued axis attribute on prim %s", prim.GetPath().GetText());
  }

  return UsdImagingGenerateConeOrCylinderTransform(height, radius, axis);
}

/*static*/
VtValue UsdImagingConeAdapter::GetMeshPoints(UsdPrim const &prim, UsdTimeCode time)
{
  // Return scaled points (and not that of a unit geometry)
  VtVec3fArray points = UsdImagingGetUnitConeMeshPoints();
  GfMatrix4d scale = _GetImplicitGeomScaleTransform(prim, time);
  for (GfVec3f &pt : points) {
    pt = scale.Transform(pt);
  }

  return VtValue(points);
}

/*static*/
VtValue UsdImagingConeAdapter::GetMeshTopology()
{
  // Topology is constant and identical for all cones.
  return VtValue(HdMeshTopology(UsdImagingGetUnitConeMeshTopology()));
}

/*virtual*/
VtValue UsdImagingConeAdapter::GetTopology(UsdPrim const &prim,
                                           SdfPath const &cachePath,
                                           UsdTimeCode time) const
{
  TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();
  return GetMeshTopology();
}

WABI_NAMESPACE_END
