//
// Copyright 2017 Pixar
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
#include "wabi/usdImaging/usdImaging/domeLightAdapter.h"
#include "wabi/usdImaging/usdImaging/delegate.h"
#include "wabi/usdImaging/usdImaging/indexProxy.h"
#include "wabi/usdImaging/usdImaging/tokens.h"

#include "wabi/imaging/hd/tokens.h"

WABI_NAMESPACE_BEGIN


TF_REGISTRY_FUNCTION(TfType)
{
  typedef UsdImagingDomeLightAdapter Adapter;
  TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter>>();
  t.SetFactory<UsdImagingPrimAdapterFactory<Adapter>>();
}

UsdImagingDomeLightAdapter::~UsdImagingDomeLightAdapter() {}

bool UsdImagingDomeLightAdapter::IsSupported(UsdImagingIndexProxy const *index) const
{
  return UsdImagingLightAdapter::IsEnabledSceneLights() &&
         index->IsSprimTypeSupported(HdPrimTypeTokens->domeLight);
}

SdfPath UsdImagingDomeLightAdapter::Populate(UsdPrim const &prim,
                                             UsdImagingIndexProxy *index,
                                             UsdImagingInstancerContext const *instancerContext)
{
  index->InsertSprim(HdPrimTypeTokens->domeLight, prim.GetPath(), prim);
  HD_PERF_COUNTER_INCR(UsdImagingTokens->usdPopulatedPrimCount);

  return prim.GetPath();
}

void UsdImagingDomeLightAdapter::_RemovePrim(SdfPath const &cachePath, UsdImagingIndexProxy *index)
{
  index->RemoveSprim(HdPrimTypeTokens->domeLight, cachePath);
}


WABI_NAMESPACE_END
