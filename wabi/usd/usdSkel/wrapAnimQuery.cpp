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
#include "wabi/usd/usdSkel/animQuery.h"

#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"
#include "wabi/usd/usd/pyConversions.h"

#include "wabi/usd/usd/prim.h"

#include "wabi/base/gf/interval.h"

#include <boost/python.hpp>

#include <vector>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace
{

  VtMatrix4dArray _ComputeJointLocalTransforms(const UsdSkelAnimQuery &self, UsdTimeCode time)
  {
    VtMatrix4dArray xforms;
    self.ComputeJointLocalTransforms(&xforms, time);
    return xforms;
  }

  boost::python::tuple _ComputeJointLocalTransformComponents(const UsdSkelAnimQuery &self, UsdTimeCode time)
  {
    VtVec3fArray translations;
    VtQuatfArray rotations;
    VtVec3hArray scales;
    self.ComputeJointLocalTransformComponents(&translations, &rotations, &scales, time);
    return boost::python::make_tuple(translations, rotations, scales);
  }

  VtFloatArray _ComputeBlendShapeWeights(const UsdSkelAnimQuery &self, UsdTimeCode time)
  {
    VtFloatArray weights;
    self.ComputeBlendShapeWeights(&weights, time);
    return weights;
  }

  std::vector<double> _GetJointTransformTimeSamples(const UsdSkelAnimQuery &self)
  {
    std::vector<double> times;
    self.GetJointTransformTimeSamples(&times);
    return times;
  }

  std::vector<double> _GetJointTransformTimeSamplesInInterval(const UsdSkelAnimQuery &self,
                                                              const GfInterval &interval)
  {
    std::vector<double> times;
    self.GetJointTransformTimeSamplesInInterval(interval, &times);
    return times;
  }

  std::vector<double> _GetBlendShapeWeightTimeSamples(const UsdSkelAnimQuery &self)
  {
    std::vector<double> times;
    self.GetBlendShapeWeightTimeSamples(&times);
    return times;
  }

  std::vector<double> _GetBlendShapeWeightTimeSamplesInInterval(const UsdSkelAnimQuery &self,
                                                                const GfInterval &interval)
  {
    std::vector<double> times;
    self.GetBlendShapeWeightTimeSamplesInInterval(interval, &times);
    return times;
  }

}  // namespace

void wrapUsdSkelAnimQuery()
{
  using This = UsdSkelAnimQuery;

  class_<This>("AnimQuery", no_init)

    .def(!self)
    .def(self == self)
    .def(self != self)

    .def("__str__", &This::GetDescription)

    .def("GetPrim", &This::GetPrim)

    .def("ComputeJointLocalTransforms",
         &_ComputeJointLocalTransforms,
         (arg("time") = UsdTimeCode::Default()))

    .def("ComputeJointLocalTransformComponents",
         &_ComputeJointLocalTransformComponents,
         (arg("time") = UsdTimeCode::Default()))

    .def("ComputeBlendShapeWeights", &_ComputeBlendShapeWeights, (arg("time") = UsdTimeCode::Default()))

    .def("GetJointTransformTimeSamples", &_GetJointTransformTimeSamples)

    .def("GetJointTransformTimeSamplesInInterval",
         &_GetJointTransformTimeSamplesInInterval,
         (arg("interval")))

    .def("JointTransformsMightBeTimeVarying", &This::JointTransformsMightBeTimeVarying)

    .def("GetBlendShapeWeightTimeSamples", &_GetBlendShapeWeightTimeSamples)

    .def("GetBlendShapeWeightTimeSamplesInInterval",
         &_GetBlendShapeWeightTimeSamplesInInterval,
         (arg("interval")))

    .def("BlendShapeWeightsMightBeTimeVarying", &This::BlendShapeWeightsMightBeTimeVarying)

    .def("GetJointOrder", &This::GetJointOrder)

    .def("GetBlendShapeOrder", &This::GetBlendShapeOrder);
}
