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
#ifndef WABI_USD_SDF_LAYER_UTILS_H
#define WABI_USD_SDF_LAYER_UTILS_H

/// \file sdf/layerUtils.h

#include "wabi/usd/sdf/api.h"
#include "wabi/usd/sdf/declareHandles.h"
#include "wabi/usd/sdf/layer.h"
#include "wabi/wabi.h"

#include <string>

WABI_NAMESPACE_BEGIN

SDF_DECLARE_HANDLES(SdfLayer);

/// Returns the path to the asset specified by \p assetPath, using the
/// \p anchor layer to anchor the path if it is relative.  If the result of
/// anchoring \p assetPath to \p anchor's path cannot be resolved and
/// \p assetPath is a search path, \p assetPath will be returned.  If
/// \p assetPath is not relative, \p assetPath will be returned.  Otherwise,
/// the anchored path will be returned.
///
/// Note that if \p anchor is an anonymous layer, we will always return
/// the untouched \p assetPath.
SDF_API std::string SdfComputeAssetPathRelativeToLayer(const SdfLayerHandle &anchor,
                                                       const std::string &assetPath);

WABI_NAMESPACE_END

#endif  // WABI_USD_SDF_LAYER_UTILS_H
