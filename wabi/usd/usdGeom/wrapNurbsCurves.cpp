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
#include "wabi/usd/usdGeom/nurbsCurves.h"
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

#define WRAP_CUSTOM template<class Cls> static void _CustomWrapCode(Cls &_class)

  // fwd decl.
  WRAP_CUSTOM;


  static UsdAttribute _CreateOrderAttr(UsdGeomNurbsCurves &self,
                                       object defaultVal,
                                       bool writeSparsely)
  {
    return self.CreateOrderAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->IntArray),
                                writeSparsely);
  }

  static UsdAttribute _CreateKnotsAttr(UsdGeomNurbsCurves &self,
                                       object defaultVal,
                                       bool writeSparsely)
  {
    return self.CreateKnotsAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->DoubleArray),
                                writeSparsely);
  }

  static UsdAttribute _CreateRangesAttr(UsdGeomNurbsCurves &self,
                                        object defaultVal,
                                        bool writeSparsely)
  {
    return self.CreateRangesAttr(UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Double2Array),
                                 writeSparsely);
  }

  static std::string _Repr(const UsdGeomNurbsCurves &self)
  {
    std::string primRepr = TfPyRepr(self.GetPrim());
    return TfStringPrintf("UsdGeom.NurbsCurves(%s)", primRepr.c_str());
  }

}  // anonymous namespace

void wrapUsdGeomNurbsCurves()
{
  typedef UsdGeomNurbsCurves This;

  class_<This, bases<UsdGeomCurves>> cls("NurbsCurves");

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

    .def("_GetStaticTfType",
         (TfType const &(*)())TfType::Find<This>,
         return_value_policy<return_by_value>())
    .staticmethod("_GetStaticTfType")

    .def(!self)


    .def("GetOrderAttr", &This::GetOrderAttr)
    .def("CreateOrderAttr",
         &_CreateOrderAttr,
         (arg("defaultValue") = object(), arg("writeSparsely") = false))

    .def("GetKnotsAttr", &This::GetKnotsAttr)
    .def("CreateKnotsAttr",
         &_CreateKnotsAttr,
         (arg("defaultValue") = object(), arg("writeSparsely") = false))

    .def("GetRangesAttr", &This::GetRangesAttr)
    .def("CreateRangesAttr",
         &_CreateRangesAttr,
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

}  // anonymous namespace
