//
// Copyright 2020 Pixar
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
#ifndef EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_HDX_PRMAN_RESOURCE_REGISTRY_H
#define EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_HDX_PRMAN_RESOURCE_REGISTRY_H

#include "context.h"
#include "wabi/imaging/hd/resourceRegistry.h"
#include "wabi/imaging/plugin/hdPrman/api.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

/// HdxPrman's implementation of the hydra resource registry.
/// Renderman manages its resources internally, but uses the HdResourceRegistry
/// to respond to certain resource changes, such as texture reloading.
class HdxPrman_ResourceRegistry final : public HdResourceRegistry
{
 public:

  HDPRMAN_API
  HdxPrman_ResourceRegistry(std::shared_ptr<HdxPrman_InteractiveContext> const &context);

  HDPRMAN_API
  virtual ~HdxPrman_ResourceRegistry();

  HDPRMAN_API
  void ReloadResource(TfToken const &resourceType, std::string const &path) override;

 private:

  std::shared_ptr<HdxPrman_InteractiveContext> _context;
};

WABI_NAMESPACE_END

#endif
