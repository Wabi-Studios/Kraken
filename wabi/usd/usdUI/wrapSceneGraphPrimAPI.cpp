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
#include "wabi/usd/usdUI/sceneGraphPrimAPI.h"
#include "wabi/usd/usd/schemaBase.h"

#include "wabi/usd/sdf/primSpec.h"

#include "wabi/usd/usd/pyConversions.h"
#include "wabi/base/tf/pyAnnotatedBoolResult.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include <boost/python.hpp>

#include <string>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace
{

#define WRAP_CUSTOM template<class Cls> static void _CustomWrapCode(Cls &_class)

  // fwd decl.
  WRAP_CUSTOM;


  static UsdAttribute _CreateDisplayNameAttr(UsdUISceneGraphPrimAPI &self,
                                             object defaultVal,
                                             bool writeSparsely)
  {
    return self.CreateDisplayNameAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Token),
                                      writeSparsely);
  }

  static UsdAttribute _CreateDisplayGroupAttr(UsdUISceneGraphPrimAPI &self,
                                              object defaultVal,
                                              bool writeSparsely)
  {
    return self.CreateDisplayGroupAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Token),
                                       writeSparsely);
  }

  static std::string _Repr(const UsdUISceneGraphPrimAPI &self)
  {
    std::string primRepr = TfPyRepr(self.GetPrim());
    return TfStringPrintf("UsdUI.SceneGraphPrimAPI(%s)", primRepr.c_str());
  }

  struct UsdUISceneGraphPrimAPI_CanApplyResult : public TfPyAnnotatedBoolResult<std::string>
  {
    UsdUISceneGraphPrimAPI_CanApplyResult(bool val, std::string const &msg)
      : TfPyAnnotatedBoolResult<std::string>(val, msg)
    {}
  };

  static UsdUISceneGraphPrimAPI_CanApplyResult _WrapCanApply(const UsdPrim &prim)
  {
    std::string whyNot;
    bool result = UsdUISceneGraphPrimAPI::CanApply(prim, &whyNot);
    return UsdUISceneGraphPrimAPI_CanApplyResult(result, whyNot);
  }

}  // anonymous namespace

void wrapUsdUISceneGraphPrimAPI()
{
  typedef UsdUISceneGraphPrimAPI This;

  UsdUISceneGraphPrimAPI_CanApplyResult::Wrap<UsdUISceneGraphPrimAPI_CanApplyResult>(
    "_CanApplyResult",
    "whyNot");

  class_<This, bases<UsdAPISchemaBase>> cls("SceneGraphPrimAPI");

  cls.def(init<UsdPrim>(arg("prim")))
    .def(init<UsdSchemaBase const &>(arg("schemaObj")))
    .def(TfTypePythonClass())

    .def("Get", &This::Get, (arg("stage"), arg("path")))
    .staticmethod("Get")

    .def("CanApply", &_WrapCanApply, (arg("prim")))
    .staticmethod("CanApply")

    .def("Apply", &This::Apply, (arg("prim")))
    .staticmethod("Apply")

    .def("GetSchemaAttributeNames",
         &This::GetSchemaAttributeNames,
         arg("includeInherited") = true,
         return_value_policy<TfPySequenceToList>())
    .staticmethod("GetSchemaAttributeNames")

    .def("_GetStaticTfType",
         (TfType const &(*)())TfType::Find<This>,
         return_value_policy<return_by_value>())
    .staticmethod("_GetStaticTfType")

    .def(!self)


    .def("GetDisplayNameAttr", &This::GetDisplayNameAttr)
    .def("CreateDisplayNameAttr",
         &_CreateDisplayNameAttr,
         (arg("defaultValue") = object(), arg("writeSparsely") = false))

    .def("GetDisplayGroupAttr", &This::GetDisplayGroupAttr)
    .def("CreateDisplayGroupAttr",
         &_CreateDisplayGroupAttr,
         (arg("defaultValue") = object(), arg("writeSparsely") = false))

    .def("__repr__", ::_Repr);

  _CustomWrapCode(cls);
}

// ===================================================================== //
// Feel free to add custom code below this line, it will be preserved by
// the code generator.  The entry point for your custom code should look
// minimally like the following:
//
// WRAP_CUSTOM {
//     _class
//         .def("MyCustomMethod", ...)
//     ;
// }
//
// Of course any other ancillary or support code may be provided.
//
// Just remember to wrap code in the appropriate delimiters:
// 'namespace {', '}'.
//
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--

namespace
{

  WRAP_CUSTOM {}

}  // namespace
