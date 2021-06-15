//
// Copyright 2019 Pixar
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
#ifndef WABI_IMAGING_HDX_OIT_VOLUME_RENDER_TASK_H
#define WABI_IMAGING_HDX_OIT_VOLUME_RENDER_TASK_H

#include "wabi/imaging/hdx/api.h"
#include "wabi/imaging/hdx/renderTask.h"
#include "wabi/imaging/hdx/version.h"
#include "wabi/wabi.h"

#include "wabi/imaging/hdPh/renderPassState.h"

WABI_NAMESPACE_BEGIN

/// \class HdxOitVolumeRenderTask
///
/// A task for rendering transparent geometry into OIT buffers.
/// Its companion task, OITResolveTask, will blend the buffers to screen.
///
class HdxOitVolumeRenderTask : public HdxRenderTask {
 public:
  HDX_API
  HdxOitVolumeRenderTask(HdSceneDelegate *delegate, SdfPath const &id);

  HDX_API
  ~HdxOitVolumeRenderTask() override;

  /// Prepare the tasks resources
  HDX_API
  void Prepare(HdTaskContext *ctx, HdRenderIndex *renderIndex) override;

  /// Execute render pass task
  HDX_API
  void Execute(HdTaskContext *ctx) override;

 protected:
  /// Sync the render pass resources
  HDX_API
  void _Sync(HdSceneDelegate *delegate, HdTaskContext *ctx, HdDirtyBits *dirtyBits) override;

 private:
  HdxOitVolumeRenderTask() = delete;
  HdxOitVolumeRenderTask(const HdxOitVolumeRenderTask &) = delete;
  HdxOitVolumeRenderTask &operator=(const HdxOitVolumeRenderTask &) = delete;

  HdPhRenderPassShaderSharedPtr _oitVolumeRenderPassShader;
  const bool _isOitEnabled;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HDX_OIT_VOLUME_RENDER_TASK_H
