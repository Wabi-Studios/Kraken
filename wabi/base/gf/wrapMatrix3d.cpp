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
// wrapMatrix3.template.cpp file to make changes.

#include "wabi/base/gf/matrix3d.h"
#include "wabi/base/gf/matrix3f.h"
#include "wabi/wabi.h"

#include "wabi/base/gf/pyBufferUtils.h"

#include "wabi/base/gf/quatd.h"
#include "wabi/base/gf/rotation.h"

#include "wabi/base/tf/py3Compat.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/detail/api_placeholder.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_arg.hpp>
#include <boost/python/tuple.hpp>

#include <string>
#include <vector>

using namespace boost::python;
using std::string;
using std::vector;

WABI_NAMESPACE_USING

namespace
{

  ////////////////////////////////////////////////////////////////////////
  // Python buffer protocol support.

#if PY_MAJOR_VERSION == 2
  // Python's getreadbuf interface function.
  static Py_ssize_t getreadbuf(PyObject *self, Py_ssize_t segment, void **ptrptr)
  {
    if (segment != 0) {
      // Always one-segment.
      PyErr_SetString(PyExc_ValueError, "accessed non-existent segment");
      return -1;
    }
    GfMatrix3d &mat = extract<GfMatrix3d &>(self);
    *ptrptr = static_cast<void *>(mat.GetArray());
    // Return size in bytes.
    return sizeof(GfMatrix3d);
  }

  // Python's getwritebuf interface function.
  static Py_ssize_t getwritebuf(PyObject *self, Py_ssize_t segment, void **ptrptr)
  {
    PyErr_SetString(PyExc_ValueError,
                    "writable buffers supported only with "
                    "new-style buffer protocol.");
    return -1;
  }

  // Python's getsegcount interface function.
  static Py_ssize_t getsegcount(PyObject *self, Py_ssize_t *lenp)
  {
    if (lenp)
      *lenp = sizeof(GfMatrix3d);
    return 1;  // Always one contiguous segment.
  }

  // Python's getcharbuf interface function.
  static Py_ssize_t getcharbuf(PyObject *self, Py_ssize_t segment, const char **ptrptr)
  {
    PyErr_SetString(PyExc_ValueError, "cannot treat binary data as text");
    return -1;
  }
#endif

  // Python's getbuffer interface function.
  static int getbuffer(PyObject *self, Py_buffer *view, int flags)
  {
    if (view == NULL) {
      PyErr_SetString(PyExc_ValueError, "NULL view in getbuffer");
      return -1;
    }

    // We don't support fortran order.
    if ((flags & PyBUF_F_CONTIGUOUS) == PyBUF_F_CONTIGUOUS) {
      PyErr_SetString(PyExc_ValueError, "Fortran contiguity unsupported");
      return -1;
    }

    GfMatrix3d &mat = extract<GfMatrix3d &>(self);

    view->obj = self;
    view->buf = static_cast<void *>(mat.GetArray());
    view->len = sizeof(GfMatrix3d);
    view->readonly = 0;
    view->itemsize = sizeof(double);
    if ((flags & PyBUF_FORMAT) == PyBUF_FORMAT) {
      view->format = Gf_GetPyBufferFmtFor<double>();
    } else {
      view->format = NULL;
    }
    if ((flags & PyBUF_ND) == PyBUF_ND) {
      view->ndim = 2;
      static Py_ssize_t shape[] = {3, 3};
      view->shape = shape;
    } else {
      view->ndim = 0;
      view->shape = NULL;
    }
    if ((flags & PyBUF_STRIDES) == PyBUF_STRIDES) {
      static Py_ssize_t strides[] = {3 * sizeof(double), sizeof(double)};
      view->strides = strides;
    } else {
      view->strides = NULL;
    }
    view->suboffsets = NULL;
    view->internal = NULL;

    Py_INCREF(self);  // need to retain a reference to self.
    return 0;
  }

  // This structure serves to instantiate a PyBufferProcs instance with pointers
  // to the right buffer protocol functions.
  static PyBufferProcs bufferProcs = {
#if PY_MAJOR_VERSION == 2
    (readbufferproc)getreadbuf,   /*bf_getreadbuffer*/
    (writebufferproc)getwritebuf, /*bf_getwritebuffer*/
    (segcountproc)getsegcount,    /*bf_getsegcount*/
    (charbufferproc)getcharbuf,   /*bf_getcharbuffer*/
#endif
    (getbufferproc)getbuffer,
    (releasebufferproc)0,
  };

  // End python buffer protocol support.
  ////////////////////////////////////////////////////////////////////////

  static string _Repr(GfMatrix3d const &self)
  {
    static char newline[] = ",\n            ";
    return TF_PY_REPR_PREFIX + "Matrix3d(" + TfPyRepr(self[0][0]) + ", " + TfPyRepr(self[0][1]) +
           ", " + TfPyRepr(self[0][2]) + newline + TfPyRepr(self[1][0]) + ", " +
           TfPyRepr(self[1][1]) + ", " + TfPyRepr(self[1][2]) + newline + TfPyRepr(self[2][0]) +
           ", " + TfPyRepr(self[2][1]) + ", " + TfPyRepr(self[2][2]) + ")";
  }

  static GfMatrix3d GetInverseWrapper(const GfMatrix3d &self)
  {
    return self.GetInverse();
  }

  static void throwIndexErr(const char *msg)
  {
    PyErr_SetString(PyExc_IndexError, msg);
    boost::python::throw_error_already_set();
  }

  static int normalizeIndex(int index)
  {
    return TfPyNormalizeIndex(index, 3, true /*throw error*/);
  }

  // Return number of rows
  static int __len__(GfMatrix3d const &self)
  {
    return 3;
  }

  static double __getitem__double(GfMatrix3d const &self, tuple index)
  {
    int i1 = 0, i2 = 0;
    if (len(index) == 2) {
      i1 = normalizeIndex(extract<int>(index[0]));
      i2 = normalizeIndex(extract<int>(index[1]));
    } else
      throwIndexErr("Index has incorrect size.");

    return self[i1][i2];
  }

  static GfVec3d __getitem__vector(GfMatrix3d const &self, int index)
  {
    return GfVec3d(self[normalizeIndex(index)]);
  }

  static void __setitem__double(GfMatrix3d &self, tuple index, double value)
  {
    int i1 = 0, i2 = 0;
    if (len(index) == 2) {
      i1 = normalizeIndex(extract<int>(index[0]));
      i2 = normalizeIndex(extract<int>(index[1]));
    } else
      throwIndexErr("Index has incorrect size.");

    self[i1][i2] = value;
  }

  static void __setitem__vector(GfMatrix3d &self, int index, GfVec3d value)
  {
    int ni = normalizeIndex(index);
    self[ni][0] = value[0];
    self[ni][1] = value[1];
    self[ni][2] = value[2];
  }

  static bool __contains__double(const GfMatrix3d &self, double value)
  {
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        if (self[i][j] == value)
          return true;
    return false;
  }

  // Check rows against GfVec
  static bool __contains__vector(const GfMatrix3d &self, GfVec3d value)
  {
    for (int i = 0; i < 3; ++i)
      if (self.GetRow(i) == value)
        return true;
    return false;
  }

#if PY_MAJOR_VERSION == 2
  static GfMatrix3d __truediv__(const GfMatrix3d &self, GfMatrix3d value)
  {
    return self / value;
  }
#endif

  static GfMatrix3d *__init__()
  {
    // Default constructor produces identity from python.
    return new GfMatrix3d(1);
  }

  // This adds support for python's builtin pickling library
  // This is used by our Shake plugins which need to pickle entire classes
  // (including code), which we don't support in pxml.
  struct GfMatrix3d_Pickle_Suite : boost::python::pickle_suite
  {
    static boost::python::tuple getinitargs(const GfMatrix3d &m)
    {
      return boost::python::make_tuple(m[0][0],
                                       m[0][1],
                                       m[0][2],
                                       m[1][0],
                                       m[1][1],
                                       m[1][2],
                                       m[2][0],
                                       m[2][1],
                                       m[2][2]);
    }
  };

  static size_t __hash__(GfMatrix3d const &m)
  {
    return hash_value(m);
  }

  static boost::python::tuple get_dimension()
  {
    // At one time this was a constant static tuple we returned for
    // dimension. With boost building for python 3 that results in
    // a segfault at shutdown. Building for python 2 with a static
    // tuple returned here seems to work fine.
    //
    // It seems likely that this has to do with the order of
    // destruction of these objects when deinitializing, but we did
    // not dig deeply into this difference.
    return make_tuple(3, 3);
  }

}  // anonymous namespace

void wrapMatrix3d()
{
  typedef GfMatrix3d This;

  def("IsClose", (bool (*)(const GfMatrix3d &m1, const GfMatrix3d &m2, double))GfIsClose);

  class_<This> cls("Matrix3d", no_init);
  cls.def_pickle(GfMatrix3d_Pickle_Suite())
    .def("__init__", make_constructor(__init__))
    .def(init<const GfMatrix3d &>())
    .def(init<const GfMatrix3f &>())
    .def(init<int>())
    .def(init<double>())
    .def(init<double, double, double, double, double, double, double, double, double>())
    .def(init<const GfVec3d &>())
    .def(init<const vector<vector<float>> &>())
    .def(init<const vector<vector<double>> &>())
    .def(init<const GfRotation &>())
    .def(init<const GfQuatd &>())

    .def(TfTypePythonClass())

    .add_static_property("dimension", get_dimension)
    .def("__len__", __len__, "Return number of rows")

    .def("__getitem__", __getitem__double)
    .def("__getitem__", __getitem__vector)
    .def("__setitem__", __setitem__double)
    .def("__setitem__", __setitem__vector)
    .def("__contains__", __contains__double)
    .def("__contains__", __contains__vector, "Check rows against GfVec")

    .def(
      "Set",
      (This & (This::*)(double, double, double, double, double, double, double, double, double)) &
        This::Set,
      return_self<>())

    .def("SetIdentity", &This::SetIdentity, return_self<>())
    .def("SetZero", &This::SetZero, return_self<>())

    .def("SetDiagonal", (This & (This::*)(double)) & This::SetDiagonal, return_self<>())
    .def("SetDiagonal", (This & (This::*)(const GfVec3d &)) & This::SetDiagonal, return_self<>())

    .def("SetRow", &This::SetRow)
    .def("SetColumn", &This::SetColumn)
    .def("GetRow", &This::GetRow)
    .def("GetColumn", &This::GetColumn)

    .def("GetTranspose", &This::GetTranspose)
    .def("GetInverse", GetInverseWrapper)

    .def("GetDeterminant", &This::GetDeterminant)
    .def("GetHandedness", &This::GetHandedness)
    .def("IsLeftHanded", &This::IsLeftHanded)
    .def("IsRightHanded", &This::IsRightHanded)

    .def("Orthonormalize", &This::Orthonormalize, (arg("issueWarning") = true))
    .def("GetOrthonormalized", &This::GetOrthonormalized, (arg("issueWarning") = true))

    .def(str(self))
    .def(self == self)
    .def(self == GfMatrix3f())
    .def(self != self)
    .def(self != GfMatrix3f())
    .def(self *= self)
    .def(self * self)
    .def(self *= double())
    .def(self * double())
    .def(double() * self)
    .def(self += self)
    .def(self + self)
    .def(self -= self)
    .def(self - self)
    .def(-self)
    .def(self / self)
    .def(self * GfVec3d())
    .def(GfVec3d() * self)
    .def(self * GfVec3f())
    .def(GfVec3f() * self)

#if PY_MAJOR_VERSION == 2
    // Needed only to support "from __future__ import division" in
    // python 2. In python 3 builds boost::python adds this for us.
    .def("__truediv__", __truediv__)
#endif

    .def("SetScale", (This & (This::*)(const GfVec3d &)) & This::SetScale, return_self<>())
    .def("SetRotate", (This & (This::*)(const GfQuatd &)) & This::SetRotate, return_self<>())
    .def("SetRotate", (This & (This::*)(const GfRotation &)) & This::SetRotate, return_self<>())
    .def("ExtractRotation", &This::ExtractRotation)
    .def("SetScale", (This & (This::*)(double)) & This::SetScale, return_self<>())

    .def("__repr__", _Repr)
    .def("__hash__", __hash__)

    ;
  to_python_converter<std::vector<This>, TfPySequenceToPython<std::vector<This>>>();

  // Install buffer protocol: set the tp_as_buffer slot to point to a
  // structure of function pointers that implement the buffer protocol for
  // this type, and set the type flags to indicate that this type supports the
  // buffer protocol.
  auto *typeObj = reinterpret_cast<PyTypeObject *>(cls.ptr());
  typeObj->tp_as_buffer = &bufferProcs;
  typeObj->tp_flags |= (TfPy_TPFLAGS_HAVE_NEWBUFFER | TfPy_TPFLAGS_HAVE_GETCHARBUFFER);
}
