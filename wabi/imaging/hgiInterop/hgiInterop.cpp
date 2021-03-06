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
#include "wabi/imaging/hgiInterop/hgiInterop.h"
#include "wabi/imaging/hgi/hgi.h"
#include "wabi/imaging/hgi/tokens.h"


#if defined(WITH_METAL)
#  include "wabi/imaging/hgiMetal/hgi.h"
#  include "wabi/imaging/hgiInterop/metal.h"
#elif defined(WITH_VULKAN)
#  include "wabi/imaging/hgiVulkan/hgi.h"
#  include "wabi/imaging/hgiInterop/vulkan.h"
#else
#  include "wabi/imaging/hgiInterop/opengl.h"
#endif

WABI_NAMESPACE_BEGIN

HgiInterop::HgiInterop() = default;

HgiInterop::~HgiInterop() = default;

void HgiInterop::TransferToApp(Hgi *srcHgi,
                               HgiTextureHandle const &srcColor,
                               HgiTextureHandle const &srcDepth,
                               TfToken const &dstApi,
                               VtValue const &dstFramebuffer,
                               GfVec4i const &dstRegion)
{
  TfToken const &srcApi = srcHgi->GetAPIName();

#if defined(WITH_METAL)
  if (srcApi == HgiTokens->Metal && dstApi == HgiTokens->OpenGL) {
    // Transfer Metal textures to OpenGL application
    if (!_metalToOpenGL) {
      _metalToOpenGL = std::make_unique<HgiInteropMetal>(srcHgi);
    }
    _metalToOpenGL->CompositeToInterop(srcColor, srcDepth, dstFramebuffer, dstRegion);
  } else {
    TF_CODING_ERROR("Unsupported Hgi backend: %s", srcApi.GetText());
  }
#elif defined(WITH_VULKAN)
  if (srcApi == HgiTokens->Vulkan && dstApi == HgiTokens->OpenGL) {
    // Transfer Vulkan textures to OpenGL application
    if (!_vulkanToOpenGL) {
      _vulkanToOpenGL = std::make_unique<HgiInteropVulkan>(srcHgi);
    }
    _vulkanToOpenGL->CompositeToInterop(srcColor, srcDepth, dstFramebuffer, dstRegion);
  } else {
    TF_CODING_ERROR("Unsupported Hgi backend: %s", srcApi.GetText());
  }
#else
  if (srcApi == HgiTokens->OpenGL && dstApi == HgiTokens->OpenGL) {
    // Transfer OpenGL textures to OpenGL application
    if (!_openGLToOpenGL) {
      _openGLToOpenGL = std::make_unique<HgiInteropOpenGL>();
    }
    _openGLToOpenGL->CompositeToInterop(srcColor, srcDepth, dstFramebuffer, dstRegion);
  } else {
    TF_CODING_ERROR("Unsupported Hgi backend: %s", srcApi.GetText());
  }
#endif
}

WABI_NAMESPACE_END
