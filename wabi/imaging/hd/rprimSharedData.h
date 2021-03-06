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
#ifndef WABI_IMAGING_HD_RPRIM_SHARED_DATA_H
#define WABI_IMAGING_HD_RPRIM_SHARED_DATA_H

#include "wabi/wabi.h"
#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/imaging/hd/bufferArrayRange.h"
#include "wabi/imaging/hd/tokens.h"

#include "wabi/usd/sdf/path.h"

#include "wabi/base/gf/bbox3d.h"
#include "wabi/base/vt/array.h"
#include "wabi/base/vt/types.h"

WABI_NAMESPACE_BEGIN

using TopologyToPrimvarVector = std::vector<std::pair<VtIntArray, std::vector<TfToken>>>;

// HdRprimSharedData is an assortment of data being shared across HdReprs,
// owned by HdRprim. HdDrawItem holds a const pointer to HdRprimSharedData.
//
// HdRprim
//   |
//   +--HdRepr(s)
//   |    |
//   |    +--HdDrawItem(s)-----.
//   |                         |
//   +--HdRprimSharedData  <---'
//

struct HdRprimSharedData
{
  HdRprimSharedData(int barContainerSize)
    : barContainer(barContainerSize),
      bounds(),
      instancerLevels(0),
      visible(true),
      rprimID()
  {}

  HdRprimSharedData(int barContainerSize, bool visible)
    : barContainer(barContainerSize),
      bounds(),
      instancerLevels(0),
      visible(visible),
      rprimID()
  {}

  // BufferArrayRange array
  HdBufferArrayRangeContainer barContainer;

  // Used for CPU frustum culling.
  GfBBox3d bounds;

  // The number of levels of instancing applied to this rprim.
  int instancerLevels;

  // Used for authored/delegate visibility.
  bool visible;

  // The owning Rprim's identifier.
  SdfPath rprimID;

  // Data structure containing the face-varying topologies of an rprim (mesh
  // only) and each of the topology's associated face-varying primvar names.
  // Used in drawing to determine which primvar uses which face-varying
  // channel.
  TopologyToPrimvarVector fvarTopologyToPrimvarVector;
};


WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_RPRIM_SHARED_DATA_H
