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
#include "wabi/imaging/hdx/selectionTracker.h"

#include "wabi/imaging/hdx/debugCodes.h"

#include "wabi/base/trace/trace.h"
#include "wabi/base/work/loops.h"
#include "wabi/imaging/hd/renderIndex.h"
#include "wabi/imaging/hd/rprim.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>

WABI_NAMESPACE_BEGIN

HdxSelectionTracker::HdxSelectionTracker() : _version(0)
{}

HdxSelectionTracker::~HdxSelectionTracker() = default;

/*virtual*/
void HdxSelectionTracker::UpdateSelection(HdRenderIndex *index)
{}

int HdxSelectionTracker::GetVersion() const
{
  return _version;
}

void HdxSelectionTracker::_IncrementVersion()
{
  ++_version;
}

namespace {
template<class T>
void _DebugPrintArray(std::string const &name, T const &array, bool withIndex = true)
{
  if (ARCH_UNLIKELY(TfDebug::IsEnabled(HDX_SELECTION_SETUP))) {
    std::stringstream out;
    out << name << ": [ ";
    for (auto const &i : array) {
      out << std::setfill(' ') << std::setw(3) << i << " ";
    }
    out << "] (offsets)" << std::endl;

    if (withIndex) {
      // Print the indices
      out << name << ": [ ";
      for (size_t i = 0; i < array.size(); i++) {
        out << std::setfill(' ') << std::setw(3) << i << " ";
      }
      out << "] (indices)" << std::endl;
      out << std::endl;
      std::cout << out.str();
    }
  }
}
}  // namespace

/*virtual*/
bool HdxSelectionTracker::GetSelectionOffsetBuffer(HdRenderIndex const *index,
                                                   bool enableSelection,
                                                   VtIntArray *offsets) const
{
  TRACE_FUNCTION();
  TfAutoMallocTag2 tag("Hdx", "GetSelectionOffsetBuffer");

  // XXX: Set minimum size for UBO/SSBO requirements. Seems like this should
  // be handled by Hydra. Update all uses of minSize below when resolved.
  const int minSize = 8;
  offsets->resize(minSize);

  // Populate a selection offset buffer that holds offset data per selection
  // highlight mode. See 'Buffer Layout' for the per-mode layout.
  //
  // The full layout is:
  // [# modes] [per-mode offsets] [seloffsets mode0] ... [seloffsets modeM]
  // [--------  header  --------]
  //
  // Example:
  //   [2 ]         [4,30]       [seloffsets mode0] [seloffsets mode1]
  //                 |  |         ^                  ^
  //                 |  |_________|__________________|
  //                 |____________|
  //
  //  Above:
  //  Index 0 holds the number of selection highlight modes (2)
  //  Indices [1,2] hold the start index for each mode's data.
  //  If a mode doesn't have any selected items, we use 0 to encode this.
  //  See hdx/shaders/renderPass.glslfx (ApplySelectionColor) for the
  //  shader readback of this buffer

  bool hasSelection              = false;
  const size_t numHighlightModes = static_cast<size_t>(HdSelection::HighlightModeCount);
  // 1 for num modes, plus one per each mode for mode offset.
  const size_t headerSize = numHighlightModes + 1;
  const int SELECT_NONE   = 0;

  if (ARCH_UNLIKELY(numHighlightModes >= minSize)) {
    // allocate enough to hold the header
    offsets->resize(headerSize);
  }

  (*offsets)[0] = numHighlightModes;

  // We expect the collection of selected items to be created externally and
  // set via SetSelection. Exit early if the tracker doesn't have one set,
  // or it's empty. Likewise if enableSelection is false.
  if (!_selection || !enableSelection || _selection->IsEmpty()) {
    for (int mode = HdSelection::HighlightModeSelect; mode < HdSelection::HighlightModeCount;
         mode++) {
      (*offsets)[mode + 1] = SELECT_NONE;
    }
    _DebugPrintArray("nothing selected", *offsets);
    return false;
  }

  size_t copyOffset = headerSize;

  for (int mode = HdSelection::HighlightModeSelect; mode < HdSelection::HighlightModeCount;
       mode++) {

    std::vector<int> output;
    bool modeHasSelection = _GetSelectionOffsets(
        static_cast<HdSelection::HighlightMode>(mode), index, copyOffset, &output);
    hasSelection = hasSelection || modeHasSelection;

    (*offsets)[mode + 1] = modeHasSelection ? copyOffset : SELECT_NONE;

    if (modeHasSelection) {
      // append the offset buffer for the highlight mode
      offsets->resize(output.size() + copyOffset);

      for (size_t i = 0; i < output.size(); i++) {
        (*offsets)[i + copyOffset] = output[i];
      }

      copyOffset += output.size();

      TF_DEBUG(HDX_SELECTION_SETUP)
          .Msg(
              "Highlight mode %d has %lu "
              "entries\n",
              mode,
              output.size());
    }
  }

  _DebugPrintArray("final output", *offsets);
  return hasSelection;
}

/*virtual*/
VtVec4fArray HdxSelectionTracker::GetSelectedPointColors() const
{
  if (!_selection)
    return VtVec4fArray();

  std::vector<GfVec4f> const &pointColors = _selection->GetSelectedPointColors();

  VtVec4fArray vtColors(pointColors.size());
  std::copy(pointColors.begin(), pointColors.end(), vtColors.begin());

  return vtColors;
}

// Helper methods to fill the selection buffer
static std::pair<int, int> _GetMinMax(std::vector<VtIntArray> const &vecIndices)
{
  int min = std::numeric_limits<int>::max();
  int max = std::numeric_limits<int>::lowest();

  for (VtIntArray const &indices : vecIndices) {
    for (int const &id : indices) {
      min = std::min(min, id);
      max = std::max(max, id);
    }
  }

  return std::pair<int, int>(min, max);
}

// The selection offsets in the buffer encode two pieces of information:
// (a) isSelected (bit 0)
//     This tells us if whatever we're encoding (prim/instance/subprim) is
//     selected.
// (b) offset (bits 31:1)
//     This tells us the offset to jump to, which may in turn encode either
//     instance/subprim selection state, thus allowing us to support selection
//     of multiple subprims.
//     If the offset is 0, it means there is nothing more to decode.
//     For points alone, the offset is overloaded to represent the point color
//     index for customized highlighting (or -1 if a color isn't specified;
//     see HdSelection::AddPoints)
static int _EncodeSelOffset(size_t offset, bool isSelected)
{
  return int(offset << 1) | static_cast<int>(isSelected);
}

// This function takes care of encoding subprim selection offsets.
// Returns true if output was filled, and false if not.
static bool _FillSubprimSelOffsets(int type,
                                   std::vector<VtIntArray> const &vecIndices,
                                   int nextSubprimOffset,
                                   std::vector<int> *output)
{
  // Nothing to do if we have no indices arrays.
  // Also worth noting that the HdxSelection::Add<Subprim> methods ensure
  // empty indices arrays aren't inserted.
  if (vecIndices.empty())
    return false;

  std::pair<int, int> minmax = _GetMinMax(vecIndices);
  int const &min             = minmax.first;
  int const &max             = minmax.second;

  // Each subprims offsets' buffer encoding is:
  // [subprim-type][min][max][     selOffsets     ]
  // <----------3 ----------><--- max - min + 1 -->
  int const SUBPRIM_SELOFFSETS_HEADER_SIZE = 3;
  bool const SELECT_ALL                    = 1;
  bool const SELECT_NONE                   = 0;
  int numOffsetsToInsert                   = SUBPRIM_SELOFFSETS_HEADER_SIZE + (max - min + 1);
  size_t startOutputSize                   = output->size();

  // Grow by the total size and then fill the header (to avoid an additional
  // insert operation)
  output->insert(
      output->end(), numOffsetsToInsert, _EncodeSelOffset(nextSubprimOffset, SELECT_NONE));

  (*output)[startOutputSize]     = type;
  (*output)[startOutputSize + 1] = min;
  (*output)[startOutputSize + 2] = max + 1;

  // For those subprim indices that are selected, set their LSB to 1.
  size_t selOffsetsStart = startOutputSize + SUBPRIM_SELOFFSETS_HEADER_SIZE;
  for (VtIntArray const &indices : vecIndices) {
    for (int const &id : indices) {
      if (id >= 0) {
        (*output)[selOffsetsStart + (id - min)] |= SELECT_ALL;
      }
    }
  }

  return true;
}

// Encode subprim selection offsets for points, with the offset representing the
// index of the point color to be used for custom point selection highlighting.
// Note: When a color isn't specified (see HdSelection::AddPoints), an index of
// -1 is used.
static bool _FillPointSelOffsets(int type,
                                 std::vector<VtIntArray> const &pointIndices,
                                 std::vector<int> const &pointColorIndices,
                                 std::vector<int> *output)
{
  size_t startOutputSize = output->size();
  bool hasSelectedPoints = _FillSubprimSelOffsets(type,
                                                  pointIndices,
                                                  /*nextSubprimOffset=*/0,
                                                  output);
  if (hasSelectedPoints) {
    // Update the 'offset' part of selOffset for each of the selected
    // points to represent the point color index for customized point
    // selection highlighting.
    std::pair<int, int> minmax               = _GetMinMax(pointIndices);
    int const &min                           = minmax.first;
    int const SUBPRIM_SELOFFSETS_HEADER_SIZE = 3;
    bool const SELECT_ALL                    = 1;
    size_t selOffsetsStart                   = startOutputSize + SUBPRIM_SELOFFSETS_HEADER_SIZE;
    size_t vtIndex                           = 0;
    for (VtIntArray const &indices : pointIndices) {
      int pointColorId = pointColorIndices[vtIndex++];
      int selOffset    = (pointColorId << 1) | SELECT_ALL;
      for (int const &id : indices) {
        (*output)[selOffsetsStart + (id - min)] = selOffset;
      }
    }
  }

  return hasSelectedPoints;
}

namespace {

constexpr int INVALID = -1;

}

/*virtual*/
bool HdxSelectionTracker::_GetSelectionOffsets(HdSelection::HighlightMode const &mode,
                                               HdRenderIndex const *index,
                                               size_t modeOffset,
                                               std::vector<int> *output) const
{

  const SdfPathVector selectedPrims = _selection->GetSelectedPrimPaths(mode);
  const size_t numPrims             = _selection ? selectedPrims.size() : 0;
  if (numPrims == 0) {
    TF_DEBUG(HDX_SELECTION_SETUP).Msg("No selected prims for mode %d\n", mode);
    return false;
  }

  std::vector<int> ids;
  ids.resize(numPrims);

  size_t const N = 1000;
  WorkParallelForN(numPrims / N + 1,
                   [&ids, index, N, &selectedPrims](size_t begin, size_t end) mutable {
                     end   = std::min(end * N, ids.size());
                     begin = begin * N;
                     for (size_t i = begin; i < end; i++) {
                       if (auto const &rprim = index->GetRprim(selectedPrims[i])) {
                         ids[i] = rprim->GetPrimId();
                       }
                       else {
                         // silently ignore non-existing prim
                         ids[i] = INVALID;
                       }
                     }
                   });

  // Note that numeric_limits<float>::min for is surprising, so using lowest()
  // here instead. Doing this for <int> here to avoid copy and paste bugs.
  int min = std::numeric_limits<int>::max(), max = std::numeric_limits<int>::lowest();

  for (int id : ids) {
    if (id == INVALID)
      continue;
    min = std::min(id, min);
    max = std::max(id, max);
  }

  if (max < min) {
    return false;
  }

  // ---------------------------------------------------------------------- //
  // Buffer Layout
  // ---------------------------------------------------------------------- //
  // In the following code, we want to build up a buffer that is capable of
  // driving selection highlighting. To do this, we leverage the fact that the
  // fragment shader has access to the drawing coord, namely the PrimID,
  // InstanceID, ElementID, EdgeID, VertexID, etc.
  // The idea is to take one such ID and compare it against a [min, max) range
  // of selected IDs. Since it is range based, it is possible that only a
  // subset of values in the range are selected. Following the range is a set
  // of "selection offset" values that encode whether the ID in question is
  // selected, and the offset to next ID in the hierarchy.
  //
  // For example, imagine the PrimID is 6, then we can know this prim *may* be
  // selected only if ID 6 is in the range of selected prims.
  //
  // The buffer layout is as follows:
  //
  // Prim: [ start index | end index | (selection offsets per prim) ]
  //
  // For subprims of a prim (specific instances, specific faces, etc),
  // we add a 'type' field before the range.
  //
  // Subprim: [ type | start index | end index | (selection offsets) ]
  //
  // To test if a given fragment is selected, we do the following:
  // (a) check if the PrimID is in the range [start,end), if so, the prim's
  //     offset in the buffer is ID-start.
  // (b) the value at that offset encodes the "selection offset" with bit 0
  //     indicating if the prim is fully selected, and bits 31:1 holding the
  //     offset to the next level in the hierarchy (instances/subprims).
  // (c) jump to the offset, and hierarchically apply (b), gathering the
  //     selection state for each level.
  //
  // All data is aggregated into a single  buffer with the following layout:
  //
  // [ prims | points | edges | elements | instances ]
  //          <-------- subprims ------->
  //          <------------- per prim --------------->
  //
  //  Each section above is prefixed with [start,end) ranges and the values of
  //  each range follow the three cases outlined.
  //
  //  To see these values built incrementally, enable the TF_DEBUG flag
  //  HDX_SELECTION_SETUP.
  // ---------------------------------------------------------------------- //

  // Start with individual arrays. Splice arrays once finished.
  int const PRIM_SELOFFSETS_HEADER_SIZE = 2;
  bool const SELECT_NONE                = 0;

  enum SubPrimType { ELEMENT = 0, EDGE = 1, POINT = 2, INSTANCE = 3 };

  _DebugPrintArray("ids", ids);
  // For initialization, use offset=0 in the seloffset encoding.
  // This will be updated as need be once we process subprims and instances.
  output->insert(output->end(),
                 PRIM_SELOFFSETS_HEADER_SIZE + (max - min + 1),
                 _EncodeSelOffset(/*offset=*/0, SELECT_NONE));
  (*output)[0] = min;
  (*output)[1] = max + 1;

  _DebugPrintArray("prims", *output);

  for (size_t primIndex = 0; primIndex < ids.size(); primIndex++) {
    // TODO: store ID and path in "ids" vector
    int id = ids[primIndex];
    if (id == INVALID)
      continue;

    SdfPath const &objPath = selectedPrims[primIndex];
    TF_DEBUG(HDX_SELECTION_SETUP).Msg("Processing: %d - %s\n", id, objPath.GetText());

    HdSelection::PrimSelectionState const *primSelState = _selection->GetPrimSelectionState(
        mode, objPath);
    if (!primSelState)
      continue;

    // netSubprimOffset tracks the "net" offset to the start of each
    // subprim's range-offsets encoding; it allows us to handle selection of
    // multiple subprims per prim (XXX: not per instance of a prim) by
    // backpointing from elements to edges to points.
    // We process subprims in the reverse order to allow for this.

    size_t netSubprimOffset = 0;
    //------------------------------------------------------------------- //
    // Subprimitives: Points
    // ------------------------------------------------------------------ //
    size_t curOffset = output->size();
    if (_FillPointSelOffsets(
            POINT, primSelState->pointIndices, primSelState->pointColorIndices, output)) {
      netSubprimOffset = curOffset + modeOffset;
      _DebugPrintArray("points", *output);
    }

    //------------------------------------------------------------------- //
    // Subprimitives: Edges
    // ------------------------------------------------------------------ //
    curOffset = output->size();
    if (_FillSubprimSelOffsets(EDGE, primSelState->edgeIndices, netSubprimOffset, output)) {
      netSubprimOffset = curOffset + modeOffset;
      _DebugPrintArray("edges", *output);
    }

    // ------------------------------------------------------------------ //
    // Subprimitives: Elements (coarse/authored face(s) for meshes,
    //                          individual curve(s) for basis curves)
    // ------------------------------------------------------------------ //
    curOffset = output->size();
    if (_FillSubprimSelOffsets(ELEMENT, primSelState->elementIndices, netSubprimOffset, output)) {
      netSubprimOffset = curOffset + modeOffset;
      _DebugPrintArray("elements", *output);
    }

    // ------------------------------------------------------------------ //
    // Instances
    // ------------------------------------------------------------------ //
    curOffset = output->size();
    if (_FillSubprimSelOffsets(
            INSTANCE, primSelState->instanceIndices, netSubprimOffset, output)) {
      netSubprimOffset = curOffset + modeOffset;
      _DebugPrintArray("instances", *output);
    }

    // Finally, put the prim selection state in.
    (*output)[id - min + 2] = _EncodeSelOffset(netSubprimOffset, primSelState->fullySelected);
  }

  _DebugPrintArray("final output", *output);

  return true;
}

WABI_NAMESPACE_END
