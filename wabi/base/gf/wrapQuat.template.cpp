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

#include "wabi/wabi.h"
{% for S in SCALARS %
}
#include "wabi/base/gf/quat{{ SCALAR_SUFFIX(S) }}.h"
{
  % endfor %
}

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

static string __repr__({
  {
    QUAT
  }
} const &self)
{
  return TF_PY_REPR_PREFIX + "Quat{{ SUFFIX }}(" + TfPyRepr(self.GetReal()) + ", " +
         TfPyRepr(self.GetImaginary()) + ")";
}

#if PY_MAJOR_VERSION == 2
static
{
  {
    QUAT
  }
}
__truediv__(const {{QUAT}} & self, {
  {
    SCL
  }
} value)
{
  return self / value;
}

static
{
  {
    QUAT
  }
}
__itruediv__({{QUAT}} & self, {
  {
    SCL
  }
} value)
{
  return self /= value;
}
#endif

// Zero-initialized default ctor for python.
static {{QUAT}} * __init__()
{
  return new {{QUAT}}(0);
}

}  // anonymous namespace

void wrapQuat{{SUFFIX}}()
{
  object getImaginary = make_function(
    &{
      {
        QUAT
      }
    } ::GetImaginary,
    return_value_policy<return_by_value>());

  object setImaginaryVec = make_function((void ({
                                           {
                                             QUAT
                                           }
                                         } ::*)(const GfVec3{{SUFFIX}} &)) &
                                         {
                                           {
                                             QUAT
                                           }
                                         } ::SetImaginary);

  object setImaginaryScl = make_function((void ({
                                           {
                                             QUAT
                                           }
                                         } ::*)({{SCL}}, {{SCL}}, {{SCL}})) &
                                           {
                                             {
                                               QUAT
                                             }
                                           } ::SetImaginary,
                                         default_call_policies(),
                                         (arg("i"), arg("j"), arg("k")));

  def("Slerp", ({{QUAT}}(*)(double, const {{QUAT}} &, const {{QUAT}} &))GfSlerp);

  def("Dot", ({{SCL}}(*)(const {{QUAT}} &, const {{QUAT}} &))GfDot);

  class_<{{QUAT}}>("Quat{{ SUFFIX }}", no_init)
    .def("__init__", make_constructor(__init__))

    .def(TfTypePythonClass())

    .def(init<GfQuat{{SUFFIX}}>())
    .def(init<{{SCL}}>(arg("real")))
    .def(init<{{SCL}}, const GfVec3{{SUFFIX}} &>((arg("real"), arg("imaginary"))))
    .def(init<{{SCL}}, {{SCL}}, {{SCL}}, {{SCL}}>((arg("real"), arg("i"), arg("j"), arg("k"))))
  {% for S in SCALARS if S != SCL and not ALLOW_IMPLICIT_CONVERSION(S, SCL) %
  }
  .def(init<const GfQuat{{SCALAR_SUFFIX(S)}} &>()){ % endfor % }

    .def(
      "GetIdentity",
      &{
        {
          QUAT
        }
      } ::GetIdentity)
    .staticmethod("GetIdentity")

    .def(
      "GetReal",
      &{
        {
          QUAT
        }
      } ::GetReal)
    .def(
      "SetReal",
      &{
        {
          QUAT
        }
      } ::SetReal)
    .add_property(
      "real",
      &{
        {
          QUAT
        }
      } ::GetReal,
      &{
        {
          QUAT
        }
      } ::SetReal)

    .def("GetImaginary", getImaginary)
    .def("SetImaginary", setImaginaryVec)
    .def("SetImaginary", setImaginaryScl)
    .add_property("imaginary", getImaginary, setImaginaryVec)

    .def(
      "GetLength",
      &{
        {
          QUAT
        }
      } ::GetLength)

    .def(
      "GetNormalized",
      &{
        {
          QUAT
        }
      } ::GetNormalized,
      (arg("eps") = GF_MIN_VECTOR_LENGTH))
    .def(
      "Normalize",
      &{
        {
          QUAT
        }
      } ::Normalize,
      (arg("eps") = GF_MIN_VECTOR_LENGTH),
      return_self<>())

    .def(
      "GetConjugate",
      &{
        {
          QUAT
        }
      } ::GetConjugate)
    .def(
      "GetInverse",
      &{
        {
          QUAT
        }
      } ::GetInverse)

    .def(
      "Transform",
      &{
        {
          QUAT
        }
      } ::Transform)

    .def(str(self))
    .def(-self)
    .def(self == self)
    .def(self != self)
    .def(self *= self)
    .def(self *= {{SCL}}())
    .def(self /= {{SCL}}())
    .def(self += self)
    .def(self -= self)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self * {{SCL}}())
    .def({{SCL}}() * self)
    .def(self / {{SCL}}())

#if PY_MAJOR_VERSION == 2
    // Needed only to support "from __future__ import division" in
    // python 2. In python 3 builds boost::python adds this for us.
    .def("__truediv__", __truediv__)
    .def("__itruediv__", __itruediv__)
#endif

    .def("__repr__", __repr__)

    ;

  {% for S in SCALARS if S != SCL and ALLOW_IMPLICIT_CONVERSION(S, SCL) %
  }
  implicitly_convertible<{{QUATNAME(S)}}, {{QUAT}}>();
  { % endfor % }

  to_python_converter<std::vector<{{QUAT}}>, TfPySequenceToPython<std::vector<{{QUAT}}>>>();
}
