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

#include "wabi/base/gf/size2.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"
#include "wabi/wabi.h"

#include <boost/python/class.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_arg.hpp>

#include <string>

using namespace boost::python;

using std::string;

WABI_NAMESPACE_USING

namespace
{

  static int normalizeIndex(int index)
  {
    return TfPyNormalizeIndex(index, 2, true /*throw error*/);
  }

  static int __len__(const GfSize2 &self)
  {
    return 2;
  }

  static size_t __getitem__(const GfSize2 &self, int index)
  {
    index = normalizeIndex(index);
    return self[index];
  }

  static void __setitem__(GfSize2 &self, int index, size_t value)
  {
    index = normalizeIndex(index);
    self[index] = value;
  }

  static bool __contains__(const GfSize2 &self, size_t value)
  {
    if (self[0] == value || self[1] == value)
      return true;
    return false;
  }

#if PY_MAJOR_VERSION == 2
  static GfSize2 __truediv__(const GfSize2 &self, int value)
  {
    return self / value;
  }

  static GfSize2 __itruediv__(GfSize2 &self, int value)
  {
    return self /= value;
  }
#endif

  static string _Repr(GfSize2 const &self)
  {
    return TF_PY_REPR_PREFIX + "Size2(" + TfPyRepr(self[0]) + ", " + TfPyRepr(self[1]) + ")";
  }

}  // anonymous namespace

void wrapSize2()
{
  typedef GfSize2 This;

  static const int dimension = 2;

  class_<This>("Size2", "A 2D size class", init<>())
    .def(init<const This &>())
    .def(init<const GfVec2i &>())
    .def(init<size_t, size_t>())

    .def(TfTypePythonClass())

    .def("Set", (GfSize2 & (This::*)(size_t, size_t)) & This::Set, return_self<>())

    .def_readonly("dimension", dimension)

    .def("__len__", __len__)
    .def("__getitem__", __getitem__)
    .def("__setitem__", __setitem__)
    .def("__contains__", __contains__)

    .def(str(self))
    .def(self == self)
    .def(self != self)
    .def(self += self)
    .def(self -= self)
    .def(self *= int())
    .def(self /= int())
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(int() * self)
    .def(self * int())
    .def(self / int())

#if PY_MAJOR_VERSION == 2
    // Needed only to support "from __future__ import division" in
    // python 2. In python 3 builds boost::python adds this for us.
    .def("__truediv__", __truediv__)
    .def("__itruediv__", __itruediv__)
#endif

    .def("__repr__", _Repr)

    ;
  to_python_converter<std::vector<This>, TfPySequenceToPython<std::vector<This>>>();

  // conversion operator
  implicitly_convertible<This, GfVec2i>();
}
