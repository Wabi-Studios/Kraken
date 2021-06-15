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

#include "wabi/imaging/plugin/rmanOslParser/rmanOslParser.h"
#include "wabi/base/arch/fileSystem.h"
#include "wabi/base/gf/matrix4d.h"
#include "wabi/base/gf/vec2f.h"
#include "wabi/base/gf/vec3f.h"
#include "wabi/base/gf/vec4f.h"
#include "wabi/base/tf/errorMark.h"
#include "wabi/base/tf/fileUtils.h"
#include "wabi/base/tf/scoped.h"
#include "wabi/base/tf/staticTokens.h"
#include "wabi/base/tf/weakPtr.h"
#include "wabi/base/vt/array.h"
#include "wabi/base/vt/types.h"
#include "wabi/usd/ar/ar.h"
#include "wabi/usd/ar/asset.h"
#include "wabi/usd/ar/resolvedPath.h"
#include "wabi/usd/ar/resolver.h"
#include "wabi/usd/ndr/debugCodes.h"
#include "wabi/usd/ndr/nodeDiscoveryResult.h"
#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdr/shaderMetadataHelpers.h"
#include "wabi/usd/sdr/shaderNode.h"
#include "wabi/usd/sdr/shaderProperty.h"

#include <tuple>

WABI_NAMESPACE_BEGIN

using ShaderMetadataHelpers::IsPropertyAnAssetIdentifier;
using ShaderMetadataHelpers::IsPropertyATerminal;
using ShaderMetadataHelpers::IsTruthy;
using ShaderMetadataHelpers::OptionVecVal;

NDR_REGISTER_PARSER_PLUGIN(RmanOslParserPlugin)

/* clang-format off */
TF_DEFINE_PRIVATE_TOKENS(
  _tokens,

  ((arraySize, "arraySize"))
  ((vstructMember, "vstructmember"))
  (sdrDefinitionName)

  // Discovery and source type
  ((discoveryType, "oso"))
  ((sourceType, "OSL"))
);
/* clang-format on */

const NdrTokenVec &RmanOslParserPlugin::GetDiscoveryTypes() const
{
  static const NdrTokenVec _DiscoveryTypes = {_tokens->discoveryType};
  return _DiscoveryTypes;
}

const TfToken &RmanOslParserPlugin::GetSourceType() const
{
  return _tokens->sourceType;
}

static std::unique_ptr<RixShaderQuery> _getShaderQuery()
{
  RixContext *ctx = RixGetContextViaRMANTREE();
  if (!ctx) {
    return nullptr;
  }

  RixShaderInfo *si = (RixShaderInfo *)ctx->GetRixInterface(k_RixShaderInfo);
  if (!si) {
    return nullptr;
  }
  return std::unique_ptr<RixShaderQuery>(si->CreateQuery());
}

RmanOslParserPlugin::RmanOslParserPlugin()
{}

RmanOslParserPlugin::~RmanOslParserPlugin()
{
  // Nothing yet
}

static std::string _WriteOSLToTempFile(const std::shared_ptr<const char> &buffer, size_t numBytes)
{
  // Note that the OSL libraries require files to have a .oso extension.
  std::string tmpFilePath = ArchMakeTmpFileName("rmanOslParser", ".oso");
  FILE *f = fopen(tmpFilePath.c_str(), "w");

  if (!f) {
    TF_WARN("Failed to open temp file %s for writing", tmpFilePath.c_str());
    return std::string();
  }

  TfScoped<> fcloser([f]() { fclose(f); });

  if (fwrite(buffer.get(), sizeof(char), numBytes, f) != numBytes) {
    TF_WARN("Failed to write OSL to temp file %s", tmpFilePath.c_str());
    return std::string();
  }

  return tmpFilePath;
}

NdrNodeUniquePtr RmanOslParserPlugin::Parse(const NdrNodeDiscoveryResult &discoveryResult)
{
  std::unique_ptr<RixShaderQuery> sq = _getShaderQuery();
  if (!sq) {
    TF_WARN("Could not obtain an instance of RixShaderQuery");
    return NdrParserPlugin::GetInvalidNode(discoveryResult);
  }

  bool hasErrors = false;

  if (!discoveryResult.uri.empty()) {
#if AR_VERSION == 1
    // Get the resolved URI to a location that it can be read by the OSL
    // parser
    bool localFetchSuccessful = ArGetResolver().FetchToLocalResolvedPath(discoveryResult.uri,
                                                                         discoveryResult.resolvedUri);

    if (!localFetchSuccessful) {
      TF_WARN(
        "Could not localize the OSL at URI [%s] into a local path. "
        "An invalid Sdr node definition will be created.",
        discoveryResult.uri.c_str());

      return NdrParserPlugin::GetInvalidNode(discoveryResult);
    }
#endif
    if (TfIsFile(discoveryResult.resolvedUri.c_str())) {
      // Attempt to parse the node
      hasErrors = sq->Open(discoveryResult.resolvedUri.c_str(), "");
    }
    else {
      std::shared_ptr<const char> buffer;
      std::shared_ptr<ArAsset> asset = ArGetResolver().OpenAsset(
        ArResolvedPath(discoveryResult.resolvedUri));
      if (asset) {
        buffer = asset->GetBuffer();
      }

      if (!buffer) {
        TF_WARN(
          "Could not open the OSL file at URI [%s] (%s). "
          "An invalid Sdr node definition will be created.",
          discoveryResult.uri.c_str(),
          discoveryResult.resolvedUri.c_str());

        return NdrParserPlugin::GetInvalidNode(discoveryResult);
      }

      // RixShaderQuery currently does not have API to read from
      // a buffer, so we need to write the OSL to a temporary file.
      const std::string tmpFile = _WriteOSLToTempFile(buffer, asset->GetSize());
      hasErrors = tmpFile.empty() || sq->Open(tmpFile.c_str(), "");

      // Attempt to delete the temporary file to avoid accumulation.
      // Since these are just temporary files we'll ignore errors if
      // we can't clean them up to avoid spewage and disrupting things.
      TfErrorMark m;
      TfDeleteFile(tmpFile);
      m.Clear();
    }
  }
  else {
    TF_WARN(
      "Invalid NdrNodeDiscoveryResult with identifier %s: both uri "
      "and sourceCode are empty.",
      discoveryResult.identifier.GetText());
    return NdrParserPlugin::GetInvalidNode(discoveryResult);
  }

  std::string errors = sq->LastError();
  if (hasErrors || !errors.empty()) {
    TF_WARN(
      "Could not parse OSL shader at URI [%s]. An invalid Sdr node "
      "definition will be created. %s%s",
      discoveryResult.uri.c_str(),
      (errors.empty() ? "" : "Errors from OSL parser: "),
      (errors.empty() ? "" : TfStringReplace(errors, "\n", "; ").c_str()));

    return NdrParserPlugin::GetInvalidNode(discoveryResult);
  }

  return NdrNodeUniquePtr(
    new SdrShaderNode(discoveryResult.identifier,
                      discoveryResult.version,
                      discoveryResult.name,
                      discoveryResult.family,
                      _tokens->sourceType,
                      _tokens->sourceType,  // OSL shaders don't declare different types
                                            // so use the same type as the source type
                      discoveryResult.resolvedUri,
                      discoveryResult.resolvedUri,  // Definitive assertion that the
                                                    // implementation is the same asset
                                                    // as the definition
                      _getNodeProperties(sq.get(), discoveryResult),
                      _getNodeMetadata(sq.get(), discoveryResult.metadata),
                      discoveryResult.sourceCode));
}

NdrPropertyUniquePtrVec RmanOslParserPlugin::_getNodeProperties(
  const RixShaderQuery *sq,
  const NdrNodeDiscoveryResult &discoveryResult) const
{
  NdrPropertyUniquePtrVec properties;
  const int nParams = sq->ParameterCount();

  RixShaderParameter const *const *params = sq->Parameters();

  for (int i = 0; i < nParams; ++i) {
    const RixShaderParameter *param = params[i];
    std::string propName = param->Name();

    // Struct members are not supported
    if (propName.find('.') != std::string::npos) {
      continue;
    }

    // Extract metadata
    NdrTokenMap metadata = _getPropertyMetadata(param, discoveryResult);

    // Get type name, and determine the size of the array (if an array)
    TfToken typeName;
    size_t arraySize;
    std::tie(typeName, arraySize) = _getTypeName(param, metadata);

    _injectParserMetadata(metadata, typeName);

    // Non-standard properties in the metadata are considered hints
    NdrTokenMap hints;
    std::string definitionName;
    for (auto metaIt = metadata.cbegin(); metaIt != metadata.cend();) {
      if (std::find(SdrPropertyMetadata->allTokens.begin(),
                    SdrPropertyMetadata->allTokens.end(),
                    metaIt->first) != SdrPropertyMetadata->allTokens.end()) {
        metaIt++;
        continue;
      }

      if (metaIt->first == _tokens->sdrDefinitionName) {
        definitionName = metaIt->second;
        metaIt = metadata.erase(metaIt);
        continue;
      }

      // The metadata sometimes incorrectly specifies array size; this
      // value is not respected
      if (metaIt->first == _tokens->arraySize) {
        TF_DEBUG(NDR_PARSING)
          .Msg(
            "Ignoring bad 'arraySize' attribute on property [%s] "
            "on OSL shader [%s]",
            propName.c_str(),
            discoveryResult.name.c_str());
        metaIt = metadata.erase(metaIt);
        continue;
      }

      hints.insert(*metaIt++);
    }

    // If we found 'definitionName' metadata, we actually need to
    // change the name of the property to match, using the OSL
    // parameter name as the ImplementationName
    if (!definitionName.empty()) {
      metadata[SdrPropertyMetadata->ImplementationName] = TfToken(propName);
      propName = definitionName;
    }

    // Extract options
    NdrOptionVec options;
    if (metadata.count(SdrPropertyMetadata->Options)) {
      options = OptionVecVal(metadata.at(SdrPropertyMetadata->Options));
    }

    properties.emplace_back(SdrShaderPropertyUniquePtr(
      new SdrShaderProperty(TfToken(propName),
                            typeName,
                            _getDefaultValue(param, typeName, arraySize, metadata),
                            param->IsOutput(),
                            arraySize,
                            metadata,
                            hints,
                            options)));
  }

  return properties;
}

NdrTokenMap RmanOslParserPlugin::_getPropertyMetadata(const RixShaderParameter *param,
                                                      const NdrNodeDiscoveryResult &discoveryResult) const
{
  NdrTokenMap metadata;

  RixShaderParameter const *const *metaData = param->MetaData();
  for (int i = 0; i < param->MetaDataSize(); ++i) {
    const RixShaderParameter *metaParam = metaData[i];
    TfToken entryName = TfToken(metaParam->Name());

    // Vstruct metadata needs to be specially parsed; otherwise, just stuff
    // the value into the map
    if (entryName == _tokens->vstructMember) {
      std::string vstruct(*metaParam->DefaultS());

      if (!vstruct.empty()) {
        // A dot splits struct from member name
        size_t dotPos = vstruct.find('.');

        if (dotPos != std::string::npos) {
          metadata[SdrPropertyMetadata->VstructMemberOf] = vstruct.substr(0, dotPos);

          metadata[SdrPropertyMetadata->VstructMemberName] = vstruct.substr(dotPos + 1);
        }
        else {
          TF_DEBUG(NDR_PARSING)
            .Msg("Bad virtual structure member in %s.%s:%s",
                 discoveryResult.name.c_str(),
                 param->Name(),
                 vstruct.c_str());
        }
      }
    }
    else if (metaParam->Type() == RixShaderParameter::k_String) {
      metadata[entryName] = std::string(*metaParam->DefaultS());
    }
  }

  return metadata;
}

void RmanOslParserPlugin::_injectParserMetadata(NdrTokenMap &metadata, const TfToken &typeName) const
{
  if (typeName == SdrPropertyTypes->String) {
    if (IsPropertyAnAssetIdentifier(metadata)) {
      metadata[SdrPropertyMetadata->IsAssetIdentifier] = "";
    }
  }
}

NdrTokenMap RmanOslParserPlugin::_getNodeMetadata(const RixShaderQuery *sq,
                                                  const NdrTokenMap &baseMetadata) const
{
  NdrTokenMap nodeMetadata = baseMetadata;

  // Convert the OSL metadata to a dict.
  const int nParams = sq->MetaDataCount();
  RixShaderParameter const *const *metaData = sq->MetaData();

  for (int i = 0; i < nParams; ++i) {
    const RixShaderParameter *md = metaData[i];
    TfToken entryName = TfToken(md->Name());
    nodeMetadata[entryName] = std::string(md->Name());
  }

  return nodeMetadata;
}

std::tuple<TfToken, size_t> RmanOslParserPlugin::_getTypeName(const RixShaderParameter *param,
                                                              const NdrTokenMap &metadata) const
{
  // Exit early if this param is known to be a struct
  if (param->IsStruct()) {
    return std::make_tuple(SdrPropertyTypes->Struct, /* array size = */ 0);
  }

  // Exit early if the param's metadata indicates the param is a terminal type
  if (IsPropertyATerminal(metadata)) {
    return std::make_tuple(SdrPropertyTypes->Terminal, /* array size = */ 0);
  }

  // Otherwise, continue on to determine the type (and possibly array size)
  // std::string typeName = std::string(param->type.c_str());
  size_t arraySize = 0;
  if (param->IsArray()) {
    arraySize = (size_t)param->ArrayLength();
  }

  std::string typeName("");
  switch (param->Type()) {
    case RixShaderParameter::k_Int:
      typeName = SdrPropertyTypes->Int;
      break;
    case RixShaderParameter::k_Float:
      typeName = SdrPropertyTypes->Float;
      break;
    case RixShaderParameter::k_String:
      typeName = SdrPropertyTypes->String;
      break;
    case RixShaderParameter::k_Color:
      typeName = SdrPropertyTypes->Color;
      break;
    case RixShaderParameter::k_Point:
      typeName = SdrPropertyTypes->Point;
      break;
    case RixShaderParameter::k_Normal:
      typeName = SdrPropertyTypes->Normal;
      break;
    case RixShaderParameter::k_Vector:
      typeName = SdrPropertyTypes->Vector;
      break;
    case RixShaderParameter::k_Matrix:
      typeName = SdrPropertyTypes->Matrix;
      break;
    default:
      break;
  }

  return std::make_tuple(TfToken(typeName), arraySize);
}

VtValue RmanOslParserPlugin::_getDefaultValue(const RixShaderParameter *param,
                                              const std::string &oslType,
                                              size_t arraySize,
                                              const NdrTokenMap &metadata) const
{
  // Determine array-ness
  bool isDynamicArray = IsTruthy(SdrPropertyMetadata->IsDynamicArray, metadata);
  bool isArray = (arraySize > 0) || isDynamicArray;

  // INT and INT ARRAY
  // -------------------------------------------------------------------------
  if (oslType == SdrPropertyTypes->Int) {
    const int *dflts = param->DefaultI();
    if (!isArray && param->DefaultSize() == 1) {
      return VtValue(*dflts);
    }

    VtIntArray array;
    array.reserve((size_t)param->ArrayLength());
    for (int i = 0; i < param->ArrayLength(); ++i) {
      array.push_back(dflts[i]);
    }

    return VtValue::Take(array);
  }

  // STRING and STRING ARRAY
  // -------------------------------------------------------------------------
  else if (oslType == SdrPropertyTypes->String) {
    const char **dflts = param->DefaultS();

    // Handle non-array
    if (!isArray && param->DefaultSize() == 1) {
      return VtValue(std::string(*dflts));
    }

    // Handle array
    VtStringArray array;
    array.reserve((size_t)param->ArrayLength());
    for (int i = 0; i < param->ArrayLength(); ++i) {
      array.push_back(std::string(dflts[i]));
    }
    return VtValue::Take(array);
  }

  // FLOAT and FLOAT ARRAY
  // -------------------------------------------------------------------------
  else if (oslType == SdrPropertyTypes->Float) {
    const float *dflts = param->DefaultF();

    if (!isArray && param->DefaultSize() == 1) {
      return VtValue(*dflts);
    }

    VtFloatArray array;
    array.reserve((size_t)param->ArrayLength());
    for (int i = 0; i < param->ArrayLength(); ++i) {
      array.push_back(dflts[i]);
    }

    return VtValue::Take(array);
  }

  // VECTOR TYPES and VECTOR TYPE ARRAYS
  // -------------------------------------------------------------------------
  else if (oslType == SdrPropertyTypes->Color || oslType == SdrPropertyTypes->Point ||
           oslType == SdrPropertyTypes->Normal || oslType == SdrPropertyTypes->Vector) {

    const float *dflts = param->DefaultF();
    int dflt_size = param->DefaultSize();

    if (!isArray && dflt_size == 3) {
      return VtValue(GfVec3f(dflts[0], dflts[1], dflts[2]));
    }
    else if (isArray && dflt_size % 3 == 0) {
      int numElements = dflt_size / 3;
      VtVec3fArray array(numElements);

      for (int i = 0; i < numElements; ++i) {
        array[i] = GfVec3f(dflts[3 * i + 0], dflts[3 * i + 1], dflts[3 * i + 2]);
      }

      return VtValue::Take(array);
    }
  }

  // MATRIX
  // -------------------------------------------------------------------------
  else if (oslType == SdrPropertyTypes->Matrix) {
    // XXX: No matrix array support
    if (!isArray && param->DefaultSize() == 16) {
      GfMatrix4d mat;
      double *values = mat.GetArray();
      const float *dflts = param->DefaultF();

      for (int i = 0; i < 16; ++i) {
        values[i] = static_cast<double>(dflts[i]);
      }

      return VtValue::Take(mat);
    }
  }

  // STRUCT, TERMINAL, VSTRUCT
  // -------------------------------------------------------------------------
  else if (oslType == SdrPropertyTypes->Struct || oslType == SdrPropertyTypes->Terminal ||
           oslType == SdrPropertyTypes->Vstruct) {
    // We return an empty VtValue for Struct, Terminal, and Vstruct
    // properties because their value may rely on being computed within the
    // renderer, or we might not have a reasonable way to represent their
    // value within Sdr
    return VtValue();
  }

  // Didn't find a supported type
  return VtValue();
}

WABI_NAMESPACE_END
