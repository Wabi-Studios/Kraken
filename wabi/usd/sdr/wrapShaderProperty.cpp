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

#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyStaticTokens.h"
#include "wabi/base/tf/weakPtr.h"
#include "wabi/usd/sdr/shaderProperty.h"
#include "wabi/wabi.h"

#include <boost/python.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

struct TfTokenPairToPythonConverter {
  static PyObject *convert(const std::pair<TfToken, TfToken> &pair)
  {
    return incref(make_tuple(pair.first, pair.second).ptr());
  }
};

// Boost treats a const ptr differently than a non-const ptr, so a custom
// converter is needed to deal with the const-ness
struct SdrShaderPropertyConstPtrToPythonConverter {
  static PyObject *convert(SdrShaderPropertyConstPtr shaderProperty)
  {
    object shaderPropertyObject(ptr(shaderProperty));

    return incref(shaderPropertyObject.ptr());
  }
};

void wrapShaderProperty()
{
  typedef SdrShaderProperty This;
  typedef SdrShaderPropertyPtr ThisPtr;

  TF_PY_WRAP_PUBLIC_TOKENS("PropertyTypes", SdrPropertyTypes, SDR_PROPERTY_TYPE_TOKENS);

  TF_PY_WRAP_PUBLIC_TOKENS("PropertyMetadata", SdrPropertyMetadata, SDR_PROPERTY_METADATA_TOKENS);

  TF_PY_WRAP_PUBLIC_TOKENS("PropertyRole", SdrPropertyRole, SDR_PROPERTY_ROLE_TOKENS);

  to_python_converter<NdrOption, TfTokenPairToPythonConverter>();
  return_value_policy<copy_const_reference> copyRefPolicy;

  to_python_converter<SdrShaderPropertyConstPtr, SdrShaderPropertyConstPtrToPythonConverter>();

  class_<This, ThisPtr, bases<NdrProperty>, boost::noncopyable>("ShaderProperty", no_init)
      .def("GetLabel", &This::GetLabel, copyRefPolicy)
      .def("GetHelp", &This::GetHelp)
      .def("GetPage", &This::GetPage, copyRefPolicy)
      .def("GetWidget", &This::GetWidget, copyRefPolicy)
      .def("GetHints", &This::GetHints, return_value_policy<TfPyMapToDictionary>())
      .def("GetOptions", &This::GetOptions, return_value_policy<TfPySequenceToList>())
      .def("GetImplementationName", &This::GetImplementationName)
      .def("GetVStructMemberOf", &This::GetVStructMemberOf, copyRefPolicy)
      .def("GetVStructMemberName", &This::GetVStructMemberName, copyRefPolicy)
      .def("GetVStructConditionalExpr", &This::GetVStructConditionalExpr, copyRefPolicy)
      .def("IsVStructMember", &This::IsVStructMember)
      .def("IsVStruct", &This::IsVStruct)
      .def("GetValidConnectionTypes", &This::GetValidConnectionTypes, copyRefPolicy)
      .def("IsAssetIdentifier", &This::IsAssetIdentifier)
      .def("IsDefaultInput", &This::IsDefaultInput);
}
