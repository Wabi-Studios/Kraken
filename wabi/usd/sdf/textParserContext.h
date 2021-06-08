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
#ifndef WABI_USD_SDF_TEXT_PARSER_CONTEXT_H
#define WABI_USD_SDF_TEXT_PARSER_CONTEXT_H

#include "wabi/usd/sdf/data.h"
#include "wabi/usd/sdf/layerHints.h"
#include "wabi/usd/sdf/layerOffset.h"
#include "wabi/usd/sdf/listOp.h"
#include "wabi/usd/sdf/parserValueContext.h"
#include "wabi/usd/sdf/path.h"
#include "wabi/usd/sdf/payload.h"
#include "wabi/usd/sdf/reference.h"
#include "wabi/usd/sdf/types.h"
#include "wabi/wabi.h"

#include "wabi/base/vt/dictionary.h"

#include "wabi/base/tf/token.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>

// Lexical scanner type.
typedef void *yyscan_t;

WABI_NAMESPACE_BEGIN

// This class contains the global state while parsing a menva file.
// It contains the data structures that we use to create the scene description
// from the file.

class Sdf_TextParserContext {
 public:
  // Constructor.
  Sdf_TextParserContext();

  // Destructor.
  ~Sdf_TextParserContext()
  {}

  std::string magicIdentifierToken;
  std::string versionString;
  std::string fileContext;

  // State for layer refs, in general
  std::string layerRefPath;
  SdfLayerOffset layerRefOffset;

  // State for sublayers
  std::vector<std::string> subLayerPaths;

  // State for sublayer offsets
  std::vector<SdfLayerOffset> subLayerOffsets;

  // String list currently being built
  std::vector<TfToken> nameVector;

  SdfTimeSampleMap timeSamples;
  double timeSampleTime;

  SdfPath savedPath;

  // Whether the current relationship target being parsed is allowed to
  // have data like relational attributes.
  bool relParsingAllowTargetData;
  // relationship target paths that will be saved in a list op
  // (use a boost::optional to track whether we have seen an opinion at all.)
  boost::optional<SdfPathVector> relParsingTargetPaths;
  // relationship target paths that will be appended to the relationship's
  // list of target children.
  SdfPathVector relParsingNewTargetChildren;

  // helpers for connection path parsing
  SdfPathVector connParsingTargetPaths;
  bool connParsingAllowConnectionData;

  // helpers for inherit path parsing
  SdfPathVector inheritParsingTargetPaths;

  // helpers for specializes path parsing
  SdfPathVector specializesParsingTargetPaths;

  // helpers for reference parsing
  SdfReferenceVector referenceParsingRefs;

  // helpers for payload parsing
  SdfPayloadVector payloadParsingRefs;

  // helper for relocates parsing
  SdfRelocatesMap relocatesParsingMap;

  // helpers for generic metadata
  TfToken genericMetadataKey;
  SdfListOpType listOpType;

  // The value parser context
  Sdf_ParserValueContext values;

  // Last parsed value
  VtValue currentValue;

  // Vector of dictionaries used to parse nested dictionaries.
  // The first element in the vector contains the last parsed dictionary.
  std::vector<VtDictionary> currentDictionaries;

  bool seenError;

  bool custom;
  SdfSpecifier specifier;
  SdfDataRefPtr data;
  SdfPath path;
  TfToken typeName;
  VtValue variability;
  VtValue assoc;

  // Should we only read metadata from the file?
  bool metadataOnly;

  // Hints to fill in about the layer's contents.
  SdfLayerHints layerHints;

  // Stack for the child names of all the prims currently being parsed
  // For instance if we're currently parsing /A/B then this vector
  // will contain three elements:
  //    names of the root prims
  //    names of A's children
  //    names of B's children.
  std::vector<std::vector<TfToken>> nameChildrenStack;

  // Stack for the property names of all the objects currently being parsed
  std::vector<std::vector<TfToken>> propertiesStack;

  // Stack of names of variant sets  being built.
  std::vector<std::string> currentVariantSetNames;

  // Stack of names of variants for the variant sets being built
  std::vector<std::vector<std::string>> currentVariantNames;

  unsigned int menvaLineNo;

  // Used by flex for reentrant parsing
  yyscan_t scanner;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_SDF_TEXT_PARSER_CONTEXT_H
