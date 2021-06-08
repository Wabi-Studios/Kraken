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

#include "wabi/usd/ndr/filesystemDiscovery.h"
#include "wabi/base/arch/fileSystem.h"
#include "wabi/usd/ndr/filesystemDiscoveryHelpers.h"

#include "wabi/base/tf/envSetting.h"
#include "wabi/base/tf/stringUtils.h"

#include <string>

WABI_NAMESPACE_BEGIN

NDR_REGISTER_DISCOVERY_PLUGIN(_NdrFilesystemDiscoveryPlugin)

TF_DEFINE_ENV_SETTING(WABI_NDR_FS_PLUGIN_SEARCH_PATHS,
                      "",
                      "The paths that should be searched, recursively, for files that represent "
                      "nodes. Paths should be separated by either a ':' or a ';' depending on "
                      "your platform (it should mimic the PATH attribute).  See "
                      "ARCH_PATH_LIST_SEP");

TF_DEFINE_ENV_SETTING(WABI_NDR_FS_PLUGIN_ALLOWED_EXTS,
                      "",
                      "The extensions on files that define nodes.  Do not include the leading "
                      "'.'. Extensions should be separated by a colon.");

TF_DEFINE_ENV_SETTING(WABI_NDR_FS_PLUGIN_FOLLOW_SYMLINKS,
                      false,
                      "Whether symlinks should be followed while walking the search paths. Set "
                      "to 'true' (case sensitive) if they should be followed.");

_NdrFilesystemDiscoveryPlugin::_NdrFilesystemDiscoveryPlugin()
{
  //
  // TODO: This needs to somehow be set up to find the nodes that USD
  //       ships with
  //
  _searchPaths       = TfStringSplit(TfGetEnvSetting(WABI_NDR_FS_PLUGIN_SEARCH_PATHS),
                               ARCH_PATH_LIST_SEP);
  _allowedExtensions = TfStringSplit(TfGetEnvSetting(WABI_NDR_FS_PLUGIN_ALLOWED_EXTS), ":");
  _followSymlinks    = TfGetEnvSetting(WABI_NDR_FS_PLUGIN_FOLLOW_SYMLINKS);
}

_NdrFilesystemDiscoveryPlugin::_NdrFilesystemDiscoveryPlugin(Filter filter)
    : _NdrFilesystemDiscoveryPlugin()
{
  _filter = std::move(filter);
}

NdrNodeDiscoveryResultVec _NdrFilesystemDiscoveryPlugin::DiscoverNodes(const Context &context)
{
  auto result = NdrFsHelpersDiscoverNodes(
      _searchPaths, _allowedExtensions, _followSymlinks, &context);

  if (_filter) {
    auto j = result.begin();
    for (auto i = j; i != result.end(); ++i) {
      // If we pass the filter and any previous haven't then move.
      if (_filter(*i)) {
        if (j != i) {
          *j = std::move(*i);
        }
        ++j;
      }
    }
    result.erase(j, result.end());
  }

  return result;
}

WABI_NAMESPACE_END
