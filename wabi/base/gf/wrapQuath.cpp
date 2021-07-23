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
// wrapQuat.template.cpp file to make changes.

#include "wabi/base/gf/quatd.h"
#include "wabi/base/gf/quatf.h"
#include "wabi/base/gf/quath.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_arg.hpp>

#include <string>

using namespace boost::python;

using std::string;

WABI_NAMESPACE_USING

namespace
{

  static string __repr__(GfQuath const &self)
  {
    return TF_PY_REPR_PREFIX + "Quath(" + TfPyRepr(self.GetReal()) + ", " + TfPyRepr(self.GetImaginary()) +
           ")";
  }

#if PY_MAJOR_VERSION == 2
  static GfQuath __truediv__(const GfQuath &self, GfHalf value)
  {
    return self / value;
  }

  static GfQuath __itruediv__(GfQuath &self, GfHalf value)
  {
    return self /= value;
  }
#endif

  // Zero-initialized default ctor for python.
  static GfQuath *__init__()
  {
    return new GfQuath(0);
  }

}  // anonymous namespace

void wrapQuath()
{
  object getImaginary = make_function(&GfQuath::GetImaginary, return_value_policy<return_by_value>());

  object setImaginaryVec = make_function((void(GfQuath::*)(const GfVec3h &)) & GfQuath::SetImaginary);

  object setImaginaryScl = make_function((void(GfQuath::*)(GfHalf, GfHalf, GfHalf)) & GfQuath::SetImaginary,
                                         default_call_policies(),
                                         (arg("i"), arg("j"), arg("k")));

  def("Slerp", (GfQuath(*)(double, const GfQuath &, const GfQuath &))GfSlerp);

  def("Dot", (GfHalf(*)(const GfQuath &, const GfQuath &))GfDot);

  class_<GfQuath>("Quath", no_init)
    .def("__init__", make_constructor(__init__))

    .def(TfTypePythonClass())

    .def(init<GfQuath>())
    .def(init<GfHalf>(arg("real")))
    .def(init<GfHalf, const GfVec3h &>((arg("real"), arg("imaginary"))))
    .def(init<GfHalf, GfHalf, GfHalf, GfHalf>((arg("real"), arg("i"), arg("j"), arg("k"))))
    .def(init<const GfQuatd &>())
    .def(init<const GfQuatf &>())

    .def("GetIdentity", &GfQuath::GetIdentity)
    .staticmethod("GetIdentity")

    .def("GetReal", &GfQuath::GetReal)
    .def("SetReal", &GfQuath::SetReal)
    .add_property("real", &GfQuath::GetReal, &GfQuath::SetReal)

    .def("GetImaginary", getImaginary)
    .def("SetImaginary", setImaginaryVec)
    .def("SetImaginary", setImaginaryScl)
    .add_property("imaginary", getImaginary, setImaginaryVec)

    .def("GetLength", &GfQuath::GetLength)

    .def("GetNormalized", &GfQuath::GetNormalized, (arg("eps") = GF_MIN_VECTOR_LENGTH))
    .def("Normalize", &GfQuath::Normalize, (arg("eps") = GF_MIN_VECTOR_LENGTH), return_self<>())

    .def("GetConjugate", &GfQuath::GetConjugate)
    .def("GetInverse", &GfQuath::GetInverse)

    .def("Transform", &GfQuath::Transform)

    .def(str(self))
    .def(-self)
    .def(self == self)
    .def(self != self)
    .def(self *= self)
    .def(self *= GfHalf())
    .def(self /= GfHalf())
    .def(self += self)
    .def(self -= self)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self * GfHalf())
    .def(GfHalf() * self)
    .def(self / GfHalf())

#if PY_MAJOR_VERSION == 2
    // Needed only to support "from __future__ import division" in
    // python 2. In python 3 builds boost::python adds this for us.
    .def("__truediv__", __truediv__)
    .def("__itruediv__", __itruediv__)
#endif

    .def("__repr__", __repr__)

    ;

  to_python_converter<std::vector<GfQuath>, TfPySequenceToPython<std::vector<GfQuath>>>();
}
