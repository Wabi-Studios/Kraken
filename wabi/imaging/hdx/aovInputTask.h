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
#ifndef WABI_IMAGING_HDX_AOV_INPUT_TASK_H
#define WABI_IMAGING_HDX_AOV_INPUT_TASK_H

#include "wabi/imaging/hdx/api.h"
#include "wabi/imaging/hdx/task.h"
#include "wabi/imaging/hgi/texture.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

/// \class HdxAovInputTask
///
/// A task for taking input AOV data comming from a render buffer that was
/// filled by render tasks and converting it to a HgiTexture.
/// The aov render buffer can be a GPU or CPU buffer, while the resulting output
/// HgiTexture will always be a GPU texture.
///
/// The HgiTexture is placed in the shared task context so that following tasks
/// maybe operate on this HgiTexture without having to worry about converting
/// the aov data from CPU to GPU.
///
class HdxAovInputTask : public HdxTask {
 public:
  HDX_API
  HdxAovInputTask(HdSceneDelegate *delegate, SdfPath const &id);

  HDX_API
  ~HdxAovInputTask() override;

  /// Hooks for progressive rendering.
  bool IsConverged() const override;

  HDX_API
  void Prepare(HdTaskContext *ctx, HdRenderIndex *renderIndex) override;

  HDX_API
  void Execute(HdTaskContext *ctx) override;

 protected:
  HDX_API
  void _Sync(HdSceneDelegate *delegate, HdTaskContext *ctx, HdDirtyBits *dirtyBits) override;

 private:
  void _UpdateTexture(HdTaskContext *ctx, HgiTextureHandle &texture, HdRenderBuffer *buffer);

  void _UpdateIntermediateTexture(HgiTextureHandle &texture, HdRenderBuffer *buffer);

  bool _converged;

  SdfPath _aovBufferPath;
  SdfPath _depthBufferPath;

  HdRenderBuffer *_aovBuffer;
  HdRenderBuffer *_depthBuffer;

  HgiTextureHandle _aovTexture;
  HgiTextureHandle _depthTexture;
  HgiTextureHandle _aovTextureIntermediate;

  HdxAovInputTask()                        = delete;
  HdxAovInputTask(const HdxAovInputTask &) = delete;
  HdxAovInputTask &operator=(const HdxAovInputTask &) = delete;
};

/// \class HdxAovInputTaskParams
///
/// AovInput parameters.
///
struct HdxAovInputTaskParams {
  HdxAovInputTaskParams() : aovBufferPath(), depthBufferPath()
  {}

  SdfPath aovBufferPath;
  SdfPath depthBufferPath;
};

// VtValue requirements
HDX_API
std::ostream &operator<<(std::ostream &out, const HdxAovInputTaskParams &pv);
HDX_API
bool operator==(const HdxAovInputTaskParams &lhs, const HdxAovInputTaskParams &rhs);
HDX_API
bool operator!=(const HdxAovInputTaskParams &lhs, const HdxAovInputTaskParams &rhs);

WABI_NAMESPACE_END

#endif
