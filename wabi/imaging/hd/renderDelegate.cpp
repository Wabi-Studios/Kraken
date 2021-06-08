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
#include "wabi/imaging/hd/renderDelegate.h"
#include "wabi/imaging/hd/renderPassState.h"
#include "wabi/imaging/hd/tokens.h"

#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/type.h"

#include <iostream>

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<HdRenderDelegate>();
}

//
// WORKAROUND: As these classes are pure interface classes, they do not need a
// vtable.  However, it is possible that some users will use rtti.
// This will cause a problem for some of our compilers:
//
// In particular clang will throw a warning: -wweak-vtables
// For gcc, there is an issue were the rtti typeid's are different.
//
// As destruction of the class is not on the performance path,
// the body of the deleter is provided here, so a vtable is created
// in this compilation unit.
HdRenderParam::~HdRenderParam()       = default;
HdRenderDelegate::~HdRenderDelegate() = default;

HdRenderDelegate::HdRenderDelegate() : _settingsMap(), _settingsVersion(1)
{}

HdRenderDelegate::HdRenderDelegate(HdRenderSettingsMap const &settingsMap)
    : _settingsMap(),
      _settingsVersion(1)
{
  _settingsMap = settingsMap;
  if (TfDebug::IsEnabled(HD_RENDER_SETTINGS)) {
    std::cout << "Initial Render Settings" << std::endl;
    for (auto const &pair : _settingsMap) {
      std::cout << "\t[" << pair.first << "] = " << pair.second << std::endl;
    }
  }
}

void HdRenderDelegate::SetDrivers(HdDriverVector const &drivers)
{}

HdRenderPassStateSharedPtr HdRenderDelegate::CreateRenderPassState() const
{
  return std::make_shared<HdRenderPassState>();
}

TfToken HdRenderDelegate::GetMaterialBindingPurpose() const
{
  return HdTokens->preview;
}

TfTokenVector HdRenderDelegate::GetShaderSourceTypes() const
{
  return TfTokenVector();
}

// deprecated
TfToken HdRenderDelegate::GetMaterialNetworkSelector() const
{
  return TfToken();
}

TfTokenVector HdRenderDelegate::GetMaterialRenderContexts() const
{
  // To support RenderDelegates that have not yet updated
  // GetMaterialNetworkSelector()
  return {GetMaterialNetworkSelector()};
}

bool HdRenderDelegate::IsPrimvarFilteringNeeded() const
{
  return false;
}

HdAovDescriptor HdRenderDelegate::GetDefaultAovDescriptor(TfToken const &name) const
{
  return HdAovDescriptor();
}

HdRenderSettingDescriptorList HdRenderDelegate::GetRenderSettingDescriptors() const
{
  return HdRenderSettingDescriptorList();
}

void HdRenderDelegate::SetRenderSetting(TfToken const &key, VtValue const &value)
{
  auto iter = _settingsMap.find(key);
  if (iter == _settingsMap.end()) {
    _settingsMap[key] = value;
    ++_settingsVersion;
  }
  else if (iter->second != value) {
    iter->second = value;
    ++_settingsVersion;
  }

  if (TfDebug::IsEnabled(HD_RENDER_SETTINGS)) {
    std::cout << "Render Setting [" << key << "] = " << value << std::endl;
  }
}

VtValue HdRenderDelegate::GetRenderSetting(TfToken const &key) const
{
  auto it = _settingsMap.find(key);
  if (it != _settingsMap.end()) {
    return it->second;
  }

  if (TfDebug::IsEnabled(HD_RENDER_SETTINGS)) {
    std::cout << "Render setting not found for key [" << key << "]" << std::endl;
  }
  return VtValue();
}

unsigned int HdRenderDelegate::GetRenderSettingsVersion() const
{
  return _settingsVersion;
}

HdCommandDescriptors HdRenderDelegate::GetCommandDescriptors() const
{
  return HdCommandDescriptors();
}

bool HdRenderDelegate::InvokeCommand(const TfToken &command, const HdCommandArgs &args)
{
  // Fail all commands that get here.
  return false;
}

VtDictionary HdRenderDelegate::GetRenderStats() const
{
  return VtDictionary();
}

void HdRenderDelegate::_PopulateDefaultSettings(
    HdRenderSettingDescriptorList const &defaultSettings)
{
  for (size_t i = 0; i < defaultSettings.size(); ++i) {
    if (_settingsMap.count(defaultSettings[i].key) == 0) {
      _settingsMap[defaultSettings[i].key] = defaultSettings[i].defaultValue;
    }
  }
}

HdRenderParam *HdRenderDelegate::GetRenderParam() const
{
  return nullptr;
}

bool HdRenderDelegate::IsPauseSupported() const
{
  return false;
}

bool HdRenderDelegate::Pause()
{
  return false;
}

bool HdRenderDelegate::Resume()
{
  return false;
}

bool HdRenderDelegate::IsStopSupported() const
{
  return false;
}

bool HdRenderDelegate::Stop()
{
  return false;
}

bool HdRenderDelegate::Restart()
{
  return false;
}

WABI_NAMESPACE_END
