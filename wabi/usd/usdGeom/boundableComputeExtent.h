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
#ifndef WABI_USD_USD_GEOM_BOUNDABLE_COMPUTE_EXTENT_H
#define WABI_USD_USD_GEOM_BOUNDABLE_COMPUTE_EXTENT_H

/// \file usdGeom/boundableComputeExtent.h

#include "wabi/usd/usdGeom/api.h"
#include "wabi/wabi.h"

#include "wabi/base/gf/vec3f.h"
#include "wabi/base/vt/array.h"

#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

class UsdGeomBoundable;
class UsdTimeCode;

/// Function registered with #UsdGeomRegisterComputeExtentFunction for
/// computing extents for a Boundable prim at the given time and filling
/// the given VtVec3fArray with the result.  If an optional transform matrix is
/// supplied, the extent is computed as if the object was first transformed by
/// the matrix. If the transform matrix is nullptr, the extent is computed as if
/// the identity matrix was passed.
///
/// The Boundable is guaranteed to be convertible to the prim type this
/// function was registered with.  The function must be thread-safe.
/// It should return true on success, false on failure.
using UsdGeomComputeExtentFunction = bool (*)(const UsdGeomBoundable &,
                                              const UsdTimeCode &,
                                              const GfMatrix4d *,
                                              VtVec3fArray *);

/// Registers \p fn as the function to use for computing extents for Boundable
/// prims of type \p PrimType by UsdGeomBoundable::ComputeExtentFromPlugins.
/// \p PrimType must derive from UsdGeomBoundable.
///
/// Plugins should generally call this function in a TF_REGISTRY_FUNCTION.
/// For example:
///
/// \code
/// TF_REGISTRY_FUNCTION(UsdGeomBoundable)
/// {
///     UsdGeomRegisterComputeExtentFunction<MyPrim>(MyComputeExtentFunction);
/// }
/// \endcode
///
/// Plugins must also note that this function is implemented for a prim type
/// in that type's schema definition.  For example:
///
/// \code
/// class "MyPrim" (
///     ...
///     customData = {
///         dictionary extraPlugInfo = {
///             bool implementsComputeExtent = true
///         }
///     }
///     ...
/// )
/// { ... }
/// \endcode
///
/// This allows the plugin system to discover this function dynamically
/// and load the plugin if needed.
template<class PrimType>
inline void UsdGeomRegisterComputeExtentFunction(const UsdGeomComputeExtentFunction &fn)
{
  static_assert(std::is_base_of<UsdGeomBoundable, PrimType>::value,
                "Prim type must derive from UsdGeomBoundable");

  UsdGeomRegisterComputeExtentFunction(TfType::Find<PrimType>(), fn);
}

/// \overload
USDGEOM_API
void UsdGeomRegisterComputeExtentFunction(const TfType &boundableType,
                                          const UsdGeomComputeExtentFunction &fn);

WABI_NAMESPACE_END

#endif  // WABI_USD_USD_GEOM_BOUNDABLE_COMPUTE_EXTENT_H
