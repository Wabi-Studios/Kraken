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
#ifndef WABI_IMAGING_HGIINTEROP_HGIINTEROPVULKAN_H
#define WABI_IMAGING_HGIINTEROP_HGIINTEROPVULKAN_H

#include "wabi/base/gf/vec4i.h"
#include "wabi/imaging/hgi/texture.h"
#include "wabi/imaging/hgiInterop/api.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class HgiVulkan;
class VtValue;

/// \class HgiInteropVulkan
///
/// Provides Vulkan/GL interop.
///
class HgiInteropVulkan final {
 public:
  HGIINTEROP_API
  HgiInteropVulkan(Hgi *hgiVulkan);

  HGIINTEROP_API
  ~HgiInteropVulkan();

  /// Composite provided color (and optional depth) textures over app's
  /// framebuffer contents.
  HGIINTEROP_API
  void CompositeToInterop(HgiTextureHandle const &color,
                          HgiTextureHandle const &depth,
                          VtValue const &framebuffer,
                          GfVec4i const &viewport);

 private:
  HgiInteropVulkan() = delete;

  HgiVulkan *_hgiVulkan;
  uint32_t _vs;
  uint32_t _fsNoDepth;
  uint32_t _fsDepth;
  uint32_t _prgNoDepth;
  uint32_t _prgDepth;
  uint32_t _vertexBuffer;

  // XXX We tmp copy Vulkan's GPU texture to CPU and then to GL texture
  // Once we share GPU memory between Vulkan and GL we can remove this.
  uint32_t _glColorTex;
  uint32_t _glDepthTex;
};

WABI_NAMESPACE_END

#endif
