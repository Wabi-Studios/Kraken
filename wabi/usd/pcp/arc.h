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
#ifndef WABI_USD_PCP_ARC_H
#define WABI_USD_PCP_ARC_H

#include "wabi/wabi.h"
#include "wabi/usd/pcp/api.h"
#include "wabi/usd/pcp/mapExpression.h"
#include "wabi/usd/pcp/node.h"
#include "wabi/usd/pcp/types.h"
#include "wabi/usd/sdf/path.h"

WABI_NAMESPACE_BEGIN

class PcpPrimIndex_Graph;

/// \class PcpArc
///
/// Represents an arc connecting two nodes in the prim index.
/// The arc is owned by a node (source) and points to its parent node (target)
/// in the index.
///
class PcpArc
{
 public:

  PcpArc() : type(PcpArcTypeRoot), siblingNumAtOrigin(0), namespaceDepth(0) {}

  /// The type of this arc.
  PcpArcType type;

  /// The parent (or target) node of this arc.
  /// If this arc's source node is a root node (i.e., type == PcpArcTypeRoot),
  /// this will be an invalid node.
  PcpNodeRef parent;

  /// The origin node of this arc.
  /// This is the node that caused this arc's source node to be brought into
  /// the prim index. In most cases, this will be the same as the parent node.
  /// For implied inherits, this is the node from which this inherit arc was
  /// propagated. This affects strength ordering.
  PcpNodeRef origin;

  /// The value-mapping function used to map values from this arc's source
  /// node to its parent node.
  PcpMapExpression mapToParent;

  // index among sibling arcs at origin; lower is stronger
  int siblingNumAtOrigin;

  // Absolute depth in namespace of node that introduced this node.
  // Note that this does *not* count any variant selections.
  int namespaceDepth;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_PCP_ARC_H
