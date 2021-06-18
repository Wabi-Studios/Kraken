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
#ifndef WABI_IMAGING_HGIGL_DEVICE_H
#define WABI_IMAGING_HGIGL_DEVICE_H

#include "wabi/imaging/hgi/graphicsCmdsDesc.h"
#include "wabi/imaging/hgiGL/api.h"
#include "wabi/imaging/hgiGL/framebufferCache.h"
#include "wabi/imaging/hgiGL/hgi.h"
#include "wabi/wabi.h"

#include <fstream>
#include <ostream>

WABI_NAMESPACE_BEGIN

/// \class HgiGlDevice
///
/// OpenGL implementation of GPU device.
/// Note that there is only one global opengl context / device.
///
class HgiGLDevice final
{
 public:
  HGIGL_API
  HgiGLDevice();

  HGIGL_API
  ~HgiGLDevice();

  /// Get a framebuffer that matches the descriptor.
  /// Do not hold onto the framebuffer Id. Instead re-acquire it every frame.
  /// Framebuffer are internally managed in a framebuffer cache.
  ///
  /// When the cmds descriptor has resolved textures, two framebuffers are
  /// created for the MSAA and for the resolved textures. The bool flag can
  /// be used to access the respective ones.
  HGIGL_API
  uint32_t AcquireFramebuffer(HgiGraphicsCmdsDesc const &desc, bool resolved = false);

  /// Execute the provided functions / ops. This will emit the GL calls.
  HGIGL_API
  void SubmitOps(HgiGLOpsVector const &ops);

 private:
  HgiGLDevice &operator=(const HgiGLDevice &) = delete;
  HgiGLDevice(const HgiGLDevice &) = delete;

  friend std::ofstream &operator<<(std::ofstream &out, const HgiGLDevice &dev);

  HgiGLFramebufferCache _framebufferCache;
};

WABI_NAMESPACE_END

#endif
