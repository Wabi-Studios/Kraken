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
#ifndef WABI_USD_IMAGING_USD_IMAGING_INSTANCER_CONTEXT_H
#define WABI_USD_IMAGING_USD_IMAGING_INSTANCER_CONTEXT_H

/// \file usdImaging/instancerContext.h

#include "wabi/wabi.h"
#include "wabi/usdImaging/usdImaging/api.h"
#include "wabi/usd/sdf/path.h"

WABI_NAMESPACE_BEGIN


using UsdImagingPrimAdapterSharedPtr = std::shared_ptr<class UsdImagingPrimAdapter>;

/// \class UsdImagingInstancerContext
///
/// Object used by instancer prim adapters to pass along context
/// about the instancer and instance prim to prototype prim adapters.
///
class UsdImagingInstancerContext
{
 public:

  /// The cachePath of the instancer.
  SdfPath instancerCachePath;

  /// The name of the child prim, typically used for prototypes.
  TfToken childName;

  /// The USD path to the material bound to the instance prim
  /// being processed.
  SdfPath instancerMaterialUsdPath;

  /// The draw mode bound to the instance prim being processed.
  TfToken instanceDrawMode;

  // The inheritable purpose bound to the instance prim being processed. If
  // the instance prim can provide this, prototypes without an explicit or
  // inherited purpose will inherit this purpose from the instance.
  TfToken instanceInheritablePurpose;

  /// The instancer's prim Adapter. Useful when an adapter is needed, but the
  /// default adapter may be overridden for the sake of instancing.
  UsdImagingPrimAdapterSharedPtr instancerAdapter;
};


WABI_NAMESPACE_END

#endif  // WABI_USD_IMAGING_USD_IMAGING_INSTANCER_CONTEXT_H
