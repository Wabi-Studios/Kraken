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
#include "wabi/usdImaging/usdImaging/sphereAdapter.h"

#include "wabi/usdImaging/usdImaging/delegate.h"
#include "wabi/usdImaging/usdImaging/implicitSurfaceMeshUtils.h"
#include "wabi/usdImaging/usdImaging/indexProxy.h"
#include "wabi/usdImaging/usdImaging/tokens.h"

#include "wabi/imaging/hd/mesh.h"
#include "wabi/imaging/hd/meshTopology.h"
#include "wabi/imaging/hd/perfLog.h"
#include "wabi/imaging/hd/tokens.h"

#include "wabi/usd/usdGeom/sphere.h"
#include "wabi/usd/usdGeom/xformCache.h"

#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN


TF_REGISTRY_FUNCTION(TfType)
{
  typedef UsdImagingSphereAdapter Adapter;
  TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter>>();
  t.SetFactory<UsdImagingPrimAdapterFactory<Adapter>>();
}

UsdImagingSphereAdapter::~UsdImagingSphereAdapter() {}

bool UsdImagingSphereAdapter::IsSupported(UsdImagingIndexProxy const *index) const
{
  return index->IsRprimTypeSupported(HdPrimTypeTokens->mesh);
}

SdfPath UsdImagingSphereAdapter::Populate(UsdPrim const &prim,
                                          UsdImagingIndexProxy *index,
                                          UsdImagingInstancerContext const *instancerContext)
{
  return _AddRprim(HdPrimTypeTokens->mesh,
                   prim,
                   index,
                   GetMaterialUsdPath(prim),
                   instancerContext);
}

void UsdImagingSphereAdapter::TrackVariability(
  UsdPrim const &prim,
  SdfPath const &cachePath,
  HdDirtyBits *timeVaryingBits,
  UsdImagingInstancerContext const *instancerContext) const
{
  BaseAdapter::TrackVariability(prim, cachePath, timeVaryingBits, instancerContext);

  // Check DirtyPoints before doing variability checks, to see if we can skip.
  if ((*timeVaryingBits & HdChangeTracker::DirtyPoints) == 0) {
    _IsVarying(prim,
               UsdGeomTokens->radius,
               HdChangeTracker::DirtyPoints,
               UsdImagingTokens->usdVaryingPrimvar,
               timeVaryingBits,
               /*inherited*/ false);
  }
}

HdDirtyBits UsdImagingSphereAdapter::ProcessPropertyChange(UsdPrim const &prim,
                                                           SdfPath const &cachePath,
                                                           TfToken const &propertyName)
{
  if (propertyName == UsdGeomTokens->radius) {
    return HdChangeTracker::DirtyPoints;
  }

  // Allow base class to handle change processing.
  return BaseAdapter::ProcessPropertyChange(prim, cachePath, propertyName);
}

/*virtual*/
VtValue UsdImagingSphereAdapter::GetPoints(UsdPrim const &prim, UsdTimeCode time) const
{
  return GetMeshPoints(prim, time);
}

static GfMatrix4d _GetImplicitGeomScaleTransform(UsdPrim const &prim, UsdTimeCode time)
{
  UsdGeomSphere sphere(prim);

  double radius = 1.0;
  if (!sphere.GetRadiusAttr().Get(&radius, time)) {
    TF_WARN("Could not evaluate double-valued radius attribute on prim %s",
            prim.GetPath().GetText());
  }

  return UsdImagingGenerateSphereOrCubeTransform(2.0 * radius);
}

/*static*/
VtValue UsdImagingSphereAdapter::GetMeshPoints(UsdPrim const &prim, UsdTimeCode time)
{
  // Return scaled points (and not that of a unit geometry)
  VtVec3fArray points = UsdImagingGetUnitSphereMeshPoints();
  GfMatrix4d scale = _GetImplicitGeomScaleTransform(prim, time);
  for (GfVec3f &pt : points) {
    pt = scale.Transform(pt);
  }

  return VtValue(points);
}

/*static*/
VtValue UsdImagingSphereAdapter::GetMeshTopology()
{
  // Topology is constant and identical for all spheres.
  return VtValue(HdMeshTopology(UsdImagingGetUnitSphereMeshTopology()));
}

/*virtual*/
VtValue UsdImagingSphereAdapter::GetTopology(UsdPrim const &prim,
                                             SdfPath const &cachePath,
                                             UsdTimeCode time) const
{
  TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  return GetMeshTopology();
}

WABI_NAMESPACE_END
