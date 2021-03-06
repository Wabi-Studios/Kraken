/************************************************************************
Copyright 2020 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#ifndef RPRUSD_MATERIAL_NODES_RPR_MATERIALX_NODE_H
#define RPRUSD_MATERIAL_NODES_RPR_MATERIALX_NODE_H

#include "wabi/base/tf/staticTokens.h"
#include "wabi/imaging/rprUsd/api.h"

WABI_NAMESPACE_BEGIN

#define RPRUSD_RPR_MATERIALX_NODE_TOKENS \
  (rpr_materialx_node)(file)(string)(basePath)(surfaceElement)(displacementElement)(stPrimvarName)

TF_DECLARE_PUBLIC_TOKENS(RprUsdRprMaterialXNodeTokens,
                         RPRUSD_API,
                         RPRUSD_RPR_MATERIALX_NODE_TOKENS);

WABI_NAMESPACE_END

#endif  // RPRUSD_MATERIAL_NODES_RPR_MATERIALX_NODE_H
