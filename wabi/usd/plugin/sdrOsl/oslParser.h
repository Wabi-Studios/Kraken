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

#ifndef WABI_USD_PLUGIN_SDR_OSL_OSL_PARSER_H
#define WABI_USD_PLUGIN_SDR_OSL_OSL_PARSER_H

/// \file sdrOsl/oslParser.h

#include "wabi/base/vt/value.h"
#include "wabi/usd/ndr/parserPlugin.h"
#include "wabi/usd/plugin/sdrOsl/api.h"
#include "wabi/usd/sdr/declare.h"
#include "wabi/wabi.h"
#include <OSL/oslquery.h>

WABI_NAMESPACE_BEGIN

// Forward declarations
class NdrNode;
struct NdrNodeDiscoveryResult;

/// \class SdrOslParserPlugin
///
/// Parses OSL nodes. For more information on parser plugins, see the
/// documentation for `NdrParserPlugin`.
///
/// \section metadata_schema Metadata Schema
/// The following metadata is pulled out of the shader and available on the
/// destination as outlined below.
///
/// \subsection node_metadata Node Metadata
///
/// OSL Metadata Key      | Destination
/// --------------------- | ------------
/// category              | GetCategory()
/// departments           | GetDepartments()
/// help                  | GetHelp()
/// label                 | GetLabel()
/// \ref pvars "primvars" | GetPrimvars(), GetAdditionalPrimvarProperties()
/// _everything else_     | GetMetadata()
///
///
/// \subsection property_metadata Property Metadata
///
/// OSL Metadata Key    | Destination
/// ------------------- | ------------
/// connectable         | IsConnectable()
/// sdrDefinitionName   | renames parameter, sends original osl param name to
///                     | SdrShaderProperty::GetImplementationName()
/// page                | GetPage()
/// help                | GetHelp()
/// label               | GetLabel()
/// isDynamicArray      | IsDynamicArray()
/// \ref opts "options" | GetOptions()
/// vstructMemberName   | GetVStructMemberName()
/// vstructMemberOf     | GetVStructMemberOf()
/// _everything else_   | GetMetadata()
///
///
/// \subsection opts Options Format
/// Multiple options are separated with the pipe '|' character; options can be
/// specified in one of two ways:
/// <ul>
///     <li>
///         key1|key2|key3|...
///     </li>
///     <li>
///         key1:value1|key2:value2|...
///     </li>
/// </ul>
///
/// In the first case, where the values are not specified, the value of the
/// option will be available on the node as an empty string.
///
///
/// \subsection pvars Primvar Format
/// Multiple primvars are separated with the pipe '|' character; primvars can be
/// specified in one of two ways:
/// <ul>
///     <li>
///         A string _not_ prefixed with a '$'; this indicates a normal primvar
///     </li>
///     <li>
///         A string prefixed with '$'; the part after the '$' indicates the
///         name of a string-typed input on the node whose value contains
///         additional primvars
///     </li>
/// </ul>
///
class SdrOslParserPlugin : public NdrParserPlugin {
 public:
  typedef OSL::OSLQuery::Parameter OslParameter;

  SDROSL_API
  SdrOslParserPlugin();
  SDROSL_API
  ~SdrOslParserPlugin();

  SDROSL_API
  NdrNodeUniquePtr Parse(const NdrNodeDiscoveryResult &discoveryResult) override;

  SDROSL_API
  const NdrTokenVec &GetDiscoveryTypes() const override;

  SDROSL_API
  const TfToken &GetSourceType() const override;

 private:
  // Gets a vector of properties that are present on the specified OSL
  // query object
  NdrPropertyUniquePtrVec _getNodeProperties(const OSL::OSLQuery &query,
                                             const NdrNodeDiscoveryResult &discoveryResult) const;

  // Gets all metadata for the node that's present on the specified OSL
  // query object
  NdrTokenMap _getNodeMetadata(const OSL::OSLQuery &query, const NdrTokenMap &baseMetadata) const;

  // Gets all metadata for the specified OSL parameter
  NdrTokenMap _getPropertyMetadata(const OslParameter *param,
                                   const NdrNodeDiscoveryResult &discoveryResult) const;

  // Injects any metadata that is generated by the parser. This type of
  // metadata provides additional hints to the property.
  void _injectParserMetadata(NdrTokenMap &metadata, const TfToken &typeName) const;

  // Gets the specified parameter's value as a string. Can handle string,
  // float, and integer values (if not one of these types, an empty string
  // is returned).
  std::string _getParamAsString(const OslParameter &param) const;

  // Gets a common type + array size (if array) from the OSL parameter
  std::tuple<TfToken, size_t> _getTypeName(const OslParameter *param,
                                           const NdrTokenMap &metadata) const;

  // Gets the default value of the specified param.
  VtValue _getDefaultValue(const SdrOslParserPlugin::OslParameter &param,
                           const std::string &oslType,
                           size_t arraySize,
                           const NdrTokenMap &metadata) const;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_PLUGIN_SDR_OSL_OSL_PARSER_H
