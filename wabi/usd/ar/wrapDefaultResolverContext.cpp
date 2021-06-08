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
#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>

#include "wabi/base/tf/pyUtils.h"
#include "wabi/usd/ar/defaultResolverContext.h"
#include "wabi/usd/ar/pyResolverContext.h"
#include "wabi/wabi.h"

using namespace boost::python;

WABI_NAMESPACE_USING

static std::string _Repr(const ArDefaultResolverContext &ctx)
{
  std::string repr = TF_PY_REPR_PREFIX;
  repr += "DefaultResolverContext(";
  if (!ctx.GetSearchPath().empty()) {
    repr += TfPyRepr(ctx.GetSearchPath());
  }
  repr += ")";
  return repr;
}

static size_t _Hash(const ArDefaultResolverContext &ctx)
{
  return hash_value(ctx);
}

void wrapDefaultResolverContext()
{
  using This = ArDefaultResolverContext;

  class_<This>("DefaultResolverContext", no_init)
      .def(init<>())
      .def(init<const std::vector<std::string> &>(arg("searchPaths")))

      .def(self == self)
      .def(self != self)

      .def("GetSearchPath", &This::GetSearchPath, return_value_policy<return_by_value>())

      .def("__str__", &This::GetAsString)
      .def("__repr__", &_Repr)
      .def("__hash__", &_Hash);

  ArWrapResolverContextForPython<This>();
}
