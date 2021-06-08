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
#include "wabi/usd/usdGeom/metrics.h"
#include "wabi/wabi.h"

#include "wabi/usd/usd/stage.h"

#include "wabi/base/tf/pyResultConversions.h"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

void wrapMetrics()
{
  def("GetStageUpAxis", UsdGeomGetStageUpAxis, arg("stage"));
  def("SetStageUpAxis", UsdGeomSetStageUpAxis, (arg("stage"), arg("upAxis")));
  def("GetFallbackUpAxis", UsdGeomGetFallbackUpAxis);

  def("GetStageMetersPerUnit", UsdGeomGetStageMetersPerUnit, arg("stage"));
  def("StageHasAuthoredMetersPerUnit", UsdGeomStageHasAuthoredMetersPerUnit, arg("stage"));
  def("SetStageMetersPerUnit", UsdGeomSetStageMetersPerUnit, (arg("stage"), arg("metersPerUnit")));
  def("LinearUnitsAre",
      UsdGeomLinearUnitsAre,
      (arg("authoredUnits"), arg("standardUnits"), arg("epsilon") = 1e-5));

  boost::python::class_<UsdGeomLinearUnits> cls("LinearUnits", boost::python::no_init);
  cls.def_readonly("nanometers", UsdGeomLinearUnits::nanometers)
      .def_readonly("micrometers", UsdGeomLinearUnits::micrometers)
      .def_readonly("millimeters", UsdGeomLinearUnits::millimeters)
      .def_readonly("centimeters", UsdGeomLinearUnits::centimeters)
      .def_readonly("meters", UsdGeomLinearUnits::meters)
      .def_readonly("kilometers", UsdGeomLinearUnits::kilometers)
      .def_readonly("lightYears", UsdGeomLinearUnits::lightYears)
      .def_readonly("inches", UsdGeomLinearUnits::inches)
      .def_readonly("feet", UsdGeomLinearUnits::feet)
      .def_readonly("yards", UsdGeomLinearUnits::yards)
      .def_readonly("miles", UsdGeomLinearUnits::miles);
}
