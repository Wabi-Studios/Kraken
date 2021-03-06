//
// Copyright 2019 Pixar
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

#ifndef EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_RMAN_ARGS_PARSER_RMAN_ARGS_PARSER_H
#define EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_RMAN_ARGS_PARSER_RMAN_ARGS_PARSER_H

/// \file rmanArgsParser/rmanArgsParser.h

#include "wabi/base/vt/value.h"
#include "wabi/imaging/plugin/rmanArgsParser/api.h"
#include "wabi/usd/ndr/parserPlugin.h"
#include "wabi/usd/sdr/declare.h"
#include "wabi/wabi.h"

#include "wabi/imaging/plugin/rmanArgsParser/pugixml/pugixml.hpp"

WABI_NAMESPACE_BEGIN

// Forward declarations
class NdrNode;
struct NdrNodeDiscoveryResult;
struct SdrShaderRepresentation;

/// \class RmanArgsParserPlugin
///
/// Parses Args files. For more information on parser plugins, see the
/// documentation for `NdrParserPlugin`.
///
/// \section schema Schema
/// The following elements, along with their attributes (italics) and child
/// elements, are respected in this parser:
///
/// * <param> and <output>
///   * <help>
///   * <hintdict>
///     * <string>
///       * _name_
///       * _value_
///     * _name_
///   * <hintlist>
///     * <string>
///       * _value_
///     * _name_
///   * <tags>
///     * <tag>
///       * _value_
///   * _name_
///   * _type_ *!* (deprecated on outputs only)
///   * _default_
///   * _label_
///   * _widget_
///   * _arraySize_
///   * _isDynamicArray_
///   * _connectable_
///   * _options_
///   * _page_
///   * _input_ *!*
///   * _help_
///   * _tag_ *!*
///   * _validConnectionTypes_
///   * _vstructmember_
///   * _sdrDefinitionName_ (renames parameter, sends original args param name to
///                          SdrShaderProperty::GetImplementationName())
///   * Note: other uncategorized attributes are available via NdrNode::GetHints()
/// * <page> _Can be nested_
///   * _name_
/// * <help>
/// * <primvars>
///   * <primvar>
///     * _name_
/// * <departments>
/// * <shaderType>
///   * _name_
///   * <tag>
///     * _value_
/// * <typeTag> *!*
///   * <tag>
///     * _value_
///
/// For more information on the specifics of what any of these elements or
/// attributes mean, see the Renderman documentation on the Args format. Items
/// marked with a '!' are deprecated and will output a warning.
///
class RmanArgsParserPlugin : public NdrParserPlugin
{
 public:

  RMAN_ARGS_PARSER_API
  RmanArgsParserPlugin();
  RMAN_ARGS_PARSER_API
  ~RmanArgsParserPlugin();

  RMAN_ARGS_PARSER_API
  NdrNodeUniquePtr Parse(const NdrNodeDiscoveryResult &discoveryRes) override;

  RMAN_ARGS_PARSER_API
  const NdrTokenVec &GetDiscoveryTypes() const override;

  RMAN_ARGS_PARSER_API
  const TfToken &GetSourceType() const override;

 private:

  // Gets a common type + array size (if array) from the property attributes
  std::tuple<TfToken, size_t> _GetTypeName(const NdrTokenMap &attributes) const;

  // Gets a VtValue instance from a string value and a type, array, metadata
  // hint
  VtValue _GetVtValue(const std::string &stringValue,
                      TfToken &type,
                      size_t arraySize,
                      const NdrTokenMap &metadata) const;

  // Emits a deprecation warning to the log
  void _OutputDeprecationWarning(const TfToken &attrName,
                                 const SdrShaderRepresentation &shaderRep,
                                 const TfToken &propName) const;

  // Parses the given child's data as a preparation to be handed off to
  // `_CreateProperty` to actually be created
  SdrShaderPropertyUniquePtr _ParseChildElem(const SdrShaderRepresentation &shaderRep,
                                             bool isOutput,
                                             pugi::xml_node childElement,
                                             const std::string &parentPage) const;

  // Creates a SdrShaderProperty with the given inputs
  SdrShaderPropertyUniquePtr _CreateProperty(const SdrShaderRepresentation &entry,
                                             bool isOutput,
                                             NdrTokenMap &attributes,
                                             NdrStringVec &validConnectionTypes,
                                             NdrOptionVec &options) const;

  // Injects any metadata that is generated by the parser. This type of
  // metadata provides additional hints to the property.
  void _injectParserMetadata(NdrTokenMap &metadata, const TfToken &typeName) const;

  // Recursively builds up the shaderRep as the shader is parsed
  void _Parse(SdrShaderRepresentation &shaderRep,
              pugi::xml_node parent,
              const std::string &parentPage);

  // For each child under the specified parent, get the value of the
  // specified attribute
  NdrStringVec _GetAttributeValuesFromChildren(pugi::xml_node parent, const char *attrName) const;

  // Helper methods to get a value out of the attr map (or the default value
  // if the key doesn't exist on the map)
  std::string _Get(const NdrTokenMap &map, const TfToken &key, std::string defaultValue) const;

  TfToken _Get(const NdrTokenMap &map, const TfToken &key, TfToken defaultValue) const;

  int _Get(const NdrTokenMap &map, const TfToken &key, int defaultValue) const;

  float _Get(const NdrTokenMap &map, const TfToken &key, float defaultValue) const;

  std::string _GetDsoPathFromArgsPath(const std::string &argsPath);
};

WABI_NAMESPACE_END

#endif  // EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_RMAN_ARGS_PARSER_RMAN_ARGS_PARSER_H
