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
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// wrapRange.template.cpp file to make changes.

#include "wabi/base/gf/range3d.h"
#include "wabi/base/gf/range3f.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_arg.hpp>

#include <string>

using namespace boost::python;

using std::string;

WABI_NAMESPACE_USING

namespace {

static const int _dimension = 3;

static string _Repr(GfRange3d const &self)
{
  return TF_PY_REPR_PREFIX + "Range3d(" + TfPyRepr(self.GetMin()) + ", " +
         TfPyRepr(self.GetMax()) + ")";
}

#if PY_MAJOR_VERSION == 2
static GfRange3d __truediv__(const GfRange3d &self, double value)
{
  return self / value;
}

static GfRange3d __itruediv__(GfRange3d &self, double value)
{
  return self /= value;
}
#endif

static size_t __hash__(GfRange3d const &r)
{
  return hash_value(r);
}

}  // anonymous namespace

void wrapRange3d()
{
  object getMin = make_function(&GfRange3d::GetMin, return_value_policy<return_by_value>());

  object getMax = make_function(&GfRange3d::GetMax, return_value_policy<return_by_value>());

  class_<GfRange3d>("Range3d", init<>())
      .def(init<GfRange3d>())
      .def(init<const GfVec3d &, const GfVec3d &>())

      .def(TfTypePythonClass())

      .def_readonly("dimension", _dimension)

      .add_property("min", getMin, &GfRange3d::SetMin)
      .add_property("max", getMax, &GfRange3d::SetMax)

      .def("GetMin", getMin)
      .def("GetMax", getMax)

      .def("GetSize", &GfRange3d::GetSize)
      .def("GetMidpoint", &GfRange3d::GetMidpoint)

      .def("SetMin", &GfRange3d::SetMin)
      .def("SetMax", &GfRange3d::SetMax)

      .def("IsEmpty", &GfRange3d::IsEmpty)

      .def("SetEmpty", &GfRange3d::SetEmpty)

      .def("Contains", (bool (GfRange3d::*)(const GfVec3d &) const) & GfRange3d::Contains)
      .def("Contains", (bool (GfRange3d::*)(const GfRange3d &) const) & GfRange3d::Contains)

      .def("GetUnion", &GfRange3d::GetUnion)
      .staticmethod("GetUnion")

      .def("UnionWith",
           (const GfRange3d &(GfRange3d::*)(const GfVec3d &)) & GfRange3d::UnionWith,
           return_self<>())
      .def("UnionWith",
           (const GfRange3d &(GfRange3d::*)(const GfRange3d &)) & GfRange3d::UnionWith,
           return_self<>())

      .def("GetIntersection", &GfRange3d::GetIntersection)
      .staticmethod("GetIntersection")

      .def("IntersectWith",
           (const GfRange3d &(GfRange3d::*)(const GfRange3d &)) & GfRange3d::IntersectWith,
           return_self<>())

      .def("GetDistanceSquared", &GfRange3d::GetDistanceSquared)

      .def(str(self))
      .def(self += self)
      .def(self -= self)
      .def(self *= double())
      .def(self /= double())
      .def(self + self)
      .def(self - self)
      .def(double() * self)
      .def(self * double())
      .def(self / double())
      .def(self == GfRange3f())
      .def(self != GfRange3f())
      .def(self == self)
      .def(self != self)

#if PY_MAJOR_VERSION == 2
      // Needed only to support "from __future__ import division" in
      // python 2. In python 3 builds boost::python adds this for us.
      .def("__truediv__", __truediv__)
      .def("__itruediv__", __itruediv__)
#endif

      .def("__repr__", _Repr)
      .def("__hash__", __hash__)

      .def("GetCorner", &GfRange3d::GetCorner)
      .def("GetOctant", &GfRange3d::GetOctant)
      .def_readonly("unitCube", &GfRange3d::UnitCube)

      ;
  to_python_converter<std::vector<GfRange3d>, TfPySequenceToPython<std::vector<GfRange3d>>>();
}
