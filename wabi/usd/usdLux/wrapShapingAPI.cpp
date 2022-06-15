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
#include "wabi/usd/usdLux/shapingAPI.h"
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

namespace {

#define WRAP_CUSTOM                                                     \
    template <class Cls> static void _CustomWrapCode(Cls &_class)

// fwd decl.
WRAP_CUSTOM;

        
static UsdAttribute
_CreateShapingFocusAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingFocusAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float), writeSparsely);
}
        
static UsdAttribute
_CreateShapingFocusTintAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingFocusTintAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Color3f), writeSparsely);
}
        
static UsdAttribute
_CreateShapingConeAngleAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingConeAngleAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float), writeSparsely);
}
        
static UsdAttribute
_CreateShapingConeSoftnessAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingConeSoftnessAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float), writeSparsely);
}
        
static UsdAttribute
_CreateShapingIesFileAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingIesFileAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Asset), writeSparsely);
}
        
static UsdAttribute
_CreateShapingIesAngleScaleAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingIesAngleScaleAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Float), writeSparsely);
}
        
static UsdAttribute
_CreateShapingIesNormalizeAttr(UsdLuxShapingAPI &self,
                                      object defaultVal, bool writeSparsely) {
    return self.CreateShapingIesNormalizeAttr(
        UsdPythonToSdfType(defaultVal, SdfValueTypeNames->Bool), writeSparsely);
}

static std::string
_Repr(const UsdLuxShapingAPI &self)
{
    std::string primRepr = TfPyRepr(self.GetPrim());
    return TfStringPrintf(
        "UsdLux.ShapingAPI(%s)",
        primRepr.c_str());
}

struct UsdLuxShapingAPI_CanApplyResult : 
    public TfPyAnnotatedBoolResult<std::string>
{
    UsdLuxShapingAPI_CanApplyResult(bool val, std::string const &msg) :
        TfPyAnnotatedBoolResult<std::string>(val, msg) {}
};

static UsdLuxShapingAPI_CanApplyResult
_WrapCanApply(const UsdPrim& prim)
{
    std::string whyNot;
    bool result = UsdLuxShapingAPI::CanApply(prim, &whyNot);
    return UsdLuxShapingAPI_CanApplyResult(result, whyNot);
}

} // anonymous namespace

void wrapUsdLuxShapingAPI()
{
    typedef UsdLuxShapingAPI This;

    UsdLuxShapingAPI_CanApplyResult::Wrap<UsdLuxShapingAPI_CanApplyResult>(
        "_CanApplyResult", "whyNot");

    class_<This, bases<UsdAPISchemaBase> >
        cls("ShapingAPI");

    cls
        .def(init<UsdPrim>(arg("prim")))
        .def(init<UsdSchemaBase const&>(arg("schemaObj")))
        .def(TfTypePythonClass())

        .def("Get", &This::Get, (arg("stage"), arg("path")))
        .staticmethod("Get")

        .def("CanApply", &_WrapCanApply, (arg("prim")))
        .staticmethod("CanApply")

        .def("Apply", &This::Apply, (arg("prim")))
        .staticmethod("Apply")

        .def("GetSchemaAttributeNames",
             &This::GetSchemaAttributeNames,
             arg("includeInherited")=true,
             return_value_policy<TfPySequenceToList>())
        .staticmethod("GetSchemaAttributeNames")

        .def("_GetStaticTfType", (TfType const &(*)()) TfType::Find<This>,
             return_value_policy<return_by_value>())
        .staticmethod("_GetStaticTfType")

        .def(!self)

        
        .def("GetShapingFocusAttr",
             &This::GetShapingFocusAttr)
        .def("CreateShapingFocusAttr",
             &_CreateShapingFocusAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetShapingFocusTintAttr",
             &This::GetShapingFocusTintAttr)
        .def("CreateShapingFocusTintAttr",
             &_CreateShapingFocusTintAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetShapingConeAngleAttr",
             &This::GetShapingConeAngleAttr)
        .def("CreateShapingConeAngleAttr",
             &_CreateShapingConeAngleAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetShapingConeSoftnessAttr",
             &This::GetShapingConeSoftnessAttr)
        .def("CreateShapingConeSoftnessAttr",
             &_CreateShapingConeSoftnessAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetShapingIesFileAttr",
             &This::GetShapingIesFileAttr)
        .def("CreateShapingIesFileAttr",
             &_CreateShapingIesFileAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetShapingIesAngleScaleAttr",
             &This::GetShapingIesAngleScaleAttr)
        .def("CreateShapingIesAngleScaleAttr",
             &_CreateShapingIesAngleScaleAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))
        
        .def("GetShapingIesNormalizeAttr",
             &This::GetShapingIesNormalizeAttr)
        .def("CreateShapingIesNormalizeAttr",
             &_CreateShapingIesNormalizeAttr,
             (arg("defaultValue")=object(),
              arg("writeSparsely")=false))

        .def("__repr__", ::_Repr)
    ;

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

#include "wabi/usd/usdShade/connectableAPI.h"

namespace {

WRAP_CUSTOM {
    _class
        .def(init<UsdShadeConnectableAPI>(arg("connectable")))
        .def("ConnectableAPI", &UsdLuxShapingAPI::ConnectableAPI)

        .def("CreateOutput", &UsdLuxShapingAPI::CreateOutput,
             (arg("name"), arg("type")))
        .def("GetOutput", &UsdLuxShapingAPI::GetOutput, arg("name"))
        .def("GetOutputs", &UsdLuxShapingAPI::GetOutputs,
             (arg("onlyAuthored")=true),
             return_value_policy<TfPySequenceToList>())

        .def("CreateInput", &UsdLuxShapingAPI::CreateInput,
             (arg("name"), arg("type")))
        .def("GetInput", &UsdLuxShapingAPI::GetInput, arg("name"))
        .def("GetInputs", &UsdLuxShapingAPI::GetInputs,
             (arg("onlyAuthored")=true),
             return_value_policy<TfPySequenceToList>())
        ;
}

}
