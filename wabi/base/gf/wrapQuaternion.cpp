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

#include "wabi/base/gf/quaternion.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"
#include "wabi/wabi.h"

#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/def.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_arg.hpp>

#include <string>

using namespace boost::python;

using std::string;

WABI_NAMESPACE_USING

namespace
{

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GetNormalized_overloads, GetNormalized, 0, 1);
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Normalize_overloads, Normalize, 0, 1);

#if PY_MAJOR_VERSION == 2
  static GfQuaternion __truediv__(const GfQuaternion &self, double value)
  {
    return self / value;
  }

  static GfQuaternion __itruediv__(GfQuaternion &self, double value)
  {
    return self /= value;
  }
#endif

  static string _Repr(GfQuaternion const &self)
  {
    return TF_PY_REPR_PREFIX + "Quaternion(" + TfPyRepr(self.GetReal()) + ", " +
           TfPyRepr(self.GetImaginary()) + ")";
  }

  static size_t __hash__(GfQuaternion const &self)
  {
    return hash_value(self);
  }

}  // anonymous namespace

void wrapQuaternion()
{
  typedef GfQuaternion This;

  object getImaginary = make_function(&This::GetImaginary, return_value_policy<return_by_value>());

  def("Slerp", (GfQuaternion(*)(double, const GfQuaternion &, const GfQuaternion &))GfSlerp);

  class_<This>("Quaternion", "Quaternion class", init<>())

    .def(init<int>())

    .def(init<double, const GfVec3d &>())

    .def(TfTypePythonClass())

    .def("GetIdentity", &This::GetIdentity)
    .staticmethod("GetIdentity")

    .add_property("real", &This::GetReal, &This::SetReal)
    .add_property("imaginary", getImaginary, &This::SetImaginary)

    .def("GetImaginary", getImaginary)
    .def("GetInverse", &This::GetInverse)
    .def("GetLength", &This::GetLength)
    .def("GetReal", &This::GetReal)

    .def("GetNormalized", &This::GetNormalized, GetNormalized_overloads())
    .def("Normalize", &This::Normalize, Normalize_overloads()[return_self<>()])

    .def(str(self))
    .def(self == self)
    .def(self != self)
    .def(self *= self)
    .def(self *= double())
    .def(self /= double())
    .def(self += self)
    .def(self -= self)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self * double())
    .def(double() * self)
    .def(self / double())

#if PY_MAJOR_VERSION == 2
    // Needed only to support "from __future__ import division" in
    // python 2. In python 3 builds boost::python adds this for us.
    .def("__truediv__", __truediv__)
    .def("__itruediv__", __itruediv__)
#endif

    .def("__repr__", _Repr)
    .def("__hash__", __hash__)

    ;
  to_python_converter<std::vector<This>, TfPySequenceToPython<std::vector<This>>>();
}
