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

#include "wabi/base/gf/range{{ SUFFIX }}.h"
#include "wabi/wabi.h"
{% for S in SCALARS if S != SCL %
}
#include "wabi/base/gf/range{{ DIM }}{{ S[0] }}.h"
{
  % endfor %
}

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

namespace
{

  static const int _dimension = {{DIM}};

  static string _Repr({
    {
      RNG
    }
  } const &self)
  {
    return TF_PY_REPR_PREFIX + "Range{{ SUFFIX }}(" + TfPyRepr(self.GetMin()) + ", " +
           TfPyRepr(self.GetMax()) + ")";
  }

#if PY_MAJOR_VERSION == 2
  static
  {
    {
      RNG
    }
  }
  __truediv__(const {{RNG}} & self, double value)
  {
    return self / value;
  }

  static
  {
    {
      RNG
    }
  }
  __itruediv__({{RNG}} & self, double value)
  {
    return self /= value;
  }
#endif

  static size_t __hash__({
    {
      RNG
    }
  } const &r)
  {
    return hash_value(r);
  }

}  // anonymous namespace

void wrapRange{{SUFFIX}}()
{
  object getMin = make_function(
    &{
      {
        RNG
      }
    } ::GetMin,
    return_value_policy<return_by_value>());

  object getMax = make_function(
    &{
      {
        RNG
      }
    } ::GetMax,
    return_value_policy<return_by_value>());

  class_<{{RNG}}>("Range{{ SUFFIX }}", init<>())
    .def(init<{{RNG}}>())
    .def(init<{{MINMAXPARM}}, {{MINMAXPARM}}>())

    .def(TfTypePythonClass())

    .def_readonly("dimension", _dimension)

    .add_property(
      "min",
      getMin,
      &{
        {
          RNG
        }
      } ::SetMin)
    .add_property(
      "max",
      getMax,
      &{
        {
          RNG
        }
      } ::SetMax)

    .def("GetMin", getMin)
    .def("GetMax", getMax)

    .def(
      "GetSize",
      &{
        {
          RNG
        }
      } ::GetSize)
    .def(
      "GetMidpoint",
      &{
        {
          RNG
        }
      } ::GetMidpoint)

    .def(
      "SetMin",
      &{
        {
          RNG
        }
      } ::SetMin)
    .def(
      "SetMax",
      &{
        {
          RNG
        }
      } ::SetMax)

    .def(
      "IsEmpty",
      &{
        {
          RNG
        }
      } ::IsEmpty)

    .def(
      "SetEmpty",
      &{
        {
          RNG
        }
      } ::SetEmpty)

    .def(
      "Contains",
      (bool({
        {
          RNG
        }
      } ::*)({{MINMAXPARM}}) const) &
        {
          {
            RNG
          }
        } ::Contains)
    .def(
      "Contains",
      (bool({
        {
          RNG
        }
      } ::*)(const {{RNG}} &) const) &
        {
          {
            RNG
          }
        } ::Contains)

    .def(
      "GetUnion",
      &{
        {
          RNG
        }
      } ::GetUnion)
    .staticmethod("GetUnion")

    .def(
      "UnionWith",
      (const {{RNG}} & ({
         {
           RNG
         }
       } ::*)({{MINMAXPARM}})) &
        {
          {
            RNG
          }
        } ::UnionWith,
      return_self<>())
    .def(
      "UnionWith",
      (const {{RNG}} & ({
         {
           RNG
         }
       } ::*)(const {{RNG}} &)) &
        {
          {
            RNG
          }
        } ::UnionWith,
      return_self<>())

    .def(
      "GetIntersection",
      &{
        {
          RNG
        }
      } ::GetIntersection)
    .staticmethod("GetIntersection")

    .def(
      "IntersectWith",
      (const {{RNG}} & ({
         {
           RNG
         }
       } ::*)(const {{RNG}} &)) &
        {
          {
            RNG
          }
        } ::IntersectWith,
      return_self<>())

    .def(
      "GetDistanceSquared",
      &{
        {
          RNG
        }
      } ::GetDistanceSquared)

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
  {% for S in SCALARS if S != SCL %
  }
  .def(self == {{RNGNAME(DIM, S)}}())
    .def(self != {{RNGNAME(DIM, S)}}()){ % endfor % }
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

  {
    % if DIM == 2 %
  }
  .def(
    "GetCorner",
    &{
      {
        RNG
      }
    } ::GetCorner)
    .def(
      "GetQuadrant",
      &{
        {
          RNG
        }
      } ::GetQuadrant)
    .def_readonly(
      "unitSquare",
      &{
        {
          RNG
        }
      } ::UnitSquare){ % elif DIM == 3 % }
    .def(
      "GetCorner",
      &{
        {
          RNG
        }
      } ::GetCorner)
    .def(
      "GetOctant",
      &{
        {
          RNG
        }
      } ::GetOctant)
    .def_readonly(
      "unitCube",
      &{
        {
          RNG
        }
      } ::UnitCube){ % endif % }

  ;
  to_python_converter<std::vector<{{RNG}}>, TfPySequenceToPython<std::vector<{{RNG}}>>>();
}
