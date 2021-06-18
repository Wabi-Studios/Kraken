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
// wrapVec.template.cpp file to make changes.

#include "wabi/base/gf/vec2i.h"
#include "wabi/wabi.h"

#include "wabi/base/gf/pyBufferUtils.h"

#include "wabi/base/tf/py3Compat.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/stringUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/slice.hpp>
#include <boost/python/tuple.hpp>

#include <boost/functional/hash.hpp>

#include <string>

using namespace boost::python;

using std::string;

WABI_NAMESPACE_USING

namespace
{

////////////////////////////////////////////////////////////////////////
// Python buffer protocol support.

#if PY_MAJOR_VERSION == 2
// Python's getreadbuf interface function.
static Py_ssize_t getreadbuf(PyObject *self, Py_ssize_t segment, void **ptrptr)
{
  if (segment != 0)
  {
    // Always one-segment.
    PyErr_SetString(PyExc_ValueError, "accessed non-existent segment");
    return -1;
  }
  GfVec2i &vec = extract<GfVec2i &>(self);
  *ptrptr = static_cast<void *>(vec.data());
  // Return size in bytes.
  return sizeof(GfVec2i);
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
    *lenp = sizeof(GfVec2i);
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
  if (view == NULL)
  {
    PyErr_SetString(PyExc_ValueError, "NULL view in getbuffer");
    return -1;
  }

  // We don't support fortran order.
  if ((flags & PyBUF_F_CONTIGUOUS) == PyBUF_F_CONTIGUOUS)
  {
    PyErr_SetString(PyExc_ValueError, "Fortran contiguity unsupported");
    return -1;
  }

  GfVec2i &vec = extract<GfVec2i &>(self);

  view->obj = self;
  view->buf = static_cast<void *>(vec.data());
  view->len = sizeof(GfVec2i);
  view->readonly = 0;
  view->itemsize = sizeof(int);
  if ((flags & PyBUF_FORMAT) == PyBUF_FORMAT)
  {
    view->format = Gf_GetPyBufferFmtFor<int>();
  }
  else
  {
    view->format = NULL;
  }
  if ((flags & PyBUF_ND) == PyBUF_ND)
  {
    view->ndim = 1;
    static Py_ssize_t shape = 2;
    view->shape = &shape;
  }
  else
  {
    view->ndim = 0;
    view->shape = NULL;
  }
  if ((flags & PyBUF_STRIDES) == PyBUF_STRIDES)
  {
    static Py_ssize_t strides = sizeof(int);
    view->strides = &strides;
  }
  else
  {
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

static string __repr__(GfVec2i const &self)
{
  string elems;
  for (size_t i = 0; i < self.dimension; ++i)
    elems += (i ? ", " : "") + TfPyRepr(self[i]);

  return TF_PY_REPR_PREFIX + "Vec2i(" + elems + ")";
}

static size_t __hash__(GfVec2i const &self)
{
  return boost::hash<GfVec2i>()(self);
}

static int normalizeIndex(int index)
{
  return TfPyNormalizeIndex(index, 2, true /*throw error*/);
}

static int __len__(const GfVec2i &self)
{
  return 2;
}

// Implements __getitem__ for a single index
static int __getitem__(const GfVec2i &self, int index)
{
  return self[normalizeIndex(index)];
}

// Implements __getitem__ for a slice
static list __getslice__(const GfVec2i &self, slice indices)
{
  list result;

  const int *begin = self.data();
  const int *end = begin + 2;

  slice::range<const int *> bounds;
  try
  {
    // This appears to be a typo in the boost headers.  The method
    // name should be "get_indices".
    //
    bounds = indices.get_indicies<>(begin, end);
  }
  catch (std::invalid_argument &)
  {
    return result;
  }

  while (bounds.start != bounds.stop)
  {
    result.append(*bounds.start);
    bounds.start += bounds.step;
  }
  // Unlike STL ranges, bounds represents a *closed* interval.  This
  // means that we must append exactly one more item at the end of
  // the list.
  //
  result.append(*bounds.start);

  return result;
}

static void __setitem__(GfVec2i &self, int index, int value)
{
  self[normalizeIndex(index)] = value;
}

// Handles refcounting & extraction for PySequence_GetItem.
static int _SequenceGetItem(PyObject *seq, Py_ssize_t i)
{
  boost::python::handle<> h(PySequence_GetItem(seq, i));
  return extract<int>(boost::python::object(h));
}

static bool _SequenceCheckItem(PyObject *seq, Py_ssize_t i)
{
  boost::python::handle<> h(PySequence_GetItem(seq, i));
  extract<int> e((boost::python::object(h)));
  return e.check();
}

static void __setslice__(GfVec2i &self, slice indices, object values)
{
  // Verify our arguments
  //
  PyObject *valuesObj = values.ptr();

  if (!PySequence_Check(valuesObj))
  {
    TfPyThrowTypeError("value must be a sequence");
  }

  int *begin = self.data();
  int *end = begin + 2;

  Py_ssize_t sliceLength = -1;

  slice::range<int *> bounds;

  // Convince g++ that we're not using uninitialized values.
  //
  bounds.start = 0;
  bounds.stop = 0;
  bounds.step = 0;

  try
  {
    // This appears to be a typo in the boost headers.  The method
    // name should be "get_indices".
    //
    bounds = indices.get_indicies<>(begin, end);
  }
  catch (std::invalid_argument &)
  {
    sliceLength = 0;
  }

  // If sliceLength was not set in the exception handling code above,
  // figure out how long it really is.
  //
  if (sliceLength == -1)
  {
    sliceLength = ((bounds.stop - bounds.start) / bounds.step) + 1;
  }

  if (PySequence_Length(valuesObj) != sliceLength)
  {
    TfPyThrowValueError(TfStringPrintf("attempt to assign sequence of size %zd to slice of size %zd",
                                       PySequence_Length(valuesObj),
                                       sliceLength));
  }

  // Short circuit for empty slices
  //
  if (sliceLength == 0)
  {
    return;
  }

  // Make sure that all items can be extracted before changing the GfVec2i.
  //
  for (Py_ssize_t i = 0; i < sliceLength; ++i)
  {
    // This will throw a TypeError if any of the items cannot be converted.
    _SequenceGetItem(valuesObj, i);
  }

  for (Py_ssize_t i = 0; i < sliceLength; ++i)
  {
    *bounds.start = _SequenceGetItem(valuesObj, i);
    bounds.start += bounds.step;
  }
}

static bool __contains__(const GfVec2i &self, int value)
{
  for (size_t i = 0; i < 2; ++i)
  {
    if (self[i] == value)
      return true;
  }
  return false;
}

#if PY_MAJOR_VERSION == 2
static GfVec2i __truediv__(const GfVec2i &self, int value)
{
  return self / value;
}

static GfVec2i __itruediv__(GfVec2i &self, int value)
{
  return self /= value;
}
#endif

template<class V>
static V *__init__()
{
  // Default contstructor zero-initializes from python.
  return new V(0);
}

struct FromPythonTuple
{
  FromPythonTuple()
  {
    converter::registry::push_back(&_convertible, &_construct, boost::python::type_id<GfVec2i>());
  }

 private:
  static void *_convertible(PyObject *obj_ptr)
  {
    // If this object is a GfVec already, disregard.
    if (PyObject_HasAttrString(obj_ptr, "__isGfVec"))
      return 0;

    typedef int Scalar;

    // XXX: Would like to allow general sequences, but currently clients
    // depend on this behavior.
    if ((PyTuple_Check(obj_ptr) || PyList_Check(obj_ptr)) && PySequence_Size(obj_ptr) == 2 &&
        _SequenceCheckItem(obj_ptr, 0) && _SequenceCheckItem(obj_ptr, 1))
    {
      return obj_ptr;
    }
    return 0;
  }

  static void _construct(PyObject *obj_ptr, converter::rvalue_from_python_stage1_data *data)
  {
    typedef int Scalar;
    void *storage = ((converter::rvalue_from_python_storage<GfVec2i> *)data)->storage.bytes;
    new (storage) GfVec2i(_SequenceGetItem(obj_ptr, 0), _SequenceGetItem(obj_ptr, 1));
    data->convertible = storage;
  }
};

// This adds support for python's builtin pickling library
// This is used by our Shake plugins which need to pickle entire classes
// (including code), which we don't support in pxml.
struct PickleSuite : boost::python::pickle_suite
{
  static boost::python::tuple getinitargs(const GfVec2i &v)
  {
    return boost::python::make_tuple(v[0], v[1]);
  }
};

}  // anonymous namespace

void wrapVec2i()
{
  typedef GfVec2i Vec;
  typedef int Scalar;

  static const size_t _dimension = 2;
  static const bool _true = true;

  def("Dot", (Scalar(*)(const Vec &, const Vec &))GfDot);

  class_<GfVec2i> cls("Vec2i", no_init);
  cls
    .def("__init__", make_constructor(__init__<Vec>))

    // A tag indicating that this is a GfVec class, for internal use.
    .def_readonly("__isGfVec", _true)

    .def_pickle(PickleSuite())

    .def(init<Vec>())
    .def(init<Scalar>())
    .def(init<Scalar, Scalar>())

    .def(TfTypePythonClass())

    .def("__len__", __len__)
    .def("__getitem__", __getitem__)
    .def("__getitem__", __getslice__)
    .def("__setitem__", __setitem__)
    .def("__setitem__", __setslice__)
    .def("__contains__", __contains__)

    .def_readonly("dimension", _dimension)

    .def(self == self)
    .def(self != self)
    .def(self += self)
    .def(self -= self)
    .def(self *= double())
    .def(self * double())
    .def(double() * self)
    .def(self /= int())
    .def(self / int())
    .def(-self)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(str(self))

#if PY_MAJOR_VERSION == 2
    // Needed only to support "from __future__ import division" in
    // python 2. In python 3 builds boost::python adds this for us.
    .def("__truediv__", __truediv__)
    .def("__itruediv__", __itruediv__)
#endif

    .def("Axis", &Vec::Axis)
    .staticmethod("Axis")

    .def("XAxis", &Vec::XAxis)
    .staticmethod("XAxis")
    .def("YAxis", &Vec::YAxis)
    .staticmethod("YAxis")

    .def("GetDot", (Scalar(*)(const Vec &, const Vec &))GfDot)

    .def("__repr__", __repr__)
    .def("__hash__", __hash__);
  to_python_converter<std::vector<GfVec2i>, TfPySequenceToPython<std::vector<GfVec2i>>>();

  // Install buffer protocol: set the tp_as_buffer slot to point to a
  // structure of function pointers that implement the buffer protocol for
  // this type, and set the type flags to indicate that this type supports the
  // buffer protocol.
  auto *typeObj = reinterpret_cast<PyTypeObject *>(cls.ptr());
  typeObj->tp_as_buffer = &bufferProcs;
  typeObj->tp_flags |= (TfPy_TPFLAGS_HAVE_NEWBUFFER | TfPy_TPFLAGS_HAVE_GETCHARBUFFER);

  // Allow appropriate tuples to be passed where Vecs are expected.
  FromPythonTuple();

  // Allow conversion of lists of GfVec2i to std::vector<GfVec2i>
  TfPyContainerConversions::from_python_sequence<std::vector<GfVec2i>,
                                                 TfPyContainerConversions::variable_capacity_policy>();
}
