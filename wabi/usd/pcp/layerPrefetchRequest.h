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
#ifndef WABI_USD_PCP_LAYER_PREFETCH_REQUEST_H
#define WABI_USD_PCP_LAYER_PREFETCH_REQUEST_H

#include "wabi/usd/pcp/api.h"
#include "wabi/usd/sdf/layer.h"
#include "wabi/wabi.h"

#include <set>
#include <utility>

WABI_NAMESPACE_BEGIN

class Pcp_MutedLayers;

/// \class PcpLayerPrefetchRequest
///
/// PcpLayerPrefetchRequest represents a request to pre-fetch and retain
/// a set of layers in memory. It performs the fetch as a parallel
/// operation using a thread pool. Fetched layers are retained by the
/// request to keep them alive until the client can decide what to do
/// with them.
///
class PcpLayerPrefetchRequest {
 public:
  /// Enqueue a request to pre-fetch of the sublayers of \a layer.
  /// This recursively includes any nested sublayers. Arguments in \a args
  /// will be passed to Sdf when finding or opening sublayers.
  PCP_API
  void RequestSublayerStack(const SdfLayerRefPtr &layer,
                            const SdfLayer::FileFormatArguments &args);

  /// Run the queued requests, returning when complete.
  PCP_API
  void Run(const Pcp_MutedLayers &mutedLayers);

 private:
  // The request object retains both the layer requests and
  // the pre-fetched sublayers.
  typedef std::pair<SdfLayerRefPtr, SdfLayer::FileFormatArguments> _Request;
  std::set<_Request> _sublayerRequests;

  std::set<SdfLayerRefPtr> _retainedLayers;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_PCP_LAYER_PREFETCH_REQUEST_H
