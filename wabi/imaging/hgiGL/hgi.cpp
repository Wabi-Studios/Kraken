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
#include "wabi/imaging/garch/glApi.h"

#include "wabi/imaging/hgi/handle.h"
#include "wabi/imaging/hgiGL/blitCmds.h"
#include "wabi/imaging/hgiGL/buffer.h"
#include "wabi/imaging/hgiGL/computeCmds.h"
#include "wabi/imaging/hgiGL/computePipeline.h"
#include "wabi/imaging/hgiGL/conversions.h"
#include "wabi/imaging/hgiGL/device.h"
#include "wabi/imaging/hgiGL/diagnostic.h"
#include "wabi/imaging/hgiGL/graphicsCmds.h"
#include "wabi/imaging/hgiGL/graphicsPipeline.h"
#include "wabi/imaging/hgiGL/hgi.h"
#include "wabi/imaging/hgiGL/resourceBindings.h"
#include "wabi/imaging/hgiGL/sampler.h"
#include "wabi/imaging/hgiGL/shaderFunction.h"
#include "wabi/imaging/hgiGL/shaderProgram.h"
#include "wabi/imaging/hgiGL/texture.h"

#include "wabi/base/tf/envSetting.h"
#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/type.h"

#include <mutex>

WABI_NAMESPACE_BEGIN

TF_DEFINE_ENV_SETTING(HGIGL_ENABLE_GL_VERSION_VALIDATION, true, "Enables validation OpenGL version.");

TF_REGISTRY_FUNCTION(TfType)
{
  TfType t = TfType::Define<HgiGL, TfType::Bases<Hgi>>();
  t.SetFactory<HgiFactory<HgiGL>>();
}

HgiGL::HgiGL() : _device(nullptr), _garbageCollector(this), _frameDepth(0)
{
  static std::once_flag versionOnce;
  std::call_once(versionOnce, []() {
    const bool validate = TfGetEnvSetting(HGIGL_ENABLE_GL_VERSION_VALIDATION);
    GarchGLApiLoad();
    if (validate && !HgiGLMeetsMinimumRequirements()) {
      TF_WARN(
        "HgiGL minimum OpenGL requirements not met. Please ensure "
        "that OpenGL is initialized and supports version 4.5.");
    }
  });

  // Create "primary device" (note there is only one for GL)
  _device = new HgiGLDevice();
}

HgiGL::~HgiGL()
{
  _garbageCollector.PerformGarbageCollection();
  delete _device;
}

HgiGLDevice *HgiGL::GetPrimaryDevice() const
{
  return _device;
}

HgiGraphicsCmdsUniquePtr HgiGL::CreateGraphicsCmds(HgiGraphicsCmdsDesc const &desc)
{
  HgiGLGraphicsCmds *cmds(new HgiGLGraphicsCmds(_device, desc));
  return HgiGraphicsCmdsUniquePtr(cmds);
}

HgiBlitCmdsUniquePtr HgiGL::CreateBlitCmds()
{
  return HgiBlitCmdsUniquePtr(new HgiGLBlitCmds());
}

HgiComputeCmdsUniquePtr HgiGL::CreateComputeCmds()
{
  HgiGLComputeCmds *cmds(new HgiGLComputeCmds(_device));
  return HgiComputeCmdsUniquePtr(cmds);
}

HgiTextureHandle HgiGL::CreateTexture(HgiTextureDesc const &desc)
{
  return HgiTextureHandle(new HgiGLTexture(desc), GetUniqueId());
}

void HgiGL::DestroyTexture(HgiTextureHandle *texHandle)
{
  _TrashObject(texHandle, _garbageCollector.GetTextureList());
}

HgiTextureViewHandle HgiGL::CreateTextureView(HgiTextureViewDesc const &desc)
{
  if (!desc.sourceTexture) {
    TF_CODING_ERROR("Source texture is null");
  }

  HgiTextureHandle src = HgiTextureHandle(new HgiGLTexture(desc), GetUniqueId());
  HgiTextureView *view = new HgiTextureView(desc);
  view->SetViewTexture(src);
  return HgiTextureViewHandle(view, GetUniqueId());
}

void HgiGL::DestroyTextureView(HgiTextureViewHandle *viewHandle)
{
  // Trash the texture inside the view and invalidate the view handle.
  HgiTextureHandle texHandle = (*viewHandle)->GetViewTexture();
  _TrashObject(&texHandle, _garbageCollector.GetTextureList());
  (*viewHandle)->SetViewTexture(HgiTextureHandle());
  delete viewHandle->Get();
  *viewHandle = HgiTextureViewHandle();
}

HgiSamplerHandle HgiGL::CreateSampler(HgiSamplerDesc const &desc)
{
  return HgiSamplerHandle(new HgiGLSampler(desc), GetUniqueId());
}

void HgiGL::DestroySampler(HgiSamplerHandle *smpHandle)
{
  _TrashObject(smpHandle, _garbageCollector.GetSamplerList());
}

HgiBufferHandle HgiGL::CreateBuffer(HgiBufferDesc const &desc)
{
  return HgiBufferHandle(new HgiGLBuffer(desc), GetUniqueId());
}

void HgiGL::DestroyBuffer(HgiBufferHandle *bufHandle)
{
  _TrashObject(bufHandle, _garbageCollector.GetBufferList());
}

HgiShaderFunctionHandle HgiGL::CreateShaderFunction(HgiShaderFunctionDesc const &desc)
{
  return HgiShaderFunctionHandle(new HgiGLShaderFunction(desc), GetUniqueId());
}

void HgiGL::DestroyShaderFunction(HgiShaderFunctionHandle *shaderFunctionHandle)
{
  _TrashObject(shaderFunctionHandle, _garbageCollector.GetShaderFunctionList());
}

HgiShaderProgramHandle HgiGL::CreateShaderProgram(HgiShaderProgramDesc const &desc)
{
  return HgiShaderProgramHandle(new HgiGLShaderProgram(desc), GetUniqueId());
}

void HgiGL::DestroyShaderProgram(HgiShaderProgramHandle *shaderProgramHandle)
{
  _TrashObject(shaderProgramHandle, _garbageCollector.GetShaderProgramList());
}

HgiResourceBindingsHandle HgiGL::CreateResourceBindings(HgiResourceBindingsDesc const &desc)
{
  return HgiResourceBindingsHandle(new HgiGLResourceBindings(desc), GetUniqueId());
}

void HgiGL::DestroyResourceBindings(HgiResourceBindingsHandle *resHandle)
{
  _TrashObject(resHandle, _garbageCollector.GetResourceBindingsList());
}

HgiGraphicsPipelineHandle HgiGL::CreateGraphicsPipeline(HgiGraphicsPipelineDesc const &desc)
{
  return HgiGraphicsPipelineHandle(new HgiGLGraphicsPipeline(desc), GetUniqueId());
}

void HgiGL::DestroyGraphicsPipeline(HgiGraphicsPipelineHandle *pipeHandle)
{
  _TrashObject(pipeHandle, _garbageCollector.GetGraphicsPipelineList());
}

HgiComputePipelineHandle HgiGL::CreateComputePipeline(HgiComputePipelineDesc const &desc)
{
  return HgiComputePipelineHandle(new HgiGLComputePipeline(desc), GetUniqueId());
}

void HgiGL::DestroyComputePipeline(HgiComputePipelineHandle *pipeHandle)
{
  _TrashObject(pipeHandle, _garbageCollector.GetComputePipelineList());
}

TfToken const &HgiGL::GetAPIName() const
{
  return HgiTokens->OpenGL;
}

void HgiGL::StartFrame()
{
  // Protect against client calling StartFrame more than once (nested engines)
  if (_frameDepth++ == 0) {
// Start Full Frame debug label
#if defined(GL_KHR_debug)
    if (GARCH_GLAPI_HAS(KHR_debug)) {
      glPushDebugGroup(GL_DEBUG_SOURCE_THIRD_PARTY, 0, -1, "Full Hydra Frame");
    }
#endif
  }
}

void HgiGL::EndFrame()
{
  if (--_frameDepth == 0) {
    _garbageCollector.PerformGarbageCollection();

// End Full Frame debug label
#if defined(GL_KHR_debug)
    if (GARCH_GLAPI_HAS(KHR_debug)) {
      glPopDebugGroup();
    }
#endif
  }
}

bool HgiGL::_SubmitCmds(HgiCmds *cmds, HgiSubmitWaitType wait)
{
  bool result = Hgi::_SubmitCmds(cmds, wait);

  if (wait == HgiSubmitWaitTypeWaitUntilCompleted) {
    // CPU - GPU synchronization (stall) by client request only.
    static const uint64_t timeOut = 100000000000;

    GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLenum status = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeOut);

    if (status != GL_ALREADY_SIGNALED && status != GL_CONDITION_SATISFIED) {
      // We could loop, but we don't expect to timeout.
      TF_RUNTIME_ERROR("Unexpected ClientWaitSync timeout");
    }

    glDeleteSync(fence);
  }

  // If the Hgi client does not call Hgi::EndFrame we garbage collect here.
  if (_frameDepth == 0) {
    _garbageCollector.PerformGarbageCollection();
  }

  return result;
}

WABI_NAMESPACE_END
