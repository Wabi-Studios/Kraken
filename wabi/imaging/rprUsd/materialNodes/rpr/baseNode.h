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

#ifndef RPRUSD_MATERIAL_NODES_RPR_BASE_NODE_H
#define RPRUSD_MATERIAL_NODES_RPR_BASE_NODE_H

#include "../materialNode.h"

#include "wabi/imaging/rprUsd/materialRegistry.h"

#include <RadeonProRender.hpp>

#include <memory>

WABI_NAMESPACE_BEGIN

/// \class RprUsd_BaseRuntimeNode
///
/// The node output of which is always rpr::MaterialNode*.
class RprUsd_BaseRuntimeNode : public RprUsd_MaterialNode {
 public:
  RprUsd_BaseRuntimeNode(rpr::MaterialNodeType type, RprUsd_MaterialBuilderContext *ctx);

  ~RprUsd_BaseRuntimeNode() override = default;

  bool SetInput(TfToken const &inputId, VtValue const &value) override;
  bool SetInput(rpr::MaterialNodeInput input, VtValue const &value);

  VtValue GetOutput(TfToken const &outputId) override;

 protected:
  rpr::MaterialNodeType m_type;
  RprUsd_MaterialBuilderContext *m_ctx;
  std::shared_ptr<rpr::MaterialNode> m_rprNode;
};

WABI_NAMESPACE_END

#endif  // RPRUSD_MATERIAL_NODES_RPR_BASE_NODE_H
