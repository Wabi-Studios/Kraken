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

#include "wabi/usd/pcp/layerPrefetchRequest.h"
#include "wabi/base/work/dispatcher.h"
#include "wabi/base/work/threadLimits.h"
#include "wabi/base/work/withScopedParallelism.h"
#include "wabi/usd/pcp/layerStackRegistry.h"
#include "wabi/wabi.h"

#include <tbb/spin_mutex.h>

WABI_NAMESPACE_BEGIN

namespace {

struct _Opener {
  explicit _Opener(const Pcp_MutedLayers &mutedLayers, std::set<SdfLayerRefPtr> *retainedLayers)
      : _mutedLayers(mutedLayers),
        _retainedLayers(retainedLayers)
  {}

  ~_Opener()
  {
    _dispatcher.Wait();
  }

  void OpenSublayers(const SdfLayerRefPtr &layer, const SdfLayer::FileFormatArguments &layerArgs)
  {
    TF_FOR_ALL(path, layer->GetSubLayerPaths())
    {
      _dispatcher.Run(&_Opener::_OpenSublayer, this, *path, layer, layerArgs);
    }
  }

 private:
  void _OpenSublayer(std::string path,
                     const SdfLayerRefPtr &anchorLayer,
                     const SdfLayer::FileFormatArguments &layerArgs)
  {
    if (_mutedLayers.IsLayerMuted(anchorLayer, path)) {
      return;
    }

    // Open this specific sublayer path.
    // The call to SdfLayer::FindOrOpenRelativeToLayer() may take some
    // time, potentially multiple seconds.
    if (SdfLayerRefPtr sublayer = SdfLayer::FindOrOpenRelativeToLayer(
            anchorLayer, path, layerArgs)) {
      // Retain this sublayer.
      bool didInsert;
      {
        tbb::spin_mutex::scoped_lock lock(_retainedLayersMutex);
        didInsert = _retainedLayers->insert(sublayer).second;
      }
      // Open the nested sublayers.  Only do this if we haven't seen this
      // layer before, i.e. didInsert is true.
      if (didInsert)
        OpenSublayers(sublayer, layerArgs);
    }
  }

  WorkDispatcher _dispatcher;
  const Pcp_MutedLayers &_mutedLayers;
  std::set<SdfLayerRefPtr> *_retainedLayers;
  mutable tbb::spin_mutex _retainedLayersMutex;
};

}  // namespace

void PcpLayerPrefetchRequest::RequestSublayerStack(const SdfLayerRefPtr &layer,
                                                   const SdfLayer::FileFormatArguments &args)
{
  _sublayerRequests.insert(std::make_pair(layer, args));
}

void PcpLayerPrefetchRequest::Run(const Pcp_MutedLayers &mutedLayers)
{
  if (!WorkHasConcurrency()) {
    // Do not bother pre-fetching if we do not have extra threads
    // available.
    return;
  }

  // Release the GIL so we don't deadlock when Sd tries to get a path
  // resolver (which does ref-counting on the resolver, which requires
  // the GIL to manage TfRefBase identity-uniqueness).
  TF_PY_ALLOW_THREADS_IN_SCOPE();

  std::set<_Request> requests;
  requests.swap(_sublayerRequests);

  // Open all the sublayers in the request.
  WorkWithScopedParallelism([&]() {
    _Opener opener(mutedLayers, &_retainedLayers);
    TF_FOR_ALL(req, requests)
    opener.OpenSublayers(req->first, req->second);
  });
}

WABI_NAMESPACE_END
