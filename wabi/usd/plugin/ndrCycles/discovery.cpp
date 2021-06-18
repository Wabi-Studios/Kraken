//  Copyright 2020 Tangent Animation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
//  including without limitation, as related to merchantability and fitness
//  for a particular purpose.
//
//  In no event shall any copyright holder be liable for any damages of any kind
//  arising from the use of this software, whether in contract, tort or otherwise.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "discovery.h"

#include <wabi/base/arch/fileSystem.h>
#include <wabi/base/tf/envSetting.h>
#include <wabi/base/tf/getenv.h>
#include <wabi/base/tf/staticTokens.h>
#include <wabi/base/tf/stringUtils.h>
#include <wabi/usd/usd/prim.h>
#include <wabi/usd/usd/primRange.h>

WABI_NAMESPACE_BEGIN

// clang-format off
TF_DEFINE_PRIVATE_TOKENS(_tokens,
    (shader)
    (cycles)
    (ccl_filename)
);
// clang-format on

NDR_REGISTER_DISCOVERY_PLUGIN(NdrCyclesDiscoveryPlugin)

NdrCyclesDiscoveryPlugin::NdrCyclesDiscoveryPlugin()
{}

NdrCyclesDiscoveryPlugin::~NdrCyclesDiscoveryPlugin()
{}

NdrNodeDiscoveryResultVec NdrCyclesDiscoveryPlugin::DiscoverNodes(const Context &context)
{
  TfToken ccl_filename("<built-in>");
  NdrNodeDiscoveryResultVec ret;

  // TODO: Store these in proper USD Schema and read at runtime...
  std::vector<std::string> temp_nodes;
  temp_nodes.push_back("output");
  temp_nodes.push_back("diffuse_bsdf");
  temp_nodes.push_back("principled_bsdf");
  temp_nodes.push_back("glossy_bsdf");
  temp_nodes.push_back("principled_hair_bsdf");
  temp_nodes.push_back("anisotropic_bsdf");
  temp_nodes.push_back("glass_bsdf");
  temp_nodes.push_back("refraction_bsdf");
  temp_nodes.push_back("toon_bsdf");
  temp_nodes.push_back("velvet_bsdf");
  temp_nodes.push_back("translucent_bsdf");
  temp_nodes.push_back("transparent_bsdf");
  temp_nodes.push_back("subsurface_scattering");
  temp_nodes.push_back("mix_closure");
  temp_nodes.push_back("add_closure");
  temp_nodes.push_back("hair_bsdf");
  temp_nodes.push_back("principled_volume");
  temp_nodes.push_back("scatter_volume");
  temp_nodes.push_back("absorption_volume");
  temp_nodes.push_back("emission");
  temp_nodes.push_back("displacement");

  for (const std::string &n : temp_nodes)
  {
    std::string cycles_id = "cycles_" + n;
    ret.emplace_back(NdrIdentifier(TfStringPrintf("%s", cycles_id.c_str())),  // identifier
                     NdrVersion(1, 0),                                        // version
                     n.c_str(),                                               // name
                     _tokens->shader,                                         // family
                     _tokens->cycles,                                         // discoveryType
                     _tokens->cycles,                                         // sourceType
                     ccl_filename,                                            // uri
                     ccl_filename                                             // resolvedUri
    );

    // DEPRECATED:
    // Added for backwards support whilst we transition to new identifier
    cycles_id = "cycles:" + n;
    ret.emplace_back(NdrIdentifier(TfStringPrintf("%s", cycles_id.c_str())),  // identifier
                     NdrVersion(1, 0),                                        // version
                     n.c_str(),                                               // name
                     _tokens->shader,                                         // family
                     _tokens->cycles,                                         // discoveryType
                     _tokens->cycles,                                         // sourceType
                     ccl_filename,                                            // uri
                     ccl_filename                                             // resolvedUri
    );
  }

  return ret;
}

const NdrStringVec &NdrCyclesDiscoveryPlugin::GetSearchURIs() const
{
  static const auto result = []() -> NdrStringVec {
    NdrStringVec ret = TfStringSplit(TfGetenv("CYCLES_PLUGIN_PATH"), ARCH_PATH_LIST_SEP);
    ret.push_back("<built-in>");
    return ret;
  }();
  return result;
}

WABI_NAMESPACE_END