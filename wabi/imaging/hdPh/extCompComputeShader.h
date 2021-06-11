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
#ifndef WABI_IMAGING_HD_ST_EXT_COMP_COMPUTE_SHADER_H
#define WABI_IMAGING_HD_ST_EXT_COMP_COMPUTE_SHADER_H

#include "wabi/imaging/hd/bufferSource.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/imaging/hdPh/api.h"
#include "wabi/imaging/hdPh/shaderCode.h"
#include "wabi/wabi.h"

#include "wabi/usd/sdf/path.h"

#include "wabi/base/tf/token.h"
#include "wabi/base/vt/value.h"

#include <string>
#include <vector>

WABI_NAMESPACE_BEGIN

using HdPh_ExtCompComputeShaderSharedPtr = std::shared_ptr<class HdPh_ExtCompComputeShader>;

class HdExtComputation;

/// \class HdPh_ExtCompComputeShader
///
/// An internal representation of a compute shader in Phoenix that allows the
/// use of the code generation and resource binding system to generate a
/// shader program.
///
class HdPh_ExtCompComputeShader final : public HdPhShaderCode {
 public:
  HDPH_API
  HdPh_ExtCompComputeShader(HdExtComputation const *extComp);

  HDPH_API
  ~HdPh_ExtCompComputeShader() override;

  // ---------------------------------------------------------------------- //
  /// \name HdPhShaderCode (pure) virtual interface                         //
  // ---------------------------------------------------------------------- //

  HDPH_API
  std::string GetSource(TfToken const &shaderStageKey) const override;

  HDPH_API
  void BindResources(int program,
                     HdPh_ResourceBinder const &binder,
                     HdRenderPassState const &state) override;

  HDPH_API
  void UnbindResources(int program,
                       HdPh_ResourceBinder const &binder,
                       HdRenderPassState const &state) override;

  HDPH_API
  void AddBindings(HdBindingRequestVector *customBindings) override;

  HDPH_API
  ID ComputeHash() const override;

  // ---------------------------------------------------------------------- //

  HDPH_API
  SdfPath const &GetExtComputationId() const;

 private:
  // No copying
  HdPh_ExtCompComputeShader(const HdPh_ExtCompComputeShader &) = delete;
  HdPh_ExtCompComputeShader &operator=(const HdPh_ExtCompComputeShader &) = delete;

  HdExtComputation const *const _extComp;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_ST_EXT_COMP_COMPUTE_SHADER_H
