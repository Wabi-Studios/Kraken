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

#include "wabi/base/tf/declarePtrs.h"
#include "wabi/base/tf/pyPtrHelpers.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/weakPtr.h"
#include "wabi/usd/ndr/node.h"
#include "wabi/wabi.h"

#include <boost/python.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

void wrapNode()
{
  typedef NdrNode This;
  typedef NdrNodePtr ThisPtr;

  return_value_policy<copy_const_reference> copyRefPolicy;

  class_<This, ThisPtr, boost::noncopyable>("Node", no_init)
    .def("__repr__", &This::GetInfoString)
    .def(TfPyBoolBuiltinFuncName, &This::IsValid)
    .def("GetIdentifier", &This::GetIdentifier, copyRefPolicy)
    .def("GetVersion", &This::GetVersion)
    .def("GetName", &This::GetName, copyRefPolicy)
    .def("GetFamily", &This::GetFamily, copyRefPolicy)
    .def("GetContext", &This::GetContext, copyRefPolicy)
    .def("GetSourceType", &This::GetSourceType, copyRefPolicy)
    .def("GetResolvedDefinitionURI", &This::GetResolvedDefinitionURI, copyRefPolicy)
    .def("GetResolvedImplementationURI", &This::GetResolvedImplementationURI, copyRefPolicy)
    .def("IsValid", &This::IsValid)
    .def("GetInfoString", &This::GetInfoString)
    .def("GetInput", &This::GetInput, return_internal_reference<>())
    .def("GetInputNames", &This::GetInputNames, copyRefPolicy)
    .def("GetOutput", &This::GetOutput, return_internal_reference<>())
    .def("GetOutputNames", &This::GetOutputNames, copyRefPolicy)
    .def("GetSourceCode", &This::GetSourceCode, copyRefPolicy)
    .def("GetMetadata", &This::GetMetadata, return_value_policy<TfPyMapToDictionary>());
}
