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
#ifndef WABI_IMAGING_HGIGL_COMPUTE_PIPELINE_H
#define WABI_IMAGING_HGIGL_COMPUTE_PIPELINE_H

#include "wabi/wabi.h"
#include "wabi/imaging/hgi/computePipeline.h"
#include "wabi/imaging/hgiGL/api.h"


WABI_NAMESPACE_BEGIN


/// \class HgiGLComputePipeline
///
/// OpenGL implementation of HgiComputePipeline.
///
class HgiGLComputePipeline final : public HgiComputePipeline
{
 public:

  HGIGL_API
  ~HgiGLComputePipeline() override;

  /// Apply pipeline state
  HGIGL_API
  void BindPipeline();

 protected:

  friend class HgiGL;

  HGIGL_API
  HgiGLComputePipeline(HgiComputePipelineDesc const &desc);

 private:

  HgiGLComputePipeline() = delete;
  HgiGLComputePipeline &operator=(const HgiGLComputePipeline &) = delete;
  HgiGLComputePipeline(const HgiGLComputePipeline &) = delete;
};


WABI_NAMESPACE_END

#endif
