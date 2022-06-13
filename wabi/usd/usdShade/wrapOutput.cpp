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
#include "wabi/usd/usdShade/connectableAPI.h"
#include "wabi/usd/usdShade/output.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/usd/usd/pyConversions.h"

#include <boost/python/class.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/tuple.hpp>

#include <vector>

using std::vector;
using namespace boost::python;

WABI_NAMESPACE_USING

namespace
{

  static bool _Set(const UsdShadeOutput &self, object val, const UsdTimeCode &time)
  {
    return self.Set(UsdPythonToSdfType(val, self.GetTypeName()), time);
  }

  static object _GetConnectedSources(const UsdShadeOutput &self)
  {
    SdfPathVector invalidSourcePaths;
    UsdShadeOutput::SourceInfoVector sources = self.GetConnectedSources(&invalidSourcePaths);
    return boost::python::make_tuple(
      std::vector<UsdShadeConnectionSourceInfo>(sources.begin(), sources.end()),
      invalidSourcePaths);
  }

  static object _GetConnectedSource(const UsdShadeOutput &self)
  {
    UsdShadeConnectableAPI source;
    TfToken sourceName;
    UsdShadeAttributeType sourceType;

    if (self.GetConnectedSource(&source, &sourceName, &sourceType)) {
      return boost::python::make_tuple(source, sourceName, sourceType);
    } else {
      return object();
    }
  }

  static SdfPathVector _GetRawConnectedSourcePaths(const UsdShadeOutput &self)
  {
    SdfPathVector sourcePaths;
    self.GetRawConnectedSourcePaths(&sourcePaths);
    return sourcePaths;
  }

}  // anonymous namespace

void wrapUsdShadeOutput()
{
  typedef UsdShadeOutput Output;

  bool (Output::*ConnectToSource_1)(UsdShadeConnectableAPI const &,
                                    TfToken const &,
                                    UsdShadeAttributeType const,
                                    SdfValueTypeName) const = &Output::ConnectToSource;

  bool (Output::*ConnectToSource_2)(SdfPath const &) const = &Output::ConnectToSource;

  bool (Output::*ConnectToSource_3)(UsdShadeInput const &) const = &Output::ConnectToSource;

  bool (Output::*ConnectToSource_4)(UsdShadeOutput const &) const = &Output::ConnectToSource;

  bool (Output::*ConnectToSource_5)(UsdShadeConnectionSourceInfo const &,
                                    Output::ConnectionModification const mod)
    const = &Output::ConnectToSource;

  bool (Output::*CanConnect_1)(UsdAttribute const &) const = &Output::CanConnect;

  class_<Output>("Output")
    .def(init<UsdAttribute>(arg("attr")))
    .def(self == self)
    .def(self != self)
    .def(!self)

    .def("GetFullName", &Output::GetFullName, return_value_policy<return_by_value>())
    .def("GetBaseName", &Output::GetBaseName)
    .def("GetPrim", &Output::GetPrim)
    .def("GetTypeName", &Output::GetTypeName)
    .def("Set", _Set, (arg("value"), arg("time") = UsdTimeCode::Default()))
    .def("SetRenderType", &Output::SetRenderType, (arg("renderType")))
    .def("GetRenderType", &Output::GetRenderType)
    .def("HasRenderType", &Output::HasRenderType)

    .def("GetSdrMetadata", &Output::GetSdrMetadata)
    .def("GetSdrMetadataByKey", &Output::GetSdrMetadataByKey, (arg("key")))

    .def("SetSdrMetadata", &Output::SetSdrMetadata, (arg("sdrMetadata")))
    .def("SetSdrMetadataByKey", &Output::SetSdrMetadataByKey, (arg("key"), arg("value")))

    .def("HasSdrMetadata", &Output::HasSdrMetadata)
    .def("HasSdrMetadataByKey", &Output::HasSdrMetadataByKey, (arg("key")))

    .def("ClearSdrMetadata", &Output::ClearSdrMetadata)
    .def("ClearSdrMetadataByKey", &Output::ClearSdrMetadataByKey, (arg("key")))

    .def("GetAttr", &Output::GetAttr)

    .def("CanConnect", CanConnect_1, (arg("source")))

    .def("ConnectToSource",
         ConnectToSource_5,
         (arg("source"), arg("mod") = UsdShadeConnectionModification::Replace))
    .def("ConnectToSource",
         ConnectToSource_1,
         (arg("source"),
          arg("sourceName"),
          arg("sourceType") = UsdShadeAttributeType::Output,
          arg("typeName") = SdfValueTypeName()))
    .def("ConnectToSource", ConnectToSource_2, (arg("sourcePath")))
    .def("ConnectToSource", ConnectToSource_3, (arg("sourceInput")))
    .def("ConnectToSource", ConnectToSource_4, (arg("sourceOutput")))

    .def("SetConnectedSources", &Output::SetConnectedSources)

    .def("GetConnectedSources", _GetConnectedSources)
    .def("GetConnectedSource", _GetConnectedSource)
    .def("GetRawConnectedSourcePaths",
         _GetRawConnectedSourcePaths,
         return_value_policy<TfPySequenceToList>())
    .def("HasConnectedSource", &Output::HasConnectedSource)
    .def("IsSourceConnectionFromBaseMaterial", &Output::IsSourceConnectionFromBaseMaterial)
    .def("DisconnectSource", &Output::DisconnectSource, (arg("sourceAttr") = UsdAttribute()))
    .def("ClearSources", &Output::ClearSources)
    .def("ClearSource", &Output::ClearSource)

    .def("GetValueProducingAttributes",
         &Output::GetValueProducingAttributes,
         (arg("shaderOutputsOnly") = false))

    .def("IsOutput", &Output::IsOutput)
    .staticmethod("IsOutput");

  implicitly_convertible<Output, UsdAttribute>();

  to_python_converter<std::vector<Output>, TfPySequenceToPython<std::vector<Output>>>();
}
