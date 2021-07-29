//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "wabi/usd/usdPhysics/prismaticJoint.h"
#include "wabi/usd/usd/schemaBase.h"

#include "wabi/usd/sdf/primSpec.h"

#include "wabi/usd/usd/pyConversions.h"
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

#define WRAP_CUSTOM   \
  template<class Cls> \
  static void _CustomWrapCode(Cls &_class)

  // fwd decl.
  WRAP_CUSTOM;


  static UsdAttribute _CreateAxisAttr(UsdPhysicsPrismaticJoint &self, object defaultVal, bool writeSparsely)
  {
    return self.CreateAxisAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Token), writeSparsely);
  }

  static UsdAttribute _CreateLowerLimitAttr(UsdPhysicsPrismaticJoint &self,
                                            object defaultVal,
                                            bool writeSparsely)
  {
    return self.CreateLowerLimitAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float),
                                     writeSparsely);
  }

  static UsdAttribute _CreateUpperLimitAttr(UsdPhysicsPrismaticJoint &self,
                                            object defaultVal,
                                            bool writeSparsely)
  {
    return self.CreateUpperLimitAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float),
                                     writeSparsely);
  }

  static std::string _Repr(const UsdPhysicsPrismaticJoint &self)
  {
    std::string primRepr = TfPyRepr(self.GetPrim());
    return TfStringPrintf("UsdPhysics.PrismaticJoint(%s)", primRepr.c_str());
  }

}  // anonymous namespace

void wrapUsdPhysicsPrismaticJoint()
{
  typedef UsdPhysicsPrismaticJoint This;

  class_<This, bases<UsdPhysicsJoint>> cls("PrismaticJoint");

  cls.def(init<UsdPrim>(arg("prim")))
    .def(init<UsdSchemaBase const &>(arg("schemaObj")))
    .def(TfTypePythonClass())

    .def("Get", &This::Get, (arg("stage"), arg("path")))
    .staticmethod("Get")

    .def("Define", &This::Define, (arg("stage"), arg("path")))
    .staticmethod("Define")

    .def("GetSchemaAttributeNames",
         &This::GetSchemaAttributeNames,
         arg("includeInherited") = true,
         return_value_policy<TfPySequenceToList>())
    .staticmethod("GetSchemaAttributeNames")

    .def("GetStaticTfType", (TfType const &(*)())TfType::Find<This>, return_value_policy<return_by_value>())
    .staticmethod("GetStaticTfType")

    .def(!self)


    .def("GetAxisAttr", &This::GetAxisAttr)
    .def("CreateAxisAttr", &_CreateAxisAttr, (arg("defaultValue") = object(), arg("writeSparsely") = false))

    .def("GetLowerLimitAttr", &This::GetLowerLimitAttr)
    .def("CreateLowerLimitAttr",
         &_CreateLowerLimitAttr,
         (arg("defaultValue") = object(), arg("writeSparsely") = false))

    .def("GetUpperLimitAttr", &This::GetUpperLimitAttr)
    .def("CreateUpperLimitAttr",
         &_CreateUpperLimitAttr,
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

  WRAP_CUSTOM
  {}

}  // namespace