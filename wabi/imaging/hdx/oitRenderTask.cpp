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
#include "wabi/imaging/garch/glApi.h"

#include "wabi/imaging/hdx/oitBufferAccessor.h"
#include "wabi/imaging/hdx/oitRenderTask.h"
#include "wabi/imaging/hdx/package.h"

#include "wabi/imaging/hd/renderDelegate.h"
#include "wabi/imaging/hd/renderIndex.h"
#include "wabi/imaging/hd/rprimCollection.h"
#include "wabi/imaging/hd/sceneDelegate.h"

#include "wabi/imaging/hdPh/renderPassShader.h"

#include "wabi/imaging/glf/diagnostic.h"

WABI_NAMESPACE_BEGIN

HdxOitRenderTask::HdxOitRenderTask(HdSceneDelegate *delegate, SdfPath const &id)
  : HdxRenderTask(delegate, id),
    _oitTranslucentRenderPassShader(std::make_shared<HdPhRenderPassShader>(HdxPackageRenderPassOitShader())),
    _oitOpaqueRenderPassShader(
      std::make_shared<HdPhRenderPassShader>(HdxPackageRenderPassOitOpaqueShader())),
    _isOitEnabled(HdxOitBufferAccessor::IsOitEnabled())
{}

HdxOitRenderTask::~HdxOitRenderTask() = default;

void HdxOitRenderTask::_Sync(HdSceneDelegate *delegate, HdTaskContext *ctx, HdDirtyBits *dirtyBits)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  if (_isOitEnabled) {
    HdxRenderTask::_Sync(delegate, ctx, dirtyBits);
  }
}

void HdxOitRenderTask::Prepare(HdTaskContext *ctx, HdRenderIndex *renderIndex)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  if (_isOitEnabled) {
    HdxRenderTask::Prepare(ctx, renderIndex);

    // OIT buffers take up significant GPU resources. Skip if there are no
    // oit draw items (i.e. no translucent or volumetric draw items)
    if (HdxRenderTask::_HasDrawItems()) {
      HdxOitBufferAccessor(ctx).RequestOitBuffers();
    }
  }
}

void HdxOitRenderTask::Execute(HdTaskContext *ctx)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  GLF_GROUP_FUNCTION();

  if (!_isOitEnabled)
    return;
  if (!HdxRenderTask::_HasDrawItems())
    return;

  //
  // Pre Execute Setup
  //
  {
    HdxOitBufferAccessor oitBufferAccessor(ctx);

    oitBufferAccessor.RequestOitBuffers();
    oitBufferAccessor.InitializeOitBuffersIfNecessary();
    if (!oitBufferAccessor.AddOitBufferBindings(_oitTranslucentRenderPassShader)) {
      TF_CODING_ERROR("No OIT buffers allocated but needed by OIT render task");
      return;
    }
  }

  HdRenderPassStateSharedPtr renderPassState = _GetRenderPassState(ctx);
  if (!TF_VERIFY(renderPassState))
    return;

  HdPhRenderPassState *extendedState = dynamic_cast<HdPhRenderPassState *>(renderPassState.get());
  if (!TF_VERIFY(extendedState, "OIT only works with HdPh")) {
    return;
  }

  // Render pass state overrides
  {
    extendedState->SetUseSceneMaterials(true);
    // blending is relevant only for the oitResolve task.
    extendedState->SetBlendEnabled(false);
    extendedState->SetAlphaToCoverageEnabled(false);
    extendedState->SetAlphaThreshold(0.f);
  }

  // We render into a SSBO -- not MSSA compatible
  bool oldMSAA = glIsEnabled(GL_MULTISAMPLE);
  glDisable(GL_MULTISAMPLE);
  // XXX When rendering HdPhPoints we set GL_POINTS and assume that
  //     GL_POINT_SMOOTH is enabled by default. This renders circles instead
  //     of squares. However, when toggling MSAA off (above) we see GL_POINTS
  //     start to render squares (driver bug?).
  //     For now we always enable GL_POINT_SMOOTH.
  // XXX Switch points rendering to emit quad with FS that draws circle.
  bool oldPointSmooth = glIsEnabled(GL_POINT_SMOOTH);
  glEnable(GL_POINT_SMOOTH);

  // XXX HdxRenderTask::Prepare calls HdPhRenderPassState::Prepare.
  // This sets the cullStyle for the render pass shader.
  // Since Oit uses a custom render pass shader, we must manually
  // set cullStyle.
  _oitOpaqueRenderPassShader->SetCullStyle(extendedState->GetCullStyle());
  _oitTranslucentRenderPassShader->SetCullStyle(extendedState->GetCullStyle());

  //
  // 1. Opaque pixels pass
  //
  // Fragments that are opaque (alpha > 1.0) are rendered to the active
  // framebuffer. Translucent fragments are discarded.
  // This can reduce the data written to the OIT SSBO buffers because of
  // improved depth testing.
  //
  {
    extendedState->SetRenderPassShader(_oitOpaqueRenderPassShader);
    renderPassState->SetEnableDepthMask(true);
    renderPassState->SetColorMasks({HdRenderPassState::ColorMaskRGBA});

    HdxRenderTask::Execute(ctx);
  }

  //
  // 2. Translucent pixels pass
  //
  // Fill OIT SSBO buffers for the translucent fragments.
  //
  {
    extendedState->SetRenderPassShader(_oitTranslucentRenderPassShader);
    renderPassState->SetEnableDepthMask(false);
    renderPassState->SetColorMasks({HdRenderPassState::ColorMaskNone});
    HdxRenderTask::Execute(ctx);
  }

  //
  // Post Execute Restore
  //

  if (oldMSAA) {
    glEnable(GL_MULTISAMPLE);
  }

  if (!oldPointSmooth) {
    glDisable(GL_POINT_SMOOTH);
  }
}

WABI_NAMESPACE_END
