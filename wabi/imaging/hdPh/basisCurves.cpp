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
#include "wabi/wabi.h"

#include "wabi/imaging/hdPh/basisCurves.h"
#include "wabi/imaging/hdPh/basisCurvesComputations.h"
#include "wabi/imaging/hdPh/basisCurvesShaderKey.h"
#include "wabi/imaging/hdPh/basisCurvesTopology.h"
#include "wabi/imaging/hdPh/bufferArrayRange.h"
#include "wabi/imaging/hdPh/drawItem.h"
#include "wabi/imaging/hdPh/extCompGpuComputation.h"
#include "wabi/imaging/hdPh/geometricShader.h"
#include "wabi/imaging/hdPh/instancer.h"
#include "wabi/imaging/hdPh/material.h"
#include "wabi/imaging/hdPh/primUtils.h"
#include "wabi/imaging/hdPh/resourceRegistry.h"
#include "wabi/imaging/hdPh/tokens.h"

#include "wabi/base/arch/hash.h"

#include "wabi/base/gf/matrix4d.h"
#include "wabi/base/gf/matrix4f.h"
#include "wabi/base/gf/vec2d.h"
#include "wabi/base/gf/vec2i.h"

#include "wabi/base/vt/value.h"
#include "wabi/imaging/hd/bufferSource.h"
#include "wabi/imaging/hd/computation.h"
#include "wabi/imaging/hd/repr.h"
#include "wabi/imaging/hd/vertexAdjacency.h"

WABI_NAMESPACE_BEGIN

HdPhBasisCurves::HdPhBasisCurves(SdfPath const &id)
    : HdBasisCurves(id),
      _topology(),
      _topologyId(0),
      _customDirtyBitsInUse(0),
      _refineLevel(0),
      _displayOpacity(false),
      _occludedSelectionShowsThrough(false)
{
  /*NOTHING*/
}

HdPhBasisCurves::~HdPhBasisCurves() = default;

void HdPhBasisCurves::Sync(HdSceneDelegate *delegate,
                           HdRenderParam *renderParam,
                           HdDirtyBits *dirtyBits,
                           TfToken const &reprToken)
{
  bool updateMaterialTag = false;
  if (*dirtyBits & HdChangeTracker::DirtyMaterialId) {
    HdPhSetMaterialId(delegate, renderParam, this);
    updateMaterialTag = true;
  }
  if (*dirtyBits & HdChangeTracker::DirtyDisplayStyle) {
    updateMaterialTag = true;
  }

  // Check if either the material or geometric shaders need updating for
  // draw items of all the reprs.
  bool updateMaterialShader = false;
  if (*dirtyBits & (HdChangeTracker::DirtyMaterialId | HdChangeTracker::NewRepr)) {
    updateMaterialShader = true;
  }

  bool updateGeometricShader = false;
  if (*dirtyBits & (HdChangeTracker::DirtyDisplayStyle | HdChangeTracker::DirtyMaterialId |
                    HdChangeTracker::DirtyTopology |  // topological visibility
                    HdChangeTracker::NewRepr)) {
    updateGeometricShader = true;
  }

  bool displayOpacity = _displayOpacity;
  _UpdateRepr(delegate, renderParam, reprToken, dirtyBits);

  if (updateMaterialTag || (GetMaterialId().IsEmpty() && displayOpacity != _displayOpacity)) {

    HdPhSetMaterialTag(
        delegate, renderParam, this, _displayOpacity, _occludedSelectionShowsThrough);
  }

  if (updateMaterialShader || updateGeometricShader) {
    _UpdateShadersForAllReprs(delegate, renderParam, updateMaterialShader, updateGeometricShader);
  }

  // This clears all the non-custom dirty bits. This ensures that the rprim
  // doesn't have pending dirty bits that add it to the dirty list every
  // frame.
  // XXX: GetInitialDirtyBitsMask sets certain dirty bits that aren't
  // reset (e.g. DirtyExtent, DirtyPrimID) that make this necessary.
  *dirtyBits &= ~HdChangeTracker::AllSceneDirtyBits;
}

void HdPhBasisCurves::Finalize(HdRenderParam *renderParam)
{
  HdPhMarkGarbageCollectionNeeded(renderParam);
}

void HdPhBasisCurves::_UpdateDrawItem(HdSceneDelegate *sceneDelegate,
                                      HdRenderParam *renderParam,
                                      HdPhDrawItem *drawItem,
                                      HdDirtyBits *dirtyBits,
                                      const HdBasisCurvesReprDesc &desc)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  SdfPath const &id = GetId();

  /* VISIBILITY */
  _UpdateVisibility(sceneDelegate, dirtyBits);

  /* MATERIAL SHADER (may affect subsequent primvar population) */
  if ((*dirtyBits & HdChangeTracker::NewRepr) ||
      HdChangeTracker::IsAnyPrimvarDirty(*dirtyBits, id)) {
    drawItem->SetMaterialShader(HdPhGetMaterialShader(this, sceneDelegate));
  }

  // Reset value of _displayOpacity
  if (HdChangeTracker::IsAnyPrimvarDirty(*dirtyBits, id)) {
    _displayOpacity = false;
  }

  /* INSTANCE PRIMVARS */
  _UpdateInstancer(sceneDelegate, dirtyBits);
  HdPhUpdateInstancerData(
      sceneDelegate->GetRenderIndex(), renderParam, this, drawItem, &_sharedData, *dirtyBits);

  _displayOpacity = _displayOpacity ||
                    HdPhIsInstancePrimvarExistentAndValid(
                        sceneDelegate->GetRenderIndex(), this, HdTokens->displayOpacity);

  /* CONSTANT PRIMVARS, TRANSFORM, EXTENT AND PRIMID */
  if (HdPhShouldPopulateConstantPrimvars(dirtyBits, id)) {
    HdPrimvarDescriptorVector constantPrimvars = HdPhGetPrimvarDescriptors(
        this, drawItem, sceneDelegate, HdInterpolationConstant);

    HdPhPopulateConstantPrimvars(
        this, &_sharedData, sceneDelegate, renderParam, drawItem, dirtyBits, constantPrimvars);

    _displayOpacity = _displayOpacity ||
                      HdPhIsPrimvarExistentAndValid(
                          this, sceneDelegate, constantPrimvars, HdTokens->displayOpacity);
  }

  /* TOPOLOGY */
  // XXX: _PopulateTopology should be split into two phase
  //      for scene dirtybits and for repr dirtybits.
  if (*dirtyBits & (HdChangeTracker::DirtyTopology | HdChangeTracker::DirtyDisplayStyle |
                    DirtyIndices | DirtyHullIndices | DirtyPointsIndices)) {
    _PopulateTopology(sceneDelegate, renderParam, drawItem, dirtyBits, desc);
  }

  /* PRIMVAR */
  if (HdChangeTracker::IsAnyPrimvarDirty(*dirtyBits, id)) {
    // XXX: curves don't use refined vertex primvars, however,
    // the refined renderpass masks the dirtiness of non-refined vertex
    // primvars, so we need to see refined dirty for updating coarse
    // vertex primvars if there is only refined reprs being updated.
    // we'll fix the change tracking in order to address this craziness.
    _PopulateVertexPrimvars(sceneDelegate, renderParam, drawItem, dirtyBits);
    _PopulateVaryingPrimvars(sceneDelegate, renderParam, drawItem, dirtyBits);
    _PopulateElementPrimvars(sceneDelegate, renderParam, drawItem, dirtyBits);
  }

  // When we have multiple drawitems for the same prim we need to clean the
  // bits for all the data fields touched in this function, otherwise it
  // will try to extract topology (for instance) twice, and this won't
  // work with delegates that don't keep information around once extracted.
  *dirtyBits &= ~HdChangeTracker::AllSceneDirtyBits;

  // Topology and VertexPrimvar may be null, if the curve has zero line
  // segments.
  TF_VERIFY(drawItem->GetConstantPrimvarRange());
}

static const char *HdPh_PrimTypeToString(HdPh_GeometricShader::PrimitiveType type)
{
  switch (type) {
    case HdPh_GeometricShader::PrimitiveType::PRIM_POINTS:
      return "points";
    case HdPh_GeometricShader::PrimitiveType::PRIM_BASIS_CURVES_LINES:
      return "lines";
    case HdPh_GeometricShader::PrimitiveType::PRIM_BASIS_CURVES_LINEAR_PATCHES:
      return "patches[linear]";
    case HdPh_GeometricShader::PrimitiveType::PRIM_BASIS_CURVES_CUBIC_PATCHES:
      return "patches[cubic]";
    default:
      TF_WARN("Unknown type");
      return "unknown";
  }
}

void HdPhBasisCurves::_UpdateDrawItemGeometricShader(HdSceneDelegate *sceneDelegate,
                                                     HdRenderParam *renderParam,
                                                     HdPhDrawItem *drawItem,
                                                     const HdBasisCurvesReprDesc &desc)
{
  if (!TF_VERIFY(_topology))
    return;

  HdRenderIndex &renderIndex = sceneDelegate->GetRenderIndex();

  TfToken curveType       = _topology->GetCurveType();
  TfToken curveBasis      = _topology->GetCurveBasis();
  bool supportsRefinement = _SupportsRefinement(_refineLevel);
  if (!supportsRefinement) {
    // XXX: Rendering non-linear (i.e., cubic) curves as linear segments
    // when unrefined can be confusing. Should we continue to do this?
    TF_DEBUG(HD_RPRIM_UPDATED)
        .Msg(
            "HdPhBasisCurves(%s) - Downcasting curve type to linear because"
            " refinement is disabled.\n",
            GetId().GetText());
    curveType  = HdTokens->linear;
    curveBasis = TfToken();
  }

  HdPh_BasisCurvesShaderKey::DrawStyle drawStyle     = HdPh_BasisCurvesShaderKey::WIRE;
  HdPh_BasisCurvesShaderKey::NormalStyle normalStyle = HdPh_BasisCurvesShaderKey::HAIR;
  switch (desc.geomStyle) {
    case HdBasisCurvesGeomStylePoints: {
      drawStyle   = HdPh_BasisCurvesShaderKey::POINTS;
      normalStyle = HdPh_BasisCurvesShaderKey::HAIR;
      break;
    }
    case HdBasisCurvesGeomStyleWire: {
      drawStyle   = HdPh_BasisCurvesShaderKey::WIRE;
      normalStyle = HdPh_BasisCurvesShaderKey::HAIR;
      break;
    }
    case HdBasisCurvesGeomStylePatch: {
      if (_SupportsRefinement(_refineLevel) && _SupportsUserWidths(drawItem)) {
        if (_SupportsUserNormals(drawItem)) {
          drawStyle   = HdPh_BasisCurvesShaderKey::RIBBON;
          normalStyle = HdPh_BasisCurvesShaderKey::ORIENTED;
        }
        else {
          if (_refineLevel > 2) {
            normalStyle = HdPh_BasisCurvesShaderKey::ROUND;
            drawStyle   = HdPh_BasisCurvesShaderKey::HALFTUBE;
          }
          else if (_refineLevel > 1) {
            normalStyle = HdPh_BasisCurvesShaderKey::ROUND;
            drawStyle   = HdPh_BasisCurvesShaderKey::RIBBON;
          }
          else {
            drawStyle   = HdPh_BasisCurvesShaderKey::RIBBON;
            normalStyle = HdPh_BasisCurvesShaderKey::HAIR;
          }
        }
      }
      break;
    }
    default: {
      TF_CODING_ERROR("Invalid geomstyle in basis curve %s repr desc.", GetId().GetText());
    }
  }

  TF_DEBUG(HD_RPRIM_UPDATED)
      .Msg("HdPhBasisCurves(%s) - Building shader with keys: %s, %s, %s, %s, %s, %s\n",
           GetId().GetText(),
           curveType.GetText(),
           curveBasis.GetText(),
           TfEnum::GetName(drawStyle).c_str(),
           TfEnum::GetName(normalStyle).c_str(),
           _basisWidthInterpolation ? "basis widths" : "linear widths",
           _basisNormalInterpolation ? "basis normals" : "linear normals");

  bool hasAuthoredTopologicalVisiblity = (bool)drawItem->GetTopologyVisibilityRange();

  HdPh_BasisCurvesShaderKey shaderKey(curveType,
                                      curveBasis,
                                      drawStyle,
                                      normalStyle,
                                      _basisWidthInterpolation,
                                      _basisNormalInterpolation,
                                      desc.shadingTerminal,
                                      hasAuthoredTopologicalVisiblity);

  TF_DEBUG(HD_RPRIM_UPDATED)
      .Msg("HdPhBasisCurves(%s) - Shader Key PrimType: %s\n ",
           GetId().GetText(),
           HdPh_PrimTypeToString(shaderKey.primType));

  HdPhResourceRegistrySharedPtr resourceRegistry = std::static_pointer_cast<HdPhResourceRegistry>(
      renderIndex.GetResourceRegistry());

  HdPh_GeometricShaderSharedPtr geomShader = HdPh_GeometricShader::Create(shaderKey,
                                                                          resourceRegistry);

  TF_VERIFY(geomShader);

  if (geomShader != drawItem->GetGeometricShader()) {
    drawItem->SetGeometricShader(geomShader);

    // If the gometric shader changes, we need to do a deep validation of
    // batches, so they can be rebuilt if necessary.
    HdPhMarkDrawBatchesDirty(renderParam);

    TF_DEBUG(HD_RPRIM_UPDATED)
        .Msg(
            "%s: Marking all batches dirty to trigger deep validation because"
            " the geometric shader was updated.\n",
            GetId().GetText());
  }
}

HdDirtyBits HdPhBasisCurves::_PropagateDirtyBits(HdDirtyBits bits) const
{
  // propagate scene-based dirtyBits into rprim-custom dirtyBits
  if (bits & HdChangeTracker::DirtyTopology) {
    bits |= _customDirtyBitsInUse & (DirtyIndices | DirtyHullIndices | DirtyPointsIndices);
  }

  return bits;
}

void HdPhBasisCurves::_InitRepr(TfToken const &reprToken, HdDirtyBits *dirtyBits)
{
  _ReprVector::iterator it = std::find_if(
      _reprs.begin(), _reprs.end(), _ReprComparator(reprToken));
  bool isNew = it == _reprs.end();
  if (isNew) {
    _BasisCurvesReprConfig::DescArray descs = _GetReprDesc(reprToken);

    // add new repr
    _reprs.emplace_back(reprToken, std::make_shared<HdRepr>());
    HdReprSharedPtr &repr = _reprs.back().second;

    *dirtyBits |= HdChangeTracker::NewRepr;

    // allocate all draw items
    for (size_t descIdx = 0; descIdx < descs.size(); ++descIdx) {
      const HdBasisCurvesReprDesc &desc = descs[descIdx];

      if (desc.geomStyle == HdBasisCurvesGeomStyleInvalid) {
        continue;
      }

      HdRepr::DrawItemUniquePtr drawItem = std::make_unique<HdPhDrawItem>(&_sharedData);
      HdDrawingCoord *drawingCoord       = drawItem->GetDrawingCoord();
      repr->AddDrawItem(std::move(drawItem));
      if (desc.geomStyle == HdBasisCurvesGeomStyleWire) {
        // Why does geom style require this change?
        drawingCoord->SetTopologyIndex(HdPhBasisCurves::HullTopology);
        if (!(_customDirtyBitsInUse & DirtyHullIndices)) {
          _customDirtyBitsInUse |= DirtyHullIndices;
          *dirtyBits |= DirtyHullIndices;
        }
      }
      else if (desc.geomStyle == HdBasisCurvesGeomStylePoints) {
        drawingCoord->SetTopologyIndex(HdPhBasisCurves::PointsTopology);
        if (!(_customDirtyBitsInUse & DirtyPointsIndices)) {
          _customDirtyBitsInUse |= DirtyPointsIndices;
          *dirtyBits |= DirtyPointsIndices;
        }
      }
      else {
        if (!(_customDirtyBitsInUse & DirtyIndices)) {
          _customDirtyBitsInUse |= DirtyIndices;
          *dirtyBits |= DirtyIndices;
        }
      }

      // Set up drawing coord instance primvars.
      drawingCoord->SetInstancePrimvarBaseIndex(HdPhBasisCurves::InstancePrimvar);
    }
  }
}

void HdPhBasisCurves::_UpdateRepr(HdSceneDelegate *sceneDelegate,
                                  HdRenderParam *renderParam,
                                  TfToken const &reprToken,
                                  HdDirtyBits *dirtyBits)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  HdReprSharedPtr const &curRepr = _GetRepr(reprToken);
  if (!curRepr) {
    return;
  }

  // Filter custom dirty bits to only those in use.
  *dirtyBits &= (_customDirtyBitsInUse | HdChangeTracker::AllSceneDirtyBits |
                 HdChangeTracker::NewRepr);

  if (TfDebug::IsEnabled(HD_RPRIM_UPDATED)) {
    TfDebug::Helper().Msg("HdPhBasisCurves::_UpdateRepr for %s : Repr = %s\n",
                          GetId().GetText(),
                          reprToken.GetText());
    HdChangeTracker::DumpDirtyBits(*dirtyBits);
  }

  _BasisCurvesReprConfig::DescArray const &reprDescs = _GetReprDesc(reprToken);

  int drawItemIndex = 0;
  for (size_t descIdx = 0; descIdx < reprDescs.size(); ++descIdx) {
    // curves don't have multiple draw items (for now)
    const HdBasisCurvesReprDesc &desc = reprDescs[descIdx];

    if (desc.geomStyle != HdBasisCurvesGeomStyleInvalid) {
      HdPhDrawItem *drawItem = static_cast<HdPhDrawItem *>(curRepr->GetDrawItem(drawItemIndex++));

      if (HdChangeTracker::IsDirty(*dirtyBits)) {
        _UpdateDrawItem(sceneDelegate, renderParam, drawItem, dirtyBits, desc);
      }
    }
  }

  *dirtyBits &= ~HdChangeTracker::NewRepr;
}

void HdPhBasisCurves::_UpdateShadersForAllReprs(HdSceneDelegate *sceneDelegate,
                                                HdRenderParam *renderParam,
                                                bool updateMaterialShader,
                                                bool updateGeometricShader)
{
  TF_DEBUG(HD_RPRIM_UPDATED)
      .Msg(
          "(%s) - Updating geometric and material shaders for draw "
          "items of all reprs.\n",
          GetId().GetText());

  HdPhShaderCodeSharedPtr materialShader;
  if (updateMaterialShader) {
    materialShader = HdPhGetMaterialShader(this, sceneDelegate);
  }

  for (auto const &reprPair : _reprs) {
    const TfToken &reprToken                       = reprPair.first;
    _BasisCurvesReprConfig::DescArray const &descs = _GetReprDesc(reprToken);
    HdReprSharedPtr repr                           = reprPair.second;
    int drawItemIndex                              = 0;
    for (size_t descIdx = 0; descIdx < descs.size(); ++descIdx) {
      if (descs[descIdx].geomStyle == HdBasisCurvesGeomStyleInvalid) {
        continue;
      }
      HdPhDrawItem *drawItem = static_cast<HdPhDrawItem *>(repr->GetDrawItem(drawItemIndex++));

      if (updateMaterialShader) {
        drawItem->SetMaterialShader(materialShader);
      }
      if (updateGeometricShader) {
        _UpdateDrawItemGeometricShader(sceneDelegate, renderParam, drawItem, descs[descIdx]);
      }
    }
  }
}

void HdPhBasisCurves::_PopulateTopology(HdSceneDelegate *sceneDelegate,
                                        HdRenderParam *renderParam,
                                        HdPhDrawItem *drawItem,
                                        HdDirtyBits *dirtyBits,
                                        const HdBasisCurvesReprDesc &desc)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  SdfPath const &id = GetId();
  HdPhResourceRegistrySharedPtr const &resourceRegistry =
      std::static_pointer_cast<HdPhResourceRegistry>(
          sceneDelegate->GetRenderIndex().GetResourceRegistry());
  HdChangeTracker &changeTracker = sceneDelegate->GetRenderIndex().GetChangeTracker();

  if (*dirtyBits & HdChangeTracker::DirtyDisplayStyle) {
    HdDisplayStyle ds              = GetDisplayStyle(sceneDelegate);
    _refineLevel                   = ds.refineLevel;
    _occludedSelectionShowsThrough = ds.occludedSelectionShowsThrough;
  }

  // XXX: is it safe to get topology even if it's not dirty?
  bool dirtyTopology = HdChangeTracker::IsTopologyDirty(*dirtyBits, id);

  if (dirtyTopology || HdChangeTracker::IsDisplayStyleDirty(*dirtyBits, id)) {

    const HdBasisCurvesTopology &srcTopology = GetBasisCurvesTopology(sceneDelegate);

    // Topological visibility (of points, curves) comes in as DirtyTopology.
    // We encode this information in a separate BAR.
    if (dirtyTopology) {
      HdPhProcessTopologyVisibility(srcTopology.GetInvisibleCurves(),
                                    srcTopology.GetNumCurves(),
                                    srcTopology.GetInvisiblePoints(),
                                    srcTopology.CalculateNeededNumberOfControlPoints(),
                                    &_sharedData,
                                    drawItem,
                                    renderParam,
                                    &changeTracker,
                                    resourceRegistry,
                                    id);
    }

    // compute id.
    _topologyId  = srcTopology.ComputeHash();
    bool refined = (_refineLevel > 0);
    _topologyId  = ArchHash64((const char *)&refined, sizeof(refined), _topologyId);

    // ask the registry if there is a sharable basisCurves topology
    HdInstance<HdPh_BasisCurvesTopologySharedPtr> topologyInstance =
        resourceRegistry->RegisterBasisCurvesTopology(_topologyId);

    if (topologyInstance.IsFirstInstance()) {
      // if this is the first instance, create a new stream topology
      // representation and use that.
      HdPh_BasisCurvesTopologySharedPtr topology = HdPh_BasisCurvesTopology::New(srcTopology);

      topologyInstance.SetValue(topology);
    }

    _topology = topologyInstance.GetValue();
    TF_VERIFY(_topology);

    // hash collision check
    if (TfDebug::IsEnabled(HD_SAFE_MODE)) {
      TF_VERIFY(srcTopology == *_topology);
    }
  }

  // bail out if the index bar is already synced
  TfToken indexToken;
  if (drawItem->GetDrawingCoord()->GetTopologyIndex() == HdPhBasisCurves::HullTopology) {
    if ((*dirtyBits & DirtyHullIndices) == 0)
      return;
    *dirtyBits &= ~DirtyHullIndices;
    indexToken = HdTokens->hullIndices;
  }
  else if (drawItem->GetDrawingCoord()->GetTopologyIndex() == HdPhBasisCurves::PointsTopology) {
    if ((*dirtyBits & DirtyPointsIndices) == 0)
      return;
    *dirtyBits &= ~DirtyPointsIndices;
    indexToken = HdTokens->pointsIndices;
  }
  else {
    if ((*dirtyBits & DirtyIndices) == 0)
      return;
    *dirtyBits &= ~DirtyIndices;
    indexToken = HdTokens->indices;
  }

  {
    HdInstance<HdBufferArrayRangeSharedPtr> rangeInstance =
        resourceRegistry->RegisterBasisCurvesIndexRange(_topologyId, indexToken);

    if (rangeInstance.IsFirstInstance()) {
      HdBufferSourceSharedPtrVector sources;
      HdBufferSpecVector bufferSpecs;

      if (desc.geomStyle == HdBasisCurvesGeomStylePoints) {
        sources.push_back(_topology->GetPointsIndexBuilderComputation());
      }
      else {
        sources.push_back(
            _topology->GetIndexBuilderComputation(!_SupportsRefinement(_refineLevel)));
      }

      HdBufferSpec::GetBufferSpecs(sources, &bufferSpecs);

      // Set up the usage hints to mark topology as varying if
      // there is a previously set range.
      HdBufferArrayUsageHint usageHint;
      usageHint.bits.sizeVarying = drawItem->GetTopologyRange() ? 1 : 0;

      // allocate new range
      HdBufferArrayRangeSharedPtr range = resourceRegistry->AllocateNonUniformBufferArrayRange(
          HdTokens->topology, bufferSpecs, usageHint);

      // add sources to update queue
      resourceRegistry->AddSources(range, std::move(sources));
      rangeInstance.SetValue(range);
    }

    HdBufferArrayRangeSharedPtr const &newRange = rangeInstance.GetValue();

    HdPhUpdateDrawItemBAR(newRange,
                          drawItem->GetDrawingCoord()->GetTopologyIndex(),
                          &_sharedData,
                          renderParam,
                          &changeTracker);
  }
}

namespace {

template<typename T>
void AddVertexOrVaryingPrimvarSource(const TfToken &name,
                                     HdInterpolation interpolation,
                                     const VtValue &value,
                                     HdPh_BasisCurvesTopologySharedPtr topology,
                                     HdBufferSourceSharedPtrVector *sources,
                                     T fallbackValue)
{
  VtArray<T> array = value.Get<VtArray<T>>();
  // Empty primvar arrays are ignored, except for points
  if (!array.empty() || name == HdTokens->points) {
    sources->push_back(HdBufferSourceSharedPtr(
        std::make_shared<HdPh_BasisCurvesPrimvarInterpolaterComputation<T>>(
            topology,
            array,
            name,
            interpolation,
            fallbackValue,
            HdGetValueTupleType(VtValue(array)).type)));
  }
}

void ProcessVertexOrVaryingPrimvar(const TfToken &name,
                                   HdInterpolation interpolation,
                                   const VtValue &value,
                                   HdPh_BasisCurvesTopologySharedPtr topology,
                                   HdBufferSourceSharedPtrVector *sources)
{
  if (value.IsHolding<VtHalfArray>()) {
    AddVertexOrVaryingPrimvarSource<GfHalf>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtFloatArray>()) {
    AddVertexOrVaryingPrimvarSource<float>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtVec2fArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec2f>(
        name, interpolation, value, topology, sources, GfVec2f(1, 0));
  }
  else if (value.IsHolding<VtVec3fArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec3f>(
        name, interpolation, value, topology, sources, GfVec3f(1, 0, 0));
  }
  else if (value.IsHolding<VtVec4fArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec4f>(
        name, interpolation, value, topology, sources, GfVec4f(1, 0, 0, 1));
  }
  else if (value.IsHolding<VtDoubleArray>()) {
    AddVertexOrVaryingPrimvarSource<double>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtVec2dArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec2d>(
        name, interpolation, value, topology, sources, GfVec2d(1, 0));
  }
  else if (value.IsHolding<VtVec3dArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec3d>(
        name, interpolation, value, topology, sources, GfVec3d(1, 0, 0));
  }
  else if (value.IsHolding<VtVec4dArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec4d>(
        name, interpolation, value, topology, sources, GfVec4d(1, 0, 0, 1));
  }
  else if (value.IsHolding<VtIntArray>()) {
    AddVertexOrVaryingPrimvarSource<int>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtVec2iArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec2i>(
        name, interpolation, value, topology, sources, GfVec2i(1, 0));
  }
  else if (value.IsHolding<VtVec3iArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec3i>(
        name, interpolation, value, topology, sources, GfVec3i(1, 0, 0));
  }
  else if (value.IsHolding<VtVec4iArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec4i>(
        name, interpolation, value, topology, sources, GfVec4i(1, 0, 0, 1));
  }
  else if (value.IsHolding<VtVec4iArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec4i>(
        name, interpolation, value, topology, sources, GfVec4i(1, 0, 0, 1));
  }
  else if (value.IsHolding<VtVec4iArray>()) {
    AddVertexOrVaryingPrimvarSource<GfVec4i>(
        name, interpolation, value, topology, sources, GfVec4i(1, 0, 0, 1));
  }
  else if (value.IsHolding<VtArray<int16_t>>()) {
    AddVertexOrVaryingPrimvarSource<int16_t>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtArray<int32_t>>()) {
    AddVertexOrVaryingPrimvarSource<int32_t>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtArray<uint16_t>>()) {
    AddVertexOrVaryingPrimvarSource<uint16_t>(name, interpolation, value, topology, sources, 1);
  }
  else if (value.IsHolding<VtArray<uint32_t>>()) {
    AddVertexOrVaryingPrimvarSource<uint32_t>(name, interpolation, value, topology, sources, 1);
  }
  else {
    TF_WARN("Type of vertex or varying primvar %s not yet fully supported", name.GetText());
    sources->push_back(HdBufferSourceSharedPtr(std::make_shared<HdVtBufferSource>(name, value)));
  }
}
}  // anonymous namespace

void HdPhBasisCurves::_PopulateVertexPrimvars(HdSceneDelegate *sceneDelegate,
                                              HdRenderParam *renderParam,
                                              HdPhDrawItem *drawItem,
                                              HdDirtyBits *dirtyBits)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  SdfPath const &id = GetId();
  HdPhResourceRegistrySharedPtr const &resourceRegistry =
      std::static_pointer_cast<HdPhResourceRegistry>(
          sceneDelegate->GetRenderIndex().GetResourceRegistry());

  // The "points" attribute is expected to be in this list.
  HdPrimvarDescriptorVector primvars = HdPhGetPrimvarDescriptors(
      this, drawItem, sceneDelegate, HdInterpolationVertex);

  HdExtComputationPrimvarDescriptorVector compPrimvars =
      sceneDelegate->GetExtComputationPrimvarDescriptors(id, HdInterpolationVertex);

  HdBufferSourceSharedPtrVector sources;
  HdBufferSourceSharedPtrVector reserveOnlySources;
  HdBufferSourceSharedPtrVector separateComputationSources;
  HdPhComputationSharedPtrVector computations;
  sources.reserve(primvars.size());

  HdPh_GetExtComputationPrimvarsComputations(id,
                                             sceneDelegate,
                                             compPrimvars,
                                             *dirtyBits,
                                             &sources,
                                             &reserveOnlySources,
                                             &separateComputationSources,
                                             &computations);

  for (HdPrimvarDescriptor const &primvar : primvars) {
    if (!HdChangeTracker::IsPrimvarDirty(*dirtyBits, id, primvar.name))
      continue;

    // TODO: We don't need to pull primvar metadata every time a value
    // changes, but we need support from the delegate.

    // Having a null topology is possible, but shouldn't happen when there
    // are points
    if (!_topology) {
      if (primvar.name == HdTokens->points) {
        TF_CODING_ERROR("No topology set for BasisCurve %s", id.GetName().c_str());
        break;
      }
      continue;
    }

    // assert name not in range.bufferArray.GetResources()
    VtValue value = GetPrimvar(sceneDelegate, primvar.name);
    if (!value.IsEmpty()) {
      ProcessVertexOrVaryingPrimvar(
          primvar.name, HdInterpolationVertex, value, _topology, &sources);

      if (primvar.name == HdTokens->displayOpacity) {
        _displayOpacity = true;
      }
    }
  }

  HdBufferArrayRangeSharedPtr const &bar = drawItem->GetVertexPrimvarRange();

  if (HdPhCanSkipBARAllocationOrUpdate(sources, computations, bar, *dirtyBits)) {
    return;
  }

  // XXX: This should be based off the DirtyPrimvarDesc bit.
  bool hasDirtyPrimvarDesc = (*dirtyBits & HdChangeTracker::DirtyPrimvar);
  HdBufferSpecVector removedSpecs;
  if (hasDirtyPrimvarDesc) {
    TfTokenVector internallyGeneratedPrimvars;  // none
    removedSpecs = HdPhGetRemovedPrimvarBufferSpecs(
        bar, primvars, compPrimvars, internallyGeneratedPrimvars, id);
  }

  HdBufferSpecVector bufferSpecs;
  HdBufferSpec::GetBufferSpecs(sources, &bufferSpecs);
  HdBufferSpec::GetBufferSpecs(reserveOnlySources, &bufferSpecs);
  HdPhGetBufferSpecsFromCompuations(computations, &bufferSpecs);

  HdBufferArrayRangeSharedPtr range = resourceRegistry->UpdateNonUniformBufferArrayRange(
      HdTokens->primvar, bar, bufferSpecs, removedSpecs, HdBufferArrayUsageHint());

  HdPhUpdateDrawItemBAR(range,
                        drawItem->GetDrawingCoord()->GetVertexPrimvarIndex(),
                        &_sharedData,
                        renderParam,
                        &(sceneDelegate->GetRenderIndex().GetChangeTracker()));

  if (!sources.empty() || !computations.empty()) {
    // If sources or computations are to be queued against the resulting
    // BAR, we expect it to be valid.
    if (!TF_VERIFY(drawItem->GetVertexPrimvarRange()->IsValid())) {
      return;
    }
  }

  // add sources to update queue
  if (!sources.empty()) {
    resourceRegistry->AddSources(drawItem->GetVertexPrimvarRange(), std::move(sources));
  }
  // add gpu computations to queue.
  for (auto const &compQueuePair : computations) {
    HdComputationSharedPtr const &comp = compQueuePair.first;
    HdPhComputeQueue queue             = compQueuePair.second;
    resourceRegistry->AddComputation(drawItem->GetVertexPrimvarRange(), comp, queue);
  }
  if (!separateComputationSources.empty()) {
    TF_FOR_ALL(it, separateComputationSources)
    {
      resourceRegistry->AddSource(*it);
    }
  }
}

void HdPhBasisCurves::_PopulateVaryingPrimvars(HdSceneDelegate *sceneDelegate,
                                               HdRenderParam *renderParam,
                                               HdPhDrawItem *drawItem,
                                               HdDirtyBits *dirtyBits)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  SdfPath const &id = GetId();
  HdPhResourceRegistrySharedPtr const &resourceRegistry =
      std::static_pointer_cast<HdPhResourceRegistry>(
          sceneDelegate->GetRenderIndex().GetResourceRegistry());

  // Gather varying primvars
  HdPrimvarDescriptorVector primvars = HdPhGetPrimvarDescriptors(
      this, drawItem, sceneDelegate, HdInterpolationVarying);

  _basisWidthInterpolation = true;
  // If we don't find varying normals, then we are assuming
  // implicit normals or prescribed basis normals. (For implicit
  // normals, varying might be the right fallback behavior, but
  // leaving as basis for now to preserve the current behavior
  // until we get can do a better pass on curve normals.)
  _basisNormalInterpolation = true;

  if (primvars.empty()) {
    return;
  }

  HdBufferSourceSharedPtrVector sources;
  sources.reserve(primvars.size());

  for (HdPrimvarDescriptor const &primvar : primvars) {
    if (primvar.name == HdTokens->widths) {
      _basisWidthInterpolation = false;
    }
    else if (primvar.name == HdTokens->normals) {
      _basisNormalInterpolation = false;
    }

    if (!HdChangeTracker::IsPrimvarDirty(*dirtyBits, id, primvar.name)) {
      continue;
    }

    // TODO: We don't need to pull primvar metadata every time a value
    // changes, but we need support from the delegate.

    // assert name not in range.bufferArray.GetResources()
    VtValue value = GetPrimvar(sceneDelegate, primvar.name);
    if (!value.IsEmpty()) {
      ProcessVertexOrVaryingPrimvar(
          primvar.name, HdInterpolationVarying, value, _topology, &sources);

      if (primvar.name == HdTokens->displayOpacity) {
        _displayOpacity = true;
      }
    }
  }

  HdBufferArrayRangeSharedPtr const &bar = drawItem->GetVaryingPrimvarRange();

  if (HdPhCanSkipBARAllocationOrUpdate(sources, bar, *dirtyBits)) {
    return;
  }

  // XXX: This should be based off the DirtyPrimvarDesc bit.
  bool hasDirtyPrimvarDesc = (*dirtyBits & HdChangeTracker::DirtyPrimvar);
  HdBufferSpecVector removedSpecs;
  if (hasDirtyPrimvarDesc) {
    TfTokenVector internallyGeneratedPrimvars;  // none
    removedSpecs = HdPhGetRemovedPrimvarBufferSpecs(
        bar, primvars, internallyGeneratedPrimvars, id);
  }

  HdBufferSpecVector bufferSpecs;
  HdBufferSpec::GetBufferSpecs(sources, &bufferSpecs);

  HdBufferArrayRangeSharedPtr range = resourceRegistry->UpdateNonUniformBufferArrayRange(
      HdTokens->primvar, bar, bufferSpecs, removedSpecs, HdBufferArrayUsageHint());

  HdPhUpdateDrawItemBAR(range,
                        drawItem->GetDrawingCoord()->GetVaryingPrimvarIndex(),
                        &_sharedData,
                        renderParam,
                        &(sceneDelegate->GetRenderIndex().GetChangeTracker()));

  // add sources to update queue
  if (!sources.empty()) {
    if (!TF_VERIFY(drawItem->GetVaryingPrimvarRange()->IsValid())) {
      return;
    }
    resourceRegistry->AddSources(drawItem->GetVaryingPrimvarRange(), std::move(sources));
  }
}

void HdPhBasisCurves::_PopulateElementPrimvars(HdSceneDelegate *sceneDelegate,
                                               HdRenderParam *renderParam,
                                               HdPhDrawItem *drawItem,
                                               HdDirtyBits *dirtyBits)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  SdfPath const &id          = GetId();
  HdRenderIndex &renderIndex = sceneDelegate->GetRenderIndex();
  HdPhResourceRegistrySharedPtr const &resourceRegistry =
      std::static_pointer_cast<HdPhResourceRegistry>(renderIndex.GetResourceRegistry());

  HdPrimvarDescriptorVector uniformPrimvars = HdPhGetPrimvarDescriptors(
      this, drawItem, sceneDelegate, HdInterpolationUniform);

  HdBufferSourceSharedPtrVector sources;
  sources.reserve(uniformPrimvars.size());

  for (HdPrimvarDescriptor const &primvar : uniformPrimvars) {
    if (!HdChangeTracker::IsPrimvarDirty(*dirtyBits, id, primvar.name))
      continue;

    VtValue value = GetPrimvar(sceneDelegate, primvar.name);
    if (!value.IsEmpty()) {
      sources.push_back(HdBufferSourceSharedPtr(new HdVtBufferSource(primvar.name, value)));

      if (primvar.name == HdTokens->displayOpacity) {
        _displayOpacity = true;
      }
    }
  }

  HdBufferArrayRangeSharedPtr const &bar = drawItem->GetElementPrimvarRange();

  if (HdPhCanSkipBARAllocationOrUpdate(sources, bar, *dirtyBits)) {
    return;
  }

  // XXX: This should be based off the DirtyPrimvarDesc bit.
  bool hasDirtyPrimvarDesc = (*dirtyBits & HdChangeTracker::DirtyPrimvar);
  HdBufferSpecVector removedSpecs;
  if (hasDirtyPrimvarDesc) {
    TfTokenVector internallyGeneratedPrimvars;  // none
    removedSpecs = HdPhGetRemovedPrimvarBufferSpecs(
        bar, uniformPrimvars, internallyGeneratedPrimvars, id);
  }

  HdBufferSpecVector bufferSpecs;
  HdBufferSpec::GetBufferSpecs(sources, &bufferSpecs);

  HdBufferArrayRangeSharedPtr range = resourceRegistry->UpdateNonUniformBufferArrayRange(
      HdTokens->primvar, bar, bufferSpecs, removedSpecs, HdBufferArrayUsageHint());

  HdPhUpdateDrawItemBAR(range,
                        drawItem->GetDrawingCoord()->GetElementPrimvarIndex(),
                        &_sharedData,
                        renderParam,
                        &(sceneDelegate->GetRenderIndex().GetChangeTracker()));

  if (!sources.empty()) {
    // If sources are to be queued against the resulting BAR, we expect it
    // to be valid.
    if (!TF_VERIFY(drawItem->GetElementPrimvarRange()->IsValid())) {
      return;
    }
    resourceRegistry->AddSources(drawItem->GetElementPrimvarRange(), std::move(sources));
  }
}

static bool HdPh_HasResource(HdPhDrawItem *drawItem, const TfToken &resourceToken)
{
  // Check for authored resource, we could leverage dirtyBits here as an
  // optimization, however the BAR is the ground truth, so until there is a
  // known peformance issue, we just check them explicitly.
  bool hasAuthoredResouce = false;

  typedef HdBufferArrayRangeSharedPtr HdBarPtr;
  if (HdBarPtr const &bar = drawItem->GetConstantPrimvarRange()) {
    HdPhBufferArrayRangeSharedPtr bar_ = std::static_pointer_cast<HdPhBufferArrayRange>(bar);
    hasAuthoredResouce |= bool(bar_->GetResource(resourceToken));
  }
  if (HdBarPtr const &bar = drawItem->GetVertexPrimvarRange()) {
    HdPhBufferArrayRangeSharedPtr bar_ = std::static_pointer_cast<HdPhBufferArrayRange>(bar);
    hasAuthoredResouce |= bool(bar_->GetResource(resourceToken));
  }
  if (HdBarPtr const &bar = drawItem->GetVaryingPrimvarRange()) {
    HdPhBufferArrayRangeSharedPtr bar_ = std::static_pointer_cast<HdPhBufferArrayRange>(bar);

    hasAuthoredResouce |= bool(bar_->GetResource(resourceToken));
  }
  if (HdBarPtr const &bar = drawItem->GetElementPrimvarRange()) {
    HdPhBufferArrayRangeSharedPtr bar_ = std::static_pointer_cast<HdPhBufferArrayRange>(bar);

    hasAuthoredResouce |= bool(bar_->GetResource(resourceToken));
  }
  int instanceNumLevels = drawItem->GetInstancePrimvarNumLevels();
  for (int i = 0; i < instanceNumLevels; ++i) {
    if (HdBarPtr const &bar = drawItem->GetInstancePrimvarRange(i)) {
      HdPhBufferArrayRangeSharedPtr bar_ = std::static_pointer_cast<HdPhBufferArrayRange>(bar);

      hasAuthoredResouce |= bool(bar_->GetResource(resourceToken));
    }
  }
  return hasAuthoredResouce;
}

bool HdPhBasisCurves::_SupportsRefinement(int refineLevel)
{
  if (!_topology) {
    TF_CODING_ERROR("Calling _SupportsRefinement before topology is set");
    return false;
  }

  return refineLevel > 0 || IsEnabledForceRefinedCurves();
}

bool HdPhBasisCurves::_SupportsUserWidths(HdPhDrawItem *drawItem)
{
  return HdPh_HasResource(drawItem, HdTokens->widths);
}
bool HdPhBasisCurves::_SupportsUserNormals(HdPhDrawItem *drawItem)
{
  return HdPh_HasResource(drawItem, HdTokens->normals);
}

HdDirtyBits HdPhBasisCurves::GetInitialDirtyBitsMask() const
{
  HdDirtyBits mask = HdChangeTracker::Clean | HdChangeTracker::InitRepr |
                     HdChangeTracker::DirtyExtent | HdChangeTracker::DirtyNormals |
                     HdChangeTracker::DirtyPoints | HdChangeTracker::DirtyPrimID |
                     HdChangeTracker::DirtyPrimvar | HdChangeTracker::DirtyDisplayStyle |
                     HdChangeTracker::DirtyRepr | HdChangeTracker::DirtyMaterialId |
                     HdChangeTracker::DirtyTopology | HdChangeTracker::DirtyTransform |
                     HdChangeTracker::DirtyVisibility | HdChangeTracker::DirtyWidths |
                     HdChangeTracker::DirtyComputationPrimvarDesc |
                     HdChangeTracker::DirtyInstancer;

  return mask;
}

WABI_NAMESPACE_END
