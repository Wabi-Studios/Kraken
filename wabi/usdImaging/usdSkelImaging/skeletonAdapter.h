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
#ifndef WABI_USD_IMAGING_USD_SKEL_IMAGING_SKELETON_ADAPTER_H
#define WABI_USD_IMAGING_USD_SKEL_IMAGING_SKELETON_ADAPTER_H

#include "wabi/usdImaging/usdImaging/primAdapter.h"
#include "wabi/usdImaging/usdSkelImaging/api.h"
#include "wabi/wabi.h"

#include "wabi/imaging/hd/meshTopology.h"

#include "wabi/usd/usdSkel/binding.h"
#include "wabi/usd/usdSkel/blendShapeQuery.h"
#include "wabi/usd/usdSkel/cache.h"
#include "wabi/usd/usdSkel/skeleton.h"
#include "wabi/usd/usdSkel/skeletonQuery.h"

#include <boost/unordered_map.hpp>
#include <unordered_map>

WABI_NAMESPACE_BEGIN

/// \class UsdImagingSkeletonAdapter
///
/// Support for drawing bones of a UsdSkelSkeleton.
///
class UsdSkelImagingSkeletonAdapter : public UsdImagingPrimAdapter
{
 public:

  using BaseAdapter = UsdImagingPrimAdapter;

  UsdSkelImagingSkeletonAdapter() : BaseAdapter() {}

  USDSKELIMAGING_API
  virtual ~UsdSkelImagingSkeletonAdapter();

  USDSKELIMAGING_API
  SdfPath Populate(const UsdPrim &prim,
                   UsdImagingIndexProxy *index,
                   const UsdImagingInstancerContext *instancerContext = nullptr) override;

  USDSKELIMAGING_API
  bool IsSupported(const UsdImagingIndexProxy *index) const override;

  // ---------------------------------------------------------------------- //
  /// \name Parallel Setup and Resolve
  // ---------------------------------------------------------------------- //

  /// Thread Safe.
  USDSKELIMAGING_API
  void TrackVariability(
    const UsdPrim &prim,
    const SdfPath &cachePath,
    HdDirtyBits *timeVaryingBits,
    const UsdImagingInstancerContext *instancerContext = nullptr) const override;

  /// Thread Safe.
  USDSKELIMAGING_API
  void UpdateForTime(const UsdPrim &prim,
                     const SdfPath &cachePath,
                     UsdTimeCode time,
                     HdDirtyBits requestedBits,
                     const UsdImagingInstancerContext *instancerContext = nullptr) const override;

  // ---------------------------------------------------------------------- //
  /// \name Change Processing API (public)
  // ---------------------------------------------------------------------- //

  USDSKELIMAGING_API
  HdDirtyBits ProcessPropertyChange(const UsdPrim &prim,
                                    const SdfPath &cachePath,
                                    const TfToken &propertyName) override;

  USDSKELIMAGING_API
  void ProcessPrimResync(SdfPath const &primPath, UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void ProcessPrimRemoval(SdfPath const &primPath, UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkDirty(const UsdPrim &prim,
                 const SdfPath &cachePath,
                 HdDirtyBits dirty,
                 UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkRefineLevelDirty(UsdPrim const &prim,
                            SdfPath const &cachePath,
                            UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkReprDirty(UsdPrim const &prim,
                     SdfPath const &cachePath,
                     UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkCullStyleDirty(UsdPrim const &prim,
                          SdfPath const &cachePath,
                          UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkRenderTagDirty(UsdPrim const &prim,
                          SdfPath const &cachePath,
                          UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkTransformDirty(const UsdPrim &prim,
                          const SdfPath &cachePath,
                          UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkVisibilityDirty(const UsdPrim &prim,
                           const SdfPath &cachePath,
                           UsdImagingIndexProxy *index) override;

  USDSKELIMAGING_API
  void MarkMaterialDirty(const UsdPrim &prim,
                         const SdfPath &cachePath,
                         UsdImagingIndexProxy *index) override;

  // ---------------------------------------------------------------------- //
  /// \name Computation API
  // ---------------------------------------------------------------------- //
  USDSKELIMAGING_API
  void InvokeComputation(SdfPath const &cachePath, HdExtComputationContext *context) override;

  // ---------------------------------------------------------------------- //
  /// \name Non-virtual public API
  // ---------------------------------------------------------------------- //

  USDSKELIMAGING_API
  void RegisterSkelBinding(UsdSkelBinding const &binding);

  // ---------------------------------------------------------------------- //
  /// \name Virtual public API
  // ---------------------------------------------------------------------- //

  USDSKELIMAGING_API
  PxOsdSubdivTags GetSubdivTags(UsdPrim const &usdPrim,
                                SdfPath const &cachePath,
                                UsdTimeCode time) const override;

  USDSKELIMAGING_API
  VtValue GetTopology(UsdPrim const &prim,
                      SdfPath const &cachePath,
                      UsdTimeCode time) const override;

  USDSKELIMAGING_API
  GfRange3d GetExtent(UsdPrim const &prim,
                      SdfPath const &cachePath,
                      UsdTimeCode time) const override;

  USDSKELIMAGING_API
  TfToken GetPurpose(UsdPrim const &prim,
                     SdfPath const &cachePath,
                     TfToken const &instanceInheritablePurpose) const override;

  USDSKELIMAGING_API
  bool GetDoubleSided(UsdPrim const &prim,
                      SdfPath const &cachePath,
                      UsdTimeCode time) const override;

  USDSKELIMAGING_API
  SdfPath GetMaterialId(UsdPrim const &prim,
                        SdfPath const &cachePath,
                        UsdTimeCode time) const override;

  USDSKELIMAGING_API
  const TfTokenVector &GetExtComputationSceneInputNames(SdfPath const &cachePath) const override;

  USDSKELIMAGING_API
  HdExtComputationInputDescriptorVector GetExtComputationInputs(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    const UsdImagingInstancerContext *instancerContext) const override;

  HdExtComputationOutputDescriptorVector GetExtComputationOutputs(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    const UsdImagingInstancerContext *instancerContext) const override;

  HdExtComputationPrimvarDescriptorVector GetExtComputationPrimvars(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    HdInterpolation interpolation,
    const UsdImagingInstancerContext *instancerContext) const override;

  USDIMAGING_API
  VtValue GetExtComputationInput(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    TfToken const &name,
    UsdTimeCode time,
    const UsdImagingInstancerContext *instancerContext) const override;

  USDIMAGING_API
  size_t SampleExtComputationInput(UsdPrim const &prim,
                                   SdfPath const &cachePath,
                                   TfToken const &name,
                                   UsdTimeCode time,
                                   const UsdImagingInstancerContext *instancerContext,
                                   size_t maxSampleCount,
                                   float *sampleTimes,
                                   VtValue *sampleValues) override;

  USDIMAGING_API
  std::string GetExtComputationKernel(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    const UsdImagingInstancerContext *instancerContext) const override;

  USDSKELIMAGING_API
  VtValue Get(UsdPrim const &prim,
              SdfPath const &cachePath,
              TfToken const &key,
              UsdTimeCode time,
              VtIntArray *outIndices) const override;

 protected:

  // ---------------------------------------------------------------------- //
  /// \name Change Processing API (protected)
  // ---------------------------------------------------------------------- //
  void _RemovePrim(const SdfPath &cachePath, UsdImagingIndexProxy *index) override;

 private:

  // ---------------------------------------------------------------------- //
  /// Handlers for the Bone Mesh
  // ---------------------------------------------------------------------- //
  bool _IsCallbackForSkeleton(const UsdPrim &prim) const;

  /// Returns a value holding color for \p prim,
  /// taking into account explicitly authored color on the prim.
  GfVec3f _GetSkeletonDisplayColor(const UsdPrim &prim, UsdTimeCode time) const;

  /// Returns a value holding opacity for \p prim,
  /// taking into account explicitly authored opacity on the prim.
  float _GetSkeletonDisplayOpacity(const UsdPrim &prim, UsdTimeCode time) const;

  void _TrackBoneMeshVariability(
    const UsdPrim &prim,
    const SdfPath &cachePath,
    HdDirtyBits *timeVaryingBits,
    const UsdImagingInstancerContext *instancerContext = nullptr) const;

  void _UpdateBoneMeshForTime(const UsdPrim &prim,
                              const SdfPath &cachePath,
                              UsdTimeCode time,
                              HdDirtyBits requestedBits,
                              const UsdImagingInstancerContext *instancerContext = nullptr) const;

  // ---------------------------------------------------------------------- //
  /// Common utitily methods for skinning computations & skinned prims
  // ---------------------------------------------------------------------- //
  bool _IsAffectedByTimeVaryingSkelAnim(const SdfPath &skinnedPrimPath) const;

  void _RemoveSkinnedPrimAndComputations(const SdfPath &cachePath, UsdImagingIndexProxy *index);

  // ---------------------------------------------------------------------- //
  /// Handlers for the skinning computations
  // ---------------------------------------------------------------------- //
  bool _IsSkinningComputationPath(const SdfPath &cachePath) const;

  bool _IsSkinningInputAggregatorComputationPath(const SdfPath &cachePath) const;

  void _TrackSkinningComputationVariability(
    const UsdPrim &skinnedPrim,
    const SdfPath &computationPath,
    HdDirtyBits *timeVaryingBits,
    const UsdImagingInstancerContext *instancerContext = nullptr) const;

  VtVec3fArray _GetSkinnedPrimPoints(const UsdPrim &skinnedPrim,
                                     const SdfPath &skinnedPrimCachePath,
                                     UsdTimeCode time) const;

  SdfPath _GetSkinningComputationPath(const SdfPath &skinnedPrimPath) const;

  SdfPath _GetSkinningInputAggregatorComputationPath(const SdfPath &skinnedPrimPath) const;

  // Static helper methods
  static std::string _LoadSkinningComputeKernel();

  static const std::string &_GetSkinningComputeKernel();

  // ---------------------------------------------------------------------- //
  /// Handlers for the skinned prim
  // ---------------------------------------------------------------------- //
  bool _IsSkinnedPrimPath(const SdfPath &cachePath) const;

  void _TrackSkinnedPrimVariability(
    const UsdPrim &prim,
    const SdfPath &cachePath,
    HdDirtyBits *timeVaryingBits,
    const UsdImagingInstancerContext *instancerContext = nullptr) const;

  void _UpdateSkinnedPrimForTime(
    const UsdPrim &prim,
    const SdfPath &cachePath,
    UsdTimeCode time,
    HdDirtyBits requestedBits,
    const UsdImagingInstancerContext *instancerContext = nullptr) const;

  // ---------------------------------------------------------------------- //
  /// GetExtComputationInput() helpers
  // ---------------------------------------------------------------------- //

  VtValue _GetExtComputationInputForSkinningComputation(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    TfToken const &name,
    UsdTimeCode time,
    const UsdImagingInstancerContext *instancerContext) const;

  VtValue _GetExtComputationInputForInputAggregator(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    TfToken const &name,
    UsdTimeCode time,
    const UsdImagingInstancerContext *instancerContext) const;

  size_t _SampleExtComputationInputForSkinningComputation(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    TfToken const &name,
    UsdTimeCode time,
    const UsdImagingInstancerContext *instancerContext,
    size_t maxSampleCount,
    float *sampleTimes,
    VtValue *sampleValues);

  size_t _SampleExtComputationInputForInputAggregator(
    UsdPrim const &prim,
    SdfPath const &cachePath,
    TfToken const &name,
    UsdTimeCode time,
    const UsdImagingInstancerContext *instancerContext,
    size_t maxSampleCount,
    float *sampleTimes,
    VtValue *sampleValues);

  // ---------------------------------------------------------------------- //
  /// Populated skeleton state
  // ---------------------------------------------------------------------- //
  /// Data for a skel instance.
  struct _SkelData
  {

    UsdSkelSkeletonQuery skelQuery;
    SdfPathSet skelRootPaths;

    /// Compute bone mesh topology, and intiailize
    /// other rest-state data for imaging bones.
    HdMeshTopology ComputeTopologyAndRestState();

    /// Compute animated  bone mesh points.
    VtVec3fArray ComputePoints(UsdTimeCode time) const;

    TfToken ComputePurpose() const;

   private:

    // Cache of a mesh for a skeleton (at rest)
    // TODO: Dedupe this infromation across UsdSkelSkeleton instances.
    VtVec3fArray _boneMeshPoints;
    VtIntArray _boneMeshJointIndices;
    size_t _numJoints;
  };

  _SkelData *_GetSkelData(const SdfPath &cachePath) const;

  UsdSkelCache _skelCache;
  using _SkelDataMap = std::unordered_map<SdfPath, std::shared_ptr<_SkelData>, SdfPath::Hash>;
  _SkelDataMap _skelDataCache;

  // Data for each skinned prim.
  struct _SkinnedPrimData
  {
    _SkinnedPrimData() = default;

    /// Construct skinned prim, referencing the skeleton at \p skelPath.
    /// XXX: We cannot extract the skel path from the skelQuery because
    /// the skelQuery might refer to a prim within an instance master.
    _SkinnedPrimData(const SdfPath &skelPath,
                     const UsdSkelSkeletonQuery &skelQuery,
                     const UsdSkelSkinningQuery &skinningQuery,
                     const SdfPath &skelRootPath);

    std::shared_ptr<UsdSkelBlendShapeQuery> blendShapeQuery;
    UsdSkelSkinningQuery skinningQuery;
    UsdSkelAnimQuery animQuery;
    SdfPath skelPath, skelRootPath;
    bool hasJointInfluences = false;
  };

  const _SkinnedPrimData *_GetSkinnedPrimData(const SdfPath &cachePath) const;

  using _SkinnedPrimDataMap = std::unordered_map<SdfPath, _SkinnedPrimData, SdfPath::Hash>;
  _SkinnedPrimDataMap _skinnedPrimDataCache;

  // ---------------------------------------------------------------------- //
  /// Skeleton -> Skinned Prim(s) state
  /// (Populated via UsdSkelImagingSkelRootAdapter::Populate)
  // ---------------------------------------------------------------------- //

  using _SkelBindingMap = std::unordered_map<SdfPath, UsdSkelBinding, SdfPath::Hash>;
  _SkelBindingMap _skelBindingMap;
};

WABI_NAMESPACE_END

#endif  // USDSKELIMAGING_SKELETONADAPTER
