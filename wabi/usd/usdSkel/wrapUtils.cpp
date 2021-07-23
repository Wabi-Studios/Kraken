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
#include "wabi/usd/usdSkel/utils.h"

#include "wabi/base/gf/matrix3d.h"
#include "wabi/base/gf/matrix3f.h"
#include "wabi/base/gf/matrix4d.h"
#include "wabi/base/gf/matrix4f.h"
#include "wabi/base/gf/range3f.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include "wabi/usd/usd/primRange.h"
#include "wabi/usd/usd/pyConversions.h"
#include "wabi/usd/usd/relationship.h"

#include "wabi/usd/usdSkel/root.h"
#include "wabi/usd/usdSkel/topology.h"
#include "wabi/usd/usdSkel/utils.h"

#include <boost/python.hpp>
#include <boost/python/extract.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace
{

  // deprecated
  VtMatrix4dArray _ComputeJointLocalTransforms(const UsdSkelTopology &topology,
                                               const VtMatrix4dArray &xforms,
                                               const VtMatrix4dArray &inverseXforms,
                                               const GfMatrix4d *rootInverseXform = nullptr)
  {
    VtMatrix4dArray jointLocalXforms;
    UsdSkelComputeJointLocalTransforms(topology, xforms, inverseXforms, &jointLocalXforms, rootInverseXform);
    return jointLocalXforms;
  }

  // deprecated
  VtMatrix4dArray _ComputeJointLocalTransforms_NoInvXforms(const UsdSkelTopology &topology,
                                                           const VtMatrix4dArray &xforms,
                                                           const GfMatrix4d *rootInverseXform = nullptr)
  {
    VtMatrix4dArray jointLocalXforms;
    UsdSkelComputeJointLocalTransforms(topology, xforms, &jointLocalXforms, rootInverseXform);
    return jointLocalXforms;
  }

  // depreacted
  VtMatrix4dArray _ConcatJointTransforms(const UsdSkelTopology &topology,
                                         const VtMatrix4dArray &jointLocalXforms,
                                         const GfMatrix4d *rootXform = nullptr)
  {
    VtMatrix4dArray xforms;
    UsdSkelConcatJointTransforms(topology, jointLocalXforms, &xforms, rootXform);
    return xforms;
  }

  template<typename Matrix4>
  tuple _DecomposeTransform(const Matrix4 &mx)
  {
    GfVec3f t;
    GfQuatf r;
    GfVec3h s;
    if (!UsdSkelDecomposeTransform(mx, &t, &r, &s))
    {
      // XXX: Want this case to throw an exception.
      TF_CODING_ERROR(
        "Failed decomposing transform. "
        "The transform may be singular.");
    }
    return boost::python::make_tuple(t, r, s);
  }

  template<typename Matrix4>
  tuple _DecomposeTransforms(const TfSpan<Matrix4> &xforms)
  {
    VtVec3fArray t(xforms.size());
    VtQuatfArray r(xforms.size());
    VtVec3hArray s(xforms.size());
    if (!UsdSkelDecomposeTransforms(xforms, t, r, s))
    {
      TF_CODING_ERROR(
        "Failed decomposing transforms. "
        "Some transforms may be singular.");
    }
    return boost::python::make_tuple(t, r, s);
  }

  GfMatrix4d _MakeTransform(const GfVec3f &translate, const GfQuatf &rotate, const GfVec3h &scale)
  {
    GfMatrix4d xform;
    UsdSkelMakeTransform(translate, rotate, scale, &xform);
    return xform;
  }

  VtMatrix4dArray _MakeTransforms(TfSpan<const GfVec3f> translations,
                                  TfSpan<const GfQuatf> rotations,
                                  TfSpan<const GfVec3h> scales)
  {
    VtMatrix4dArray xforms(translations.size());
    UsdSkelMakeTransforms(translations, rotations, scales, xforms);
    return xforms;
  }

  template<typename Matrix4>
  GfRange3f _ComputeJointsExtent(TfSpan<const Matrix4> xforms,
                                 float pad = 0,
                                 const Matrix4 *rootXform = nullptr)
  {
    GfRange3f range;
    UsdSkelComputeJointsExtent(xforms, &range, pad, rootXform);
    return range;
  }

  template<typename T>
  bool _ExpandConstantInfluencesToVarying(VtArray<T> &array, size_t size)
  {
    return UsdSkelExpandConstantInfluencesToVarying(&array, size);
  }

  template<typename T>
  bool _ResizeInfluences(VtArray<T> &array, int srcNumInfluencesPerPoint, int newNumInfluencesPerPoint)
  {
    return UsdSkelResizeInfluences(&array, srcNumInfluencesPerPoint, newNumInfluencesPerPoint);
  }

  template<typename Matrix4>
  Matrix4 _InterleavedSkinTransformLBS(const Matrix4 &geomBindTransform,
                                       TfSpan<const Matrix4> jointXforms,
                                       TfSpan<const GfVec2f> influences)
  {
    Matrix4 xform;
    if (!UsdSkelSkinTransformLBS(geomBindTransform, jointXforms, influences, &xform))
    {
      xform = geomBindTransform;
    }
    return xform;
  }

  template<typename Matrix4>
  Matrix4 _NonInterleavedSkinTransformLBS(const Matrix4 &geomBindTransform,
                                          TfSpan<const Matrix4> jointXforms,
                                          TfSpan<const int> jointIndices,
                                          TfSpan<const float> jointWeights)
  {
    Matrix4 xform;
    if (!UsdSkelSkinTransformLBS(geomBindTransform, jointXforms, jointIndices, jointWeights, &xform))
    {
      xform = geomBindTransform;
    }
    return xform;
  }

  template<class Matrix3, class Matrix4>
  void _WrapUtilsT()
  {
    def("ComputeJointLocalTransforms",
        static_cast<bool (*)(const UsdSkelTopology &,
                             TfSpan<const Matrix4>,
                             TfSpan<const Matrix4>,
                             TfSpan<Matrix4>,
                             const Matrix4 *)>(&UsdSkelComputeJointLocalTransforms),
        (arg("topology"),
         arg("xforms"),
         arg("inverseXforms"),
         arg("jointLocalXforms"),
         arg("rootInverseXform") = object()));

    def("ComputeJointLocalTransforms",
        static_cast<
          bool (*)(const UsdSkelTopology &, TfSpan<const Matrix4>, TfSpan<Matrix4>, const Matrix4 *)>(
          &UsdSkelComputeJointLocalTransforms),
        (arg("topology"), arg("xforms"), arg("jointLocalXforms"), arg("rootInverseXform") = object()));

    def("ConcatJointTransforms",
        static_cast<
          bool (*)(const UsdSkelTopology &, TfSpan<const Matrix4>, TfSpan<Matrix4>, const Matrix4 *)>(
          &UsdSkelConcatJointTransforms),
        (arg("topology"), arg("jointLocalXforms"), arg("rootXform") = object()));

    def("DecomposeTransform",
        &_DecomposeTransform<Matrix4>,
        "Decompose a transform into a (translate,rotate,scale) tuple.");

    def("DecomposeTransforms",
        &_DecomposeTransforms<Matrix4>,
        "Decompose a transform array into a "
        "(translations,rotations,scales) tuple.");

    def("ComputeJointsExtent",
        _ComputeJointsExtent<Matrix4>,
        (arg("xforms"), arg("pad") = 0.0f, arg("rootXform") = object()));

    def("SkinPointsLBS",
        static_cast<bool (*)(const Matrix4 &,
                             TfSpan<const Matrix4>,
                             TfSpan<const int>,
                             TfSpan<const float>,
                             int,
                             TfSpan<GfVec3f>,
                             bool)>(&UsdSkelSkinPointsLBS),
        (arg("geomBindTransform"),
         arg("jointXforms"),
         arg("jointIndices"),
         arg("jointWeights"),
         arg("numInfluencesPerPoint"),
         arg("points"),
         arg("inSerial") = true));

    def(
      "SkinPointsLBS",
      static_cast<
        bool (*)(const Matrix4 &, TfSpan<const Matrix4>, TfSpan<const GfVec2f>, int, TfSpan<GfVec3f>, bool)>(
        &UsdSkelSkinPointsLBS),
      (arg("geomBindTransform"),
       arg("jointXforms"),
       arg("influences"),
       arg("numInfluencesPerPoint"),
       arg("points"),
       arg("inSerial") = true));

    def("SkinNormalsLBS",
        static_cast<bool (*)(const Matrix3 &,
                             TfSpan<const Matrix3>,
                             TfSpan<const int>,
                             TfSpan<const float>,
                             int,
                             TfSpan<GfVec3f>,
                             bool)>(&UsdSkelSkinNormalsLBS),
        (arg("geomBindTransform"),
         arg("jointXforms"),
         arg("jointIndices"),
         arg("jointWeights"),
         arg("numInfluencesPerPoint"),
         arg("normals"),
         arg("inSerial") = true));

    def(
      "SkinNormalsLBS",
      static_cast<
        bool (*)(const Matrix3 &, TfSpan<const Matrix3>, TfSpan<const GfVec2f>, int, TfSpan<GfVec3f>, bool)>(
        &UsdSkelSkinNormalsLBS),
      (arg("geomBindTransform"),
       arg("jointXforms"),
       arg("influences"),
       arg("numInfluencesPerPoint"),
       arg("normals"),
       arg("inSerial") = true));

    def("SkinTransformLBS",
        &_InterleavedSkinTransformLBS<Matrix4>,
        (arg("geomBindTransform"), arg("jointXforms"), arg("influences")));

    def("SkinTransformLBS",
        &_NonInterleavedSkinTransformLBS<Matrix4>,
        (arg("geomBindTransform"), arg("jointXforms"), arg("jointIndices"), arg("jointWeights")));
  }

}  // namespace

void wrapUsdSkelUtils()
{
  // Wrap methods supporting different matrix precisions.
  _WrapUtilsT<GfMatrix3d, GfMatrix4d>();
  _WrapUtilsT<GfMatrix3f, GfMatrix4f>();

  def("IsSkelAnimationPrim", &UsdSkelIsSkelAnimationPrim, (arg("prim")));

  def("IsSkinnablePrim", &UsdSkelIsSkinnablePrim, (arg("prim")));

  // deprecated
  def("ComputeJointLocalTransforms",
      &_ComputeJointLocalTransforms,
      (arg("topology"), arg("xforms"), arg("inverseXforms"), arg("rootInverseXform") = object()));

  // deprecated
  def("ComputeJointLocalTransforms",
      &_ComputeJointLocalTransforms_NoInvXforms,
      (arg("topology"), arg("xforms"), arg("rootInverseXform") = object()));

  // deprecated
  def("ConcatJointTransforms",
      &_ConcatJointTransforms,
      (arg("topology"), arg("jointLocalXforms"), arg("rootXform") = object()));

  def("MakeTransform", &_MakeTransform, (arg("translate"), arg("rotate"), arg("scale")));

  // deprecated
  def("MakeTransforms", &_MakeTransforms, (arg("translations"), arg("rotations"), arg("scales")));

  def("NormalizeWeights",
      static_cast<bool (*)(TfSpan<float>, int)>(&UsdSkelNormalizeWeights),
      (arg("weights"), arg("numInfluencesPerComponent")));

  def("SortInfluences",
      static_cast<bool (*)(TfSpan<int>, TfSpan<float>, int)>(&UsdSkelSortInfluences),
      (arg("indices"), arg("weights"), arg("numInfluencesPerComponent")));

  def("ExpandConstantInfluencesToVarying",
      &_ExpandConstantInfluencesToVarying<int>,
      (arg("array"), arg("size")));

  def("ExpandConstantInfluencesToVarying",
      &_ExpandConstantInfluencesToVarying<float>,
      (arg("array"), arg("size")));

  def("ResizeInfluences",
      &_ResizeInfluences<int>,
      (arg("array"), arg("srcNumInfluencesPerComponent"), arg("newNumInfluencesPerComponent")));

  def("ResizeInfluences",
      &_ResizeInfluences<float>,
      (arg("array"), arg("srcNumInfluencesPerComponent"), arg("newNumInfluencesPerComponent")));

  def("InterleaveInfluences",
      &UsdSkelInterleaveInfluences,
      (arg("indices"), arg("weights"), arg("interleavedInfluences")));

  def("ApplyBlendShape",
      &UsdSkelApplyBlendShape,
      (arg("weight"), arg("offsets"), arg("indices"), arg("points")));
}
