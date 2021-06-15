//
// Copyright 2017 Pixar
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
#ifndef WABI_IMAGING_HD_RENDERER_PLUGIN_REGISTRY_H
#define WABI_IMAGING_HD_RENDERER_PLUGIN_REGISTRY_H

#include "wabi/base/tf/singleton.h"
#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/renderDelegate.h"
#include "wabi/imaging/hf/pluginRegistry.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class HdRendererPlugin;
class HdRendererPluginHandle;
class HdPluginRenderDelegateUniqueHandle;

class HdRendererPluginRegistry final : public HfPluginRegistry {
 public:
  ///
  /// Returns the singleton registry for \c HdRendererPlugin
  ///
  HD_API
  static HdRendererPluginRegistry &GetInstance();

  ///
  /// Entry point for defining an HdRendererPlugin plugin.
  ///
  template<typename T, typename... Bases> static void Define();

  ///
  /// Returns the id of plugin to use as the default
  ///
  HD_API
  TfToken GetDefaultPluginId();

  ///
  /// \deprecated Use GetOrCreateRendererPlugin instead.
  ///
  /// Returns the renderer plugin for the given id or null
  /// if not found.  The reference count on the returned
  /// delegate is incremented.
  ///
  HD_API
  HdRendererPlugin *GetRendererPlugin(const TfToken &pluginId);

  ///
  /// Returns the renderer plugin for the given id or a null handle
  /// if not found. The plugin is wrapped in a handle that automatically
  /// increments and decrements the reference count and also stores the
  /// plugin id.
  ///
  HD_API
  HdRendererPluginHandle GetOrCreateRendererPlugin(const TfToken &pluginId);

  ///
  /// Returns a render delegate created by the plugin with the given name
  /// if the plugin is supported using given initial settings.
  /// The render delegate is wrapped in a movable handle that
  /// keeps the plugin alive until the render delegate is destroyed by
  /// dropping the handle.
  ///
  HD_API
  HdPluginRenderDelegateUniqueHandle CreateRenderDelegate(const TfToken &pluginId,
                                                          const HdRenderSettingsMap &settingsMap = {});

 private:
  // Friend required by TfSingleton to access constructor (as it is private).
  friend class TfSingleton<HdRendererPluginRegistry>;

  // Singleton gets private constructed
  HdRendererPluginRegistry();
  ~HdRendererPluginRegistry() override;

  //
  /// This class is not intended to be copied.
  ///
  HdRendererPluginRegistry(const HdRendererPluginRegistry &) = delete;
  HdRendererPluginRegistry &operator=(const HdRendererPluginRegistry &) = delete;
};

template<typename T, typename... Bases> void HdRendererPluginRegistry::Define()
{
  HfPluginRegistry::Define<T, HdRendererPlugin, Bases...>();
}

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_RENDERER_PLUGIN_REGISTRY_H
