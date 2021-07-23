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

#include "wabi/imaging/hdx/oitResolveTask.h"
#include "wabi/imaging/hdx/package.h"
#include "wabi/imaging/hdx/tokens.h"

#include "wabi/imaging/hd/renderBuffer.h"
#include "wabi/imaging/hd/renderDelegate.h"
#include "wabi/imaging/hd/renderIndex.h"
#include "wabi/imaging/hd/renderPass.h"
#include "wabi/imaging/hd/rprimCollection.h"
#include "wabi/imaging/hd/sceneDelegate.h"
#include "wabi/imaging/hd/vtBufferSource.h"

#include "wabi/imaging/hdPh/imageShaderRenderPass.h"
#include "wabi/imaging/hdPh/renderDelegate.h"
#include "wabi/imaging/hdPh/renderPassShader.h"
#include "wabi/imaging/hdPh/renderPassState.h"
#include "wabi/imaging/hdPh/resourceRegistry.h"

#include "wabi/imaging/hdx/oitBufferAccessor.h"

#include "wabi/imaging/glf/contextCaps.h"
#include "wabi/imaging/glf/diagnostic.h"

WABI_NAMESPACE_BEGIN

using HdBufferSourceSharedPtrVector = std::vector<HdBufferSourceSharedPtr>;

static GfVec2i _GetScreenSize()
{
  // XXX Ideally we want screenSize to be passed in via the app.
  // (see Presto Stagecontext/TaskGraph), but for now we query this from GL.
  //
  // Using GL_VIEWPORT here (or viewport from RenderParams) is in-correct!
  //
  // The gl_FragCoord we use in the OIT shaders is relative to the FRAMEBUFFER
  // size (screen size), not the gl_viewport size.
  // We do various tricks with glViewport for Presto slate mode so we cannot
  // rely on it to determine the 'screenWidth' we need in the gl shaders.
  //
  // The CounterBuffer is especially fragile to this because in the glsl shdr
  // we calculate a 'screenIndex' based on gl_fragCoord that indexes into
  // the CounterBuffer. If we did not make enough room in the CounterBuffer
  // we may be reading/writing an invalid index into the CounterBuffer.
  //

  GLint attachType = 0;
  glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
                                        GL_COLOR_ATTACHMENT0,
                                        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                        &attachType);

  GLint attachId = 0;
  glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
                                        GL_COLOR_ATTACHMENT0,
                                        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                        &attachId);

  // XXX Fallback to gl viewport in case we do not find a non-default FBO for
  // bakends that do not attach a custom FB. This is in-correct, but gl does
  // not let us query size properties of default framebuffer. For this we
  // need the screenSize to be passed in via app (see note above)
  if (attachId <= 0)
  {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return GfVec2i(viewport[2], viewport[3]);
  }

  GlfContextCaps const &caps = GlfContextCaps::GetInstance();

  if (ARCH_LIKELY(caps.directStateAccessEnabled))
  {
    if (attachType == GL_TEXTURE)
    {
      GLint w, h;
      glGetTextureLevelParameteriv(attachId, 0, GL_TEXTURE_WIDTH, &w);
      glGetTextureLevelParameteriv(attachId, 0, GL_TEXTURE_HEIGHT, &h);
      return GfVec2i(w, h);
    }

    if (attachType == GL_RENDERBUFFER)
    {
      GLint w, h;
      glGetNamedRenderbufferParameteriv(attachId, GL_RENDERBUFFER_WIDTH, &w);
      glGetNamedRenderbufferParameteriv(attachId, GL_RENDERBUFFER_HEIGHT, &h);
      return GfVec2i(w, h);
    }
  } else
  {
    if (attachType == GL_TEXTURE)
    {
      GLint w, h;
      GLint oldBinding;
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);
      glBindTexture(GL_TEXTURE_2D, attachId);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
      glBindTexture(GL_TEXTURE_2D, oldBinding);
      return GfVec2i(w, h);
    }

    if (attachType == GL_RENDERBUFFER)
    {
      GLint w, h;
      GLint oldBinding;
      glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldBinding);
      glBindRenderbuffer(GL_RENDERBUFFER, attachId);
      glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w);
      glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h);
      glBindRenderbuffer(GL_RENDERBUFFER, oldBinding);
      return GfVec2i(w, h);
    }
  }

  constexpr int oitScreenSizeFallback = 2048;

  return GfVec2i(oitScreenSizeFallback, oitScreenSizeFallback);
}

HdxOitResolveTask::HdxOitResolveTask(HdSceneDelegate *delegate, SdfPath const &id)
  : HdTask(id),
    _screenSize(0)
{}

HdxOitResolveTask::~HdxOitResolveTask() = default;

void HdxOitResolveTask::Sync(HdSceneDelegate *delegate, HdTaskContext *ctx, HdDirtyBits *dirtyBits)
{
  HD_TRACE_FUNCTION();
  *dirtyBits = HdChangeTracker::Clean;
}

const HdRenderPassAovBindingVector &HdxOitResolveTask::_GetAovBindings(HdTaskContext *const ctx) const
{
  static HdRenderPassAovBindingVector empty;

  if (!_HasTaskContextData(ctx, HdxTokens->renderPassState))
  {
    return empty;
  }

  HdRenderPassStateSharedPtr renderPassState;
  _GetTaskContextData(ctx, HdxTokens->renderPassState, &renderPassState);
  if (!renderPassState)
  {
    return empty;
  }

  return renderPassState->GetAovBindings();
}

GfVec2i HdxOitResolveTask::_ComputeScreenSize(HdTaskContext *ctx, HdRenderIndex *renderIndex) const
{
  const HdRenderPassAovBindingVector &aovBindings = _GetAovBindings(ctx);
  if (aovBindings.empty())
  {
    return _GetScreenSize();
  }

  const SdfPath &bufferId = aovBindings.front().renderBufferId;
  HdRenderBuffer *const buffer = static_cast<HdRenderBuffer *>(
    renderIndex->GetBprim(HdPrimTypeTokens->renderBuffer, bufferId));
  if (!buffer)
  {
    TF_CODING_ERROR("No render buffer at path %s specified in AOV bindings", bufferId.GetText());
    return _GetScreenSize();
  }

  return GfVec2i(buffer->GetWidth(), buffer->GetHeight());
}

void HdxOitResolveTask::_PrepareOitBuffers(HdTaskContext *ctx,
                                           HdRenderIndex *renderIndex,
                                           GfVec2i const &screenSize)
{
  static const int numSamples = 8;  // Should match glslfx files

  if (!(screenSize[0] >= 0 && screenSize[1] >= 0))
  {
    TF_CODING_ERROR("Invalid screen size for OIT resolve task %s", GetId().GetText());
    return;
  }

  HdPhResourceRegistrySharedPtr const &hdPhResourceRegistry = std::static_pointer_cast<HdPhResourceRegistry>(
    renderIndex->GetResourceRegistry());

  const bool createOitBuffers = !_counterBar;
  if (createOitBuffers)
  {
    //
    // Counter Buffer
    //
    HdBufferSpecVector counterSpecs;
    counterSpecs.push_back(HdBufferSpec(HdxTokens->hdxOitCounterBuffer, HdTupleType{HdTypeInt32, 1}));
    _counterBar = hdPhResourceRegistry->AllocateSingleBufferArrayRange(
      /*role*/ HdxTokens->oitCounter,
      counterSpecs,
      HdBufferArrayUsageHint());
    //
    // Index Buffer
    //
    HdBufferSpecVector indexSpecs;
    indexSpecs.push_back(HdBufferSpec(HdxTokens->hdxOitIndexBuffer, HdTupleType{HdTypeInt32, 1}));
    _indexBar = hdPhResourceRegistry->AllocateSingleBufferArrayRange(
      /*role*/ HdxTokens->oitIndices,
      indexSpecs,
      HdBufferArrayUsageHint());

    //
    // Data Buffer
    //
    HdBufferSpecVector dataSpecs;
    dataSpecs.push_back(HdBufferSpec(HdxTokens->hdxOitDataBuffer, HdTupleType{HdTypeFloatVec4, 1}));
    _dataBar = hdPhResourceRegistry->AllocateSingleBufferArrayRange(
      /*role*/ HdxTokens->oitData,
      dataSpecs,
      HdBufferArrayUsageHint());

    //
    // Depth Buffer
    //
    HdBufferSpecVector depthSpecs;
    depthSpecs.push_back(HdBufferSpec(HdxTokens->hdxOitDepthBuffer, HdTupleType{HdTypeFloat, 1}));
    _depthBar = hdPhResourceRegistry->AllocateSingleBufferArrayRange(
      /*role*/ HdxTokens->oitDepth,
      depthSpecs,
      HdBufferArrayUsageHint());

    //
    // Uniforms
    //
    HdBufferSpecVector uniformSpecs;
    uniformSpecs.push_back(HdBufferSpec(HdxTokens->oitScreenSize, HdTupleType{HdTypeInt32Vec2, 1}));

    _uniformBar = hdPhResourceRegistry->AllocateUniformBufferArrayRange(
      /*role*/ HdxTokens->oitUniforms,
      uniformSpecs,
      HdBufferArrayUsageHint());
  }

  // Make sure task context has our buffer each frame (in case its cleared)
  (*ctx)[HdxTokens->oitCounterBufferBar] = _counterBar;
  (*ctx)[HdxTokens->oitIndexBufferBar] = _indexBar;
  (*ctx)[HdxTokens->oitDataBufferBar] = _dataBar;
  (*ctx)[HdxTokens->oitDepthBufferBar] = _depthBar;
  (*ctx)[HdxTokens->oitUniformBar] = _uniformBar;

  // The OIT buffer are sized based on the size of the screen and use
  // fragCoord to index into the buffers.
  // We must update uniform screenSize when either X or Y increases in size.
  const bool resizeOitBuffers = (screenSize[0] > _screenSize[0] || screenSize[1] > _screenSize[1]);

  if (resizeOitBuffers)
  {
    _screenSize = screenSize;
    const int newBufferSize = screenSize[0] * screenSize[1];

    // +1 because element 0 of the counter buffer is used as an atomic
    // counter in the shader to give each fragment a unique index.
    _counterBar->Resize(newBufferSize + 1);
    _indexBar->Resize(newBufferSize * numSamples);
    _dataBar->Resize(newBufferSize * numSamples);
    _depthBar->Resize(newBufferSize * numSamples);

    // Update the values in the uniform buffer
    hdPhResourceRegistry->AddSource(
      _uniformBar,
      std::make_shared<HdVtBufferSource>(HdxTokens->oitScreenSize, VtValue(screenSize)));
  }
}

void HdxOitResolveTask::Prepare(HdTaskContext *ctx, HdRenderIndex *renderIndex)
{
  // Only allocate/resize buffer if a render task requested it.
  if (ctx->find(HdxTokens->oitRequestFlag) == ctx->end())
  {
    // Deallocate buffers here?
    return;
  }

  // The HdTaskContext might not be cleared between two engine execute
  // iterations, so we explicitly delete the cleared flag here so that the
  // execute of the first oit render task will clear the buffer in this
  // iteration.
  ctx->erase(HdxTokens->oitClearedFlag);

  if (!_renderPass)
  {
    HdRprimCollection collection;
    HdRenderDelegate *renderDelegate = renderIndex->GetRenderDelegate();

    if (!TF_VERIFY(dynamic_cast<HdPhRenderDelegate *>(renderDelegate), "OIT Task only works with HdPh"))
    {
      return;
    }

    _renderPass = std::make_shared<HdPh_ImageShaderRenderPass>(renderIndex, collection);

    // We do not use renderDelegate->CreateRenderPassState because
    // ImageShaders always use HdPh
    _renderPassState = std::make_shared<HdPhRenderPassState>();
    _renderPassState->SetEnableDepthTest(false);
    _renderPassState->SetEnableDepthMask(false);
    _renderPassState->SetAlphaThreshold(0.0f);
    _renderPassState->SetAlphaToCoverageEnabled(false);
    _renderPassState->SetColorMasks({HdRenderPassState::ColorMaskRGBA});
    _renderPassState->SetBlendEnabled(true);
    // We expect pre-multiplied color as input into the OIT resolve shader
    // e.g. vec4(rgb * a, a). Hence the src factor for rgb is "One" since
    // src alpha is already accounted for.
    // Alpha's are blended with the same blending equation as the rgb's.
    // Thinking about it conceptually, if you're looking through two glass
    // windows both occluding 50% of light, some light would still be
    // passing through. 50% of light passes through the first window, then
    // 50% of the remaining light through the second window. Hence the
    // equation: 0.5 + 0.5 * (1 - 0.5) = 0.75, as 75% of light is occluded.
    _renderPassState->SetBlend(HdBlendOp::HdBlendOpAdd,
                               HdBlendFactor::HdBlendFactorOne,
                               HdBlendFactor::HdBlendFactorOneMinusSrcAlpha,
                               HdBlendOp::HdBlendOpAdd,
                               HdBlendFactor::HdBlendFactorOne,
                               HdBlendFactor::HdBlendFactorOneMinusSrcAlpha);

    _renderPassShader = std::make_shared<HdPhRenderPassShader>(HdxPackageOitResolveImageShader());
    _renderPassState->SetRenderPassShader(_renderPassShader);

    _renderPass->Prepare(GetRenderTags());
  }

  _PrepareOitBuffers(ctx, renderIndex, _ComputeScreenSize(ctx, renderIndex));
}

void HdxOitResolveTask::Execute(HdTaskContext *ctx)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  GLF_GROUP_FUNCTION();

  // Check whether the request flag was set and delete it so that for the
  // next iteration the request flag is not set unless an OIT render task
  // explicitly sets it.
  if (ctx->erase(HdxTokens->oitRequestFlag) == 0)
  {
    return;
  }

  // Explicitly erase clear flag so that it can be re-used by subsequent
  // OIT render and resolve tasks.
  ctx->erase(HdxTokens->oitClearedFlag);

  if (!TF_VERIFY(_renderPassState))
    return;
  if (!TF_VERIFY(_renderPassShader))
    return;

  _renderPassState->SetAovBindings(_GetAovBindings(ctx));

  HdxOitBufferAccessor oitBufferAccessor(ctx);
  if (!oitBufferAccessor.AddOitBufferBindings(_renderPassShader))
  {
    TF_CODING_ERROR("No OIT buffers allocated but needed by OIT resolve task");
    return;
  }

  _renderPass->Execute(_renderPassState, GetRenderTags());
}

WABI_NAMESPACE_END
