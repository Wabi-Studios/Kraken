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
#include "wabi/imaging/hdx/oitBufferAccessor.h"

#include "wabi/imaging/hdSt/bufferArrayRange.h"
#include "wabi/imaging/hdSt/bufferResource.h"
#include "wabi/imaging/hdSt/renderPassShader.h"

// XXX todo tmp needed until we remove direct gl calls below.
#include "wabi/imaging/hgiGL/buffer.h"

#include "wabi/imaging/hdx/tokens.h"

#include "wabi/base/tf/envSetting.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_ENV_SETTING(HDX_ENABLE_OIT, true, 
                      "Enable order independent translucency");

/* static */
bool HdxOitBufferAccessor::IsOitEnabled()
{
  return TfGetEnvSetting(HDX_ENABLE_OIT);
}

HdxOitBufferAccessor::HdxOitBufferAccessor(HdTaskContext *ctx) : _ctx(ctx) {}

void HdxOitBufferAccessor::RequestOitBuffers()
{
  (*_ctx)[HdxTokens->oitRequestFlag] = VtValue(true);
}

HdBufferArrayRangeSharedPtr const &HdxOitBufferAccessor::_GetBar(const TfToken &name)
{
  const auto it = _ctx->find(name);
  if (it == _ctx->end()) {
    static HdBufferArrayRangeSharedPtr n;
    return n;
  }

  const VtValue &v = it->second;
  return v.Get<HdBufferArrayRangeSharedPtr>();
}

bool HdxOitBufferAccessor::AddOitBufferBindings(const HdStRenderPassShaderSharedPtr &shader)
{
  HdBufferArrayRangeSharedPtr const &counterBar = _GetBar(HdxTokens->oitCounterBufferBar);
  HdBufferArrayRangeSharedPtr const &dataBar = _GetBar(HdxTokens->oitDataBufferBar);
  HdBufferArrayRangeSharedPtr const &depthBar = _GetBar(HdxTokens->oitDepthBufferBar);
  HdBufferArrayRangeSharedPtr const &indexBar = _GetBar(HdxTokens->oitIndexBufferBar);
  HdBufferArrayRangeSharedPtr const &uniformBar = _GetBar(HdxTokens->oitUniformBar);

  if (counterBar && dataBar && depthBar && indexBar && uniformBar) {
    shader->AddBufferBinding(HdBindingRequest(HdBinding::SSBO,
                                              HdxTokens->oitCounterBufferBar,
                                              counterBar,
                                              /*interleave = */ false,
                                              /*writable = */ true));

    shader->AddBufferBinding(HdBindingRequest(HdBinding::SSBO,
                                              HdxTokens->oitDataBufferBar,
                                              dataBar,
                                              /*interleave = */ false,
                                              /*writable = */ true));

    shader->AddBufferBinding(HdBindingRequest(HdBinding::SSBO,
                                              HdxTokens->oitDepthBufferBar,
                                              depthBar,
                                              /*interleave = */ false,
                                              /*writable = */ true));

    shader->AddBufferBinding(HdBindingRequest(HdBinding::SSBO,
                                              HdxTokens->oitIndexBufferBar,
                                              indexBar,
                                              /*interleave = */ false,
                                              /*writable = */ true));

    shader->AddBufferBinding(HdBindingRequest(HdBinding::UBO,
                                              HdxTokens->oitUniformBar,
                                              uniformBar,
                                              /*interleave = */ true));
    return true;
  } else {
    shader->RemoveBufferBinding(HdxTokens->oitCounterBufferBar);
    shader->RemoveBufferBinding(HdxTokens->oitDataBufferBar);
    shader->RemoveBufferBinding(HdxTokens->oitDepthBufferBar);
    shader->RemoveBufferBinding(HdxTokens->oitIndexBufferBar);
    shader->RemoveBufferBinding(HdxTokens->oitUniformBar);
    return false;
  }
}

void HdxOitBufferAccessor::InitializeOitBuffersIfNecessary(Hgi *hgi)
{
  // If the OIT buffers were already cleared earlier, skip and do not
  // clear them again.
  VtValue &clearFlag = (*_ctx)[HdxTokens->oitClearedFlag];
  if (!clearFlag.IsEmpty()) {
    return;
  }

  // Mark OIT buffers as cleared.
  clearFlag = true;

  // Clear counter buffer.

  // The shader determines what elements in each buffer are used based on
  // finding -1 in the counter buffer. We can skip clearing the other buffers.

  HdStBufferArrayRangeSharedPtr stCounterBar = std::dynamic_pointer_cast<HdStBufferArrayRange>(
    _GetBar(HdxTokens->oitCounterBufferBar));

  if (!stCounterBar) {
    TF_CODING_ERROR("No OIT counter buffer allocateed when trying to clear it");
    return;
  }

  HdStBufferResourceSharedPtr stCounterResource = stCounterBar->GetResource(
    HdxTokens->hdxOitCounterBuffer);

  // We want to fill the buffer with int -1 but the FillBuffer interface
  // supports uint8_t (due to a limitation in the Metal API which we can later
  // revisit to find a workaround). A buffer filled with uint8_t 0xff is the
  // same as a buffer filled with int 0xffffffff.
  const uint8_t clearCounter = -1;

  HgiBlitCmdsUniquePtr blitCmds = hgi->CreateBlitCmds();
  blitCmds->PushDebugGroup("Clear OIT buffers");
  blitCmds->FillBuffer(stCounterResource->GetHandle(), clearCounter);
  blitCmds->PopDebugGroup();
  hgi->SubmitCmds(blitCmds.get());
}

WABI_NAMESPACE_END
