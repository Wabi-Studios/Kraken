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

#include "wabi/wabi.h"
#include "wabi/base/plug/registry.h"

#include "wabi/base/plug/debugCodes.h"
#include "wabi/base/plug/notice.h"
#include "wabi/base/plug/info.h"
#include "wabi/base/plug/plugin.h"

#include "wabi/base/arch/attributes.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/fileUtils.h"
#include "wabi/base/tf/getenv.h"
#include "wabi/base/tf/instantiateSingleton.h"
#include "wabi/base/tf/mallocTag.h"
#include "wabi/base/tf/scopeDescription.h"
#include "wabi/base/tf/stl.h"
#include "wabi/base/tf/stringUtils.h"
#include "wabi/base/tf/type.h"
#include "wabi/base/work/withScopedParallelism.h"

#include <tbb/concurrent_vector.h>
#include <tbb/spin_mutex.h>

#include <functional>

using std::pair;
using std::string;
using std::vector;

WABI_NAMESPACE_BEGIN

TF_INSTANTIATE_SINGLETON(PlugRegistry);

PlugRegistry &PlugRegistry::GetInstance()
{
  return TfSingleton<This>::GetInstance();
}

PlugRegistry::PlugRegistry()
{
  TfSingleton<This>::SetInstanceConstructed(*this);
}

bool PlugRegistry::_InsertRegisteredPluginPath(const std::string &path)
{
  static tbb::spin_mutex mutex;
  tbb::spin_mutex::scoped_lock lock(mutex);
  return _registeredPluginPaths.insert(path).second;
}

template<class ConcurrentVector>
void PlugRegistry::_RegisterPlugin(const Plug_RegistrationMetadata &metadata,
                                   ConcurrentVector *newPlugins)
{
  std::pair<PlugPluginPtr, bool> newPlugin(TfNullPtr, false);
  switch (metadata.type) {
    default:
    case Plug_RegistrationMetadata::UnknownType:
      TF_CODING_ERROR(
        "Tried to register a plugin of unknown type "
        "(maybe from %s)",
        metadata.pluginPath.c_str());
      break;

    case Plug_RegistrationMetadata::LibraryType:
      newPlugin = PlugPlugin::_NewDynamicLibraryPlugin(metadata);
      break;
#ifdef WITH_PYTHON
    case Plug_RegistrationMetadata::PythonType:
      newPlugin = PlugPlugin::_NewPythonModulePlugin(metadata);
      break;
#endif  // WITH_PYTHON
    case Plug_RegistrationMetadata::ResourceType:
      newPlugin = PlugPlugin::_NewResourcePlugin(metadata);
      break;
  }

  if (newPlugin.second) {
    newPlugins->push_back(newPlugin.first);
  }
}

PlugPluginPtrVector PlugRegistry::RegisterPlugins(const std::string &pathToPlugInfo)
{
  return RegisterPlugins(vector<string>(1, pathToPlugInfo));
}

PlugPluginPtrVector PlugRegistry::RegisterPlugins(const std::vector<std::string> &pathsToPlugInfo)
{
  const bool pathsAreOrdered = true;
  PlugPluginPtrVector result = _RegisterPlugins(pathsToPlugInfo, pathsAreOrdered);
  if (!result.empty()) {
    PlugNotice::DidRegisterPlugins(result).Send(TfCreateWeakPtr(this));
  }
  return result;
}

PlugPluginPtrVector PlugRegistry::_RegisterPlugins(const std::vector<std::string> &pathsToPlugInfo,
                                                   bool pathsAreOrdered)
{
  TF_DESCRIBE_SCOPE("Registering plugins");
  TfAutoMallocTag2 tag2("Plug", "PlugRegistry::RegisterPlugins");

  typedef tbb::concurrent_vector<PlugPluginPtr> NewPluginsVec;
  NewPluginsVec newPlugins;
  {
    Plug_TaskArena taskArena;
    // XXX -- Is this mutex really needed?
    std::lock_guard<std::mutex> lock(_mutex);
    WorkWithScopedParallelism([&]() {
      Plug_ReadPlugInfo(
        pathsToPlugInfo,
        pathsAreOrdered,
        std::bind(&PlugRegistry::_InsertRegisteredPluginPath, this, std::placeholders::_1),
        std::bind(&PlugRegistry::_RegisterPlugin<NewPluginsVec>,
                  this,
                  std::placeholders::_1,
                  &newPlugins),
        &taskArena);
    });
  }

  if (!newPlugins.empty()) {
    PlugPluginPtrVector v(newPlugins.begin(), newPlugins.end());
    for (const auto &plug : v) {
      plug->_DeclareTypes();
    }
    return v;
  }
  return PlugPluginPtrVector();
}

PlugPluginPtr PlugRegistry::GetPluginForType(TfType t) const
{
  if (t.IsUnknown()) {
    TF_CODING_ERROR("Unknown base type");
    return TfNullPtr;
  }
  return PlugPlugin::_GetPluginForType(t);
}

PlugPluginPtrVector PlugRegistry::GetAllPlugins() const
{
  return PlugPlugin::_GetAllPlugins();
}

PlugPluginPtr PlugRegistry::GetPluginWithName(const string &name) const
{
  return PlugPlugin::_GetPluginWithName(name);
}

JsValue PlugRegistry::GetDataFromPluginMetaData(TfType type, const string &key) const
{
  JsValue result;

  string typeName = type.GetTypeName();
  PlugPluginPtr plugin = GetPluginForType(type);
  if (plugin) {
    JsObject dict = plugin->GetMetadataForType(type);
    TfMapLookup(dict, key, &result);
  }
  return result;
}

string PlugRegistry::GetStringFromPluginMetaData(TfType type, const string &key) const
{
  JsValue v = GetDataFromPluginMetaData(type, key);
  return v.IsString() ? v.GetString() : string();
}

TfType PlugRegistry::FindTypeByName(std::string const &typeName)
{
  PlugPlugin::_RegisterAllPlugins();
  return TfType::FindByName(typeName);
}

TfType PlugRegistry::FindDerivedTypeByName(TfType base, std::string const &typeName)
{
  PlugPlugin::_RegisterAllPlugins();
  return base.FindDerivedByName(typeName);
}

std::vector<TfType> PlugRegistry::GetDirectlyDerivedTypes(TfType base)
{
  PlugPlugin::_RegisterAllPlugins();
  return base.GetDirectlyDerivedTypes();
}

void PlugRegistry::GetAllDerivedTypes(TfType base, std::set<TfType> *result)
{
  PlugPlugin::_RegisterAllPlugins();
  base.GetAllDerivedTypes(result);
}

namespace
{

  struct PathsInfo
  {
    std::vector<std::string> paths;
    std::vector<std::string> debugMessages;
    bool pathsAreOrdered = true;
  };

  // Return a static vector<string> that holds the bootstrap plugin paths.
  static PathsInfo &Plug_GetPathsInfo()
  {
    // This is a static local variable since the function is called from
    // ARCH_CONSTRUCTOR methods, potentially before module-level static
    // initialization.
    static PathsInfo pathsInfo;
    return pathsInfo;
  }

}  // namespace

void Plug_SetPaths(const std::vector<std::string> &paths,
                   const std::vector<std::string> &debugMessages,
                   bool pathsAreOrdered)
{
  auto &pathsInfo = Plug_GetPathsInfo();
  pathsInfo.paths = paths;
  pathsInfo.debugMessages = debugMessages;
  pathsInfo.pathsAreOrdered = pathsAreOrdered;
}

// This is here so plugin.cpp doesn't have to include info.h or registry.h.
void PlugPlugin::_RegisterAllPlugins()
{
  PlugPluginPtrVector result;

  static std::once_flag once;
  std::call_once(once, [&result]() {
    PlugRegistry &registry = PlugRegistry::GetInstance();

    if (!TfGetenvBool("WABI_DISABLE_STANDARD_PLUG_SEARCH_PATH", false)) {
      // Emit any debug messages first, then call _RegisterPlugins.
      for (std::string const &msg : Plug_GetPathsInfo().debugMessages) {
        TF_DEBUG(PLUG_INFO_SEARCH).Msg("%s", msg.c_str());
      }
      // Register plugins in the tree. This declares TfTypes.
      result = registry._RegisterPlugins(Plug_GetPathsInfo().paths,
                                         Plug_GetPathsInfo().pathsAreOrdered);
    }
  });


  // Send a notice outside of the call_once.  We don't want to be holding
  // a lock (even an implicit one) when sending a notice.
  if (!result.empty()) {
    PlugNotice::DidRegisterPlugins(result).Send(TfCreateWeakPtr(&PlugRegistry::GetInstance()));
  }
}

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<PlugRegistry>();
}

WABI_NAMESPACE_END
