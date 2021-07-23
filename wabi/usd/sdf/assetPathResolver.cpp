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
///
/// \file Sdf/AssetPathResolver.cpp

#include "wabi/usd/sdf/assetPathResolver.h"
#include "wabi/usd/sdf/debugCodes.h"
#include "wabi/usd/sdf/fileFormat.h"
#include "wabi/wabi.h"

#include "wabi/base/arch/systemInfo.h"
#include "wabi/base/tf/fileUtils.h"
#include "wabi/base/tf/pathUtils.h"
#include "wabi/base/tf/staticData.h"
#include "wabi/base/tf/staticTokens.h"
#include "wabi/base/trace/trace.h"
#include "wabi/usd/ar/assetInfo.h"
#include "wabi/usd/ar/packageUtils.h"
#include "wabi/usd/ar/resolver.h"

#include <utility>
#include <vector>

using std::make_pair;
using std::pair;
using std::string;
using std::vector;

WABI_NAMESPACE_BEGIN

TF_DEFINE_PRIVATE_TOKENS(_Tokens, ((AnonLayerPrefix, "anon:"))((ArgsDelimiter, ":SDF_FORMAT_ARGS:")));

bool operator==(const Sdf_AssetInfo &lhs, const Sdf_AssetInfo &rhs)
{
  return (lhs.identifier == rhs.identifier) && (lhs.resolvedPath == rhs.resolvedPath) &&
         (lhs.resolverContext == rhs.resolverContext) && (lhs.assetInfo == rhs.assetInfo);
}

bool Sdf_CanCreateNewLayerWithIdentifier(const string &identifier, string *whyNot)
{
  if (identifier.empty())
  {
    if (whyNot)
    {
      *whyNot = "cannot use empty identifier.";
    }
    return false;
  }

  if (Sdf_IsAnonLayerIdentifier(identifier))
  {
    if (whyNot)
    {
      *whyNot = "cannot use anonymous layer identifier.";
    }
    return false;
  }

  if (Sdf_IdentifierContainsArguments(identifier))
  {
    if (whyNot)
    {
      *whyNot = "cannot use arguments in the identifier.";
    }
    return false;
  }

  return ArGetResolver().CanCreateNewLayerWithIdentifier(identifier, whyNot);
}

ArResolvedPath Sdf_ResolvePath(const string &layerPath, ArAssetInfo *assetInfo)
{
  TRACE_FUNCTION();
#if AR_VERSION == 1
  return ArResolvedPath(ArGetResolver().ResolveWithAssetInfo(layerPath, assetInfo));
#else
  return ArGetResolver().Resolve(layerPath);
#endif
}

bool Sdf_CanWriteLayerToPath(const string &layerPath)
{
  return ArGetResolver().CanWriteLayerToPath(layerPath, /* whyNot = */ nullptr);
}

ArResolvedPath Sdf_ComputeFilePath(const string &layerPath, ArAssetInfo *assetInfo)
{
  TRACE_FUNCTION();

  ArResolvedPath resolvedPath = Sdf_ResolvePath(layerPath, assetInfo);
  if (resolvedPath.empty())
  {
#if AR_VERSION == 1
    // If we can't resolve layerPath, it means no layer currently
    // exists at that location. Compute the local path to figure
    // out where this layer would go if we were to create a new
    // one.
    //
    // However, we skip this for search paths since the real path
    // is ambiguous if we can't resolve the search path above.
    // This is important for layers with search path identifiers,
    // because otherwise we may compute a confusing real path
    // for these layers.
    ArResolver &resolver = ArGetResolver();
    if (!resolver.IsSearchPath(layerPath))
    {
      resolvedPath = ArResolvedPath(resolver.ComputeLocalPath(layerPath));
    }
#else
    // If we can't resolve layerPath, it means no layer currently
    // exists at that location. Use ResolveForNewAsset to figure
    // out where this layer would go if we were to create a new
    // one.
    ArResolver &resolver = ArGetResolver();
    resolvedPath = resolver.ResolveForNewAsset(layerPath);
#endif
  }

  return resolvedPath;
}

Sdf_AssetInfo *Sdf_ComputeAssetInfoFromIdentifier(const string &identifier,
                                                  const string &filePath,
                                                  const ArAssetInfo &inResolveInfo,
                                                  const string &fileVersion)
{
  // Allocate a new asset info object. The caller is responsible for
  // managing the returned object.
  Sdf_AssetInfo *assetInfo = new Sdf_AssetInfo;
  ArAssetInfo resolveInfo = inResolveInfo;

  TF_DEBUG(SDF_ASSET).Msg("Sdf_ComputeAssetInfoFromIdentifier('%s', '%s', '%s')\n",
                          identifier.c_str(),
                          filePath.c_str(),
                          fileVersion.c_str());

  if (Sdf_IsAnonLayerIdentifier(identifier))
  {
    // If the identifier is an anonymous layer identifier, don't
    // normalize, and also don't set any of the other assetInfo fields.
    // Anonymous layers do not have repository, overlay, or real paths.
    assetInfo->identifier = identifier;
  } else
  {
#if AR_VERSION == 1
    assetInfo->identifier = ArGetResolver().ComputeNormalizedPath(identifier);
#else
    assetInfo->identifier = identifier;
#endif

    if (filePath.empty())
    {
      string layerPath, arguments;
      Sdf_SplitIdentifier(assetInfo->identifier, &layerPath, &arguments);
      assetInfo->resolvedPath = Sdf_ComputeFilePath(layerPath, &resolveInfo);
    } else
    {
      assetInfo->resolvedPath = ArResolvedPath(filePath);
    }

#if AR_VERSION == 1
    assetInfo->resolvedPath = ArResolvedPath(Sdf_CanonicalizeRealPath(assetInfo->resolvedPath));

    ArGetResolver().UpdateAssetInfo(assetInfo->identifier,
                                    assetInfo->resolvedPath,
                                    fileVersion,
                                    &resolveInfo);
#else
    resolveInfo = ArGetResolver().GetAssetInfo(assetInfo->identifier, assetInfo->resolvedPath);
#endif
  }

  assetInfo->resolverContext = ArGetResolver().GetCurrentContext();
  assetInfo->assetInfo = resolveInfo;

  TF_DEBUG(SDF_ASSET).Msg(
    "Sdf_ComputeAssetInfoFromIdentifier:\n"
    "  assetInfo->identifier = '%s'\n"
    "  assetInfo->resolvedPath = '%s'\n"
    "  assetInfo->repoPath = '%s'\n"
    "  assetInfo->assetName = '%s'\n"
    "  assetInfo->version = '%s'\n",
    assetInfo->identifier.c_str(),
    assetInfo->resolvedPath.GetPathString().c_str(),
    resolveInfo.repoPath.c_str(),
    resolveInfo.assetName.c_str(),
    resolveInfo.version.c_str());

  return assetInfo;
}

string Sdf_ComputeAnonLayerIdentifier(const string &identifierTemplate, const SdfLayer *layer)
{
  TF_VERIFY(layer);
  return TfStringPrintf(identifierTemplate.c_str(), layer);
}

bool Sdf_IsAnonLayerIdentifier(const string &identifier)
{
  return TfStringStartsWith(identifier, _Tokens->AnonLayerPrefix.GetString());
}

string Sdf_GetAnonLayerDisplayName(const string &identifier)
{
  // We want to find the second occurence of ':', traversing from the left,
  // in our identifier which is of the form anon:0x4rfs23:displayName
  auto fst = std::find(identifier.begin(), identifier.end(), ':');
  if (fst == identifier.end())
  {
    return std::string();
  }

  auto snd = std::find(fst + 1, identifier.end(), ':');
  if (snd == identifier.end())
  {
    return std::string();
  }

  return identifier.substr(std::distance(identifier.begin(), snd) + 1);
}

string Sdf_GetAnonLayerIdentifierTemplate(const string &tag)
{
  string idTag = tag.empty() ? tag : TfStringTrim(tag);
  return _Tokens->AnonLayerPrefix.GetString() + "%p" + (idTag.empty() ? idTag : ":" + idTag);
}

string Sdf_CreateIdentifier(const string &layerPath, const string &arguments)
{
  return layerPath + arguments;
}

// XXX: May need to escape characters in the arguments map
// when encoding arguments and unescape then when decoding?
static string Sdf_EncodeArguments(const SdfLayer::FileFormatArguments &args)
{
  const char *delimiter = _Tokens->ArgsDelimiter.GetText();
  string argString;
  for (const auto &entry : args)
  {
    argString += delimiter;
    argString += entry.first;
    argString += '=';
    argString += entry.second;

    delimiter = "&";
  }

  return argString;
}

static bool Sdf_DecodeArguments(const string &argString, SdfLayer::FileFormatArguments *args)
{
  if (argString.empty() || argString.size() == _Tokens->ArgsDelimiter.size())
  {
    args->clear();
    return true;
  }

  const size_t argStringLength = argString.size();
  if (!TF_VERIFY(argStringLength > _Tokens->ArgsDelimiter.size()))
  {
    return false;
  }

  SdfLayer::FileFormatArguments tmpArgs;

  size_t startIdx = _Tokens->ArgsDelimiter.size();
  while (startIdx < argStringLength)
  {
    const size_t eqIdx = argString.find('=', startIdx);
    if (eqIdx == string::npos)
    {
      TF_CODING_ERROR("Invalid file format arguments: %s", argString.c_str());
      return false;
    }

    const string key = argString.substr(startIdx, eqIdx - startIdx);
    startIdx = eqIdx + 1;

    const size_t sepIdx = argString.find('&', startIdx);
    if (sepIdx == string::npos)
    {
      tmpArgs[key] = argString.substr(startIdx);
      break;
    } else
    {
      tmpArgs[key] = argString.substr(startIdx, sepIdx - startIdx);
      startIdx = sepIdx + 1;
    }
  }

  args->swap(tmpArgs);
  return true;
}

string Sdf_CreateIdentifier(const string &layerPath, const SdfLayer::FileFormatArguments &arguments)
{
  return layerPath + Sdf_EncodeArguments(arguments);
}

bool Sdf_SplitIdentifier(const string &identifier, string *layerPath, string *arguments)
{
  size_t argPos = identifier.find(_Tokens->ArgsDelimiter.GetString());
  if (argPos == string::npos)
  {
    argPos = identifier.size();
  }

  *layerPath = string(identifier, 0, argPos);
  *arguments = string(identifier, argPos, string::npos);
  return true;
}

bool Sdf_SplitIdentifier(const string &identifier, string *layerPath, SdfLayer::FileFormatArguments *args)
{
  string tmpLayerPath, tmpArgs;
  if (!Sdf_SplitIdentifier(identifier, &tmpLayerPath, &tmpArgs))
  {
    return false;
  }

  if (!Sdf_DecodeArguments(tmpArgs, args))
  {
    return false;
  }

  layerPath->swap(tmpLayerPath);
  return true;
}

bool Sdf_IdentifierContainsArguments(const string &identifier)
{
  return identifier.find(_Tokens->ArgsDelimiter.GetString()) != string::npos;
}

string Sdf_GetLayerDisplayName(const string &identifier)
{

  string layerPath, arguments;
  Sdf_SplitIdentifier(identifier, &layerPath, &arguments);

  if (Sdf_IsAnonLayerIdentifier(layerPath))
  {
    return Sdf_GetAnonLayerDisplayName(layerPath);
  }

  // If the layer path is a package-relative path, we want
  // the basename of the outermost package combined with
  // the packaged path. For example, given:
  //    "/tmp/asset.package[sub/dir/file.sdf]",
  // we want:
  //    "asset.package[sub/dir/file.sdf]".
  if (ArIsPackageRelativePath(layerPath))
  {
    std::pair<std::string, std::string> packagePath = ArSplitPackageRelativePathOuter(layerPath);
    packagePath.first = TfGetBaseName(packagePath.first);
    return ArJoinPackageRelativePath(packagePath);
  }

  return TfGetBaseName(layerPath);
}

string Sdf_GetExtension(const string &identifier)
{
  // Split the identifier to get the layer asset path without
  // any file format arguments.
  string assetPath;
  std::string dummyArgs;
  Sdf_SplitIdentifier(identifier, &assetPath, &dummyArgs);

  if (Sdf_IsAnonLayerIdentifier(assetPath))
  {
    // Strip off the "anon:0x...:" portion of the anonymous layer
    // identifier and look for an extension in the remainder. This
    // allows clients to create anonymous layers using tags that
    // match their asset path scheme and retrieve the extension
    // via ArResolver.
    assetPath = Sdf_GetAnonLayerDisplayName(assetPath);
  }

  // XXX: If the asset path is a dot file (e.g. ".sdf"), we append
  // a temporary name so that the path we pass to Ar is not
  // interpreted as a directory name. This is legacy behavior that
  // should be fixed.
  if (!assetPath.empty() && assetPath[0] == '.')
  {
    assetPath = "temp_file_name" + assetPath;
  }

  return ArGetResolver().GetExtension(assetPath);
}

bool Sdf_IsPackageOrPackagedLayer(const SdfLayerHandle &layer)
{
  return Sdf_IsPackageOrPackagedLayer(layer->GetFileFormat(), layer->GetIdentifier());
}

bool Sdf_IsPackageOrPackagedLayer(const SdfFileFormatConstPtr &fileFormat, const std::string &identifier)
{
  return fileFormat->IsPackage() || ArIsPackageRelativePath(identifier);
}

string Sdf_CanonicalizeRealPath(const string &realPath)
{
  // Use the given realPath as-is if it's a relative path, otherwise
  // use TfAbsPath to compute a platform-dependent real path.
  //
  // XXX: This method needs to be re-examined as we move towards a
  // less filesystem-dependent implementation.

  // If realPath is a package-relative path, absolutize just the
  // outer path; the packaged path has a specific format defined in
  // Ar that we don't want to modify.
  if (ArIsPackageRelativePath(realPath))
  {
    pair<string, string> packagePath = ArSplitPackageRelativePathOuter(realPath);
    return TfIsRelativePath(packagePath.first) ?
             realPath :
             ArJoinPackageRelativePath(TfAbsPath(packagePath.first), packagePath.second);
  }

  return TfIsRelativePath(realPath) ? realPath : TfAbsPath(realPath);
}

WABI_NAMESPACE_END
