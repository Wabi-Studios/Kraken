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
#include <boost/python/object.hpp>
#include <boost/python/to_python_converter.hpp>

#include "wabi/usd/ar/pyResolverContext.h"
#include "wabi/usd/ar/resolverContext.h"
#include "wabi/wabi.h"

using namespace boost::python;

WABI_NAMESPACE_USING

namespace {

struct Ar_ResolverContextToPython {
  Ar_ResolverContextToPython()
  {
    to_python_converter<ArResolverContext, Ar_ResolverContextToPython>();
  }

  static PyObject *convert(const ArResolverContext &context)
  {
    return incref(Ar_ConvertResolverContextToPython(context).ptr());
  }
};

struct Ar_ResolverContextFromPython {
  Ar_ResolverContextFromPython()
  {
    converter::registry::push_back(
        &_convertible, &_construct, boost::python::type_id<ArResolverContext>());
  }

  static void *_convertible(PyObject *obj_ptr)
  {
    if (obj_ptr == Py_None || Ar_CanConvertResolverContextFromPython(obj_ptr)) {
      return obj_ptr;
    }
    return 0;
  }

  static void _construct(PyObject *obj_ptr, converter::rvalue_from_python_stage1_data *data)
  {
    void *storage =
        ((converter::rvalue_from_python_storage<ArResolverContext> *)data)->storage.bytes;

    ArResolverContext context;
    if (obj_ptr != Py_None) {
      context = Ar_ConvertResolverContextFromPython(obj_ptr);
    }

    new (storage) ArResolverContext(context);
    data->convertible = storage;
  }
};

}  // anonymous namespace

void wrapResolverContext()
{
  Ar_ResolverContextToPython();
  Ar_ResolverContextFromPython();
}
