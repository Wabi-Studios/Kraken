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

#include "wabi/usd/usd/modelAPI.h"
#include "wabi/usd/usd/schemaBase.h"

#include "wabi/usd/sdf/primSpec.h"

#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"
#include "wabi/usd/usd/pyConversions.h"

#include <boost/python.hpp>

#include <string>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace
{
#define WRAP_CUSTOM   \
  template<class Cls> \
  static void _CustomWrapCode(Cls &_class)

  /**
   * fwds ->. */
  WRAP_CUSTOM;

  static std::string _Repr(const UsdModelAPI &self)
  {
    std::string primRepr = TfPyRepr(self.GetPrim());
    return TfStringPrintf("Usd.ModelAPI(%s)", primRepr.c_str());
  }

}  // namespace

/* clang-format off */
void wrapUsdModelAPI()
{
  typedef UsdModelAPI This;

  class_<This, bases<UsdAPISchemaBase>> cls("ModelAPI");

  cls
    .def(init<UsdPrim>
        (arg("prim")))
    .def(init<UsdSchemaBase const &>
        (arg("schemaObj")))
    .def(TfTypePythonClass())
    .def("Get", &This::Get, (arg("stage"), arg("path")))
    .staticmethod("Get")
    .def("GetSchemaAttributeNames", &This::GetSchemaAttributeNames,
        arg("includeInherited") = true,
        return_value_policy<TfPySequenceToList>())
        .staticmethod("GetSchemaAttributeNames")
    .def("GetStaticTfType",
        (TfType const &(*)())TfType::Find<This>,
        return_value_policy<return_by_value>())
    .staticmethod("GetStaticTfType")
    .def(!self)
    .def("__repr__", ::_Repr)
  ;

  _CustomWrapCode(cls);
}

  /**
   * ======================================================================
   *     Feel free to add custom code below this line, it will be preserved
   *     by the code generator. The entry point for your custom code should
   *     look minimally like the following:
   *
   *         WRAP_CUSTOM {
   *           _class
   *             .def("MyCustomMethod", ...)
   *           ;
   *         }
   *
   *     Of course any other ancillary or support code may be provided.
   *     Just remember to wrap code in the appropriate delimiters:
   *     'namespace {', '}'.
   * ======================================================================
   * --(BEGIN CUSTOM CODE)-- */

/* clang-format on */

#include "wabi/base/tf/pyEnum.h"
#include "wabi/base/tf/pyStaticTokens.h"

namespace
{

  static TfToken _GetKind(const UsdModelAPI &self)
  {
    TfToken result;
    self.GetKind(&result);
    return result;
  }

  static SdfAssetPath _GetAssetIdentifier(const UsdModelAPI &self)
  {
    SdfAssetPath identifier;
    self.GetAssetIdentifier(&identifier);
    return identifier;
  }

  static std::string _GetAssetName(const UsdModelAPI &self)
  {
    std::string assetName;
    self.GetAssetName(&assetName);
    return assetName;
  }

  static std::string _GetAssetVersion(const UsdModelAPI &self)
  {
    std::string version;
    self.GetAssetVersion(&version);
    return version;
  }

  static VtArray<SdfAssetPath> _GetPayloadAssetDependencies(const UsdModelAPI &self)
  {
    VtArray<SdfAssetPath> assetDeps;
    self.GetPayloadAssetDependencies(&assetDeps);
    return assetDeps;
  }

  static VtDictionary _GetAssetInfo(const UsdModelAPI &self)
  {
    VtDictionary info;
    self.GetAssetInfo(&info);
    return info;
  }

  WRAP_CUSTOM
  {
    /* clang-format off */
  TF_PY_WRAP_PUBLIC_TOKENS("AssetInfoKeys", UsdModelAPIAssetInfoKeys, USDMODEL_ASSET_INFO_KEYS);
  {
    scope parent = _class;
    /**
     * This must be defined before KindValidationModelHierarchy
     * is used as a default argument to validation. */
    TfPyWrapEnum<UsdModelAPI::KindValidation>();
    _class
      .def("GetKind", _GetKind)
      .def("SetKind", &UsdModelAPI::SetKind,
           arg("value"))
      .def("IsKind", &UsdModelAPI::IsKind,
          (arg("baseKind"),
           arg("validation") = UsdModelAPI::KindValidationModelHierarchy))
      .def("IsModel", &UsdModelAPI::IsModel)
      .def("IsGroup", &UsdModelAPI::IsGroup)
      .def("GetAssetIdentifier", _GetAssetIdentifier)
      .def("SetAssetIdentifier", &UsdModelAPI::SetAssetIdentifier)
      .def("GetAssetName", _GetAssetName)
      .def("SetAssetName", &UsdModelAPI::SetAssetName)
      .def("GetAssetVersion", _GetAssetVersion)
      .def("SetAssetVersion", &UsdModelAPI::SetAssetVersion)
      .def("GetPayloadAssetDependencies", _GetPayloadAssetDependencies)
      .def("SetPayloadAssetDependencies", &UsdModelAPI::SetPayloadAssetDependencies)
      .def("GetAssetInfo", _GetAssetInfo)
      .def("SetAssetInfo", &UsdModelAPI::SetAssetInfo);
  }
    /* clang-format on */
  }

}  // namespace
