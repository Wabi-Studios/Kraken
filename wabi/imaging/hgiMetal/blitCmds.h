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
#ifndef WABI_IMAGING_HGI_METAL_BLIT_CMDS_H
#define WABI_IMAGING_HGI_METAL_BLIT_CMDS_H

#include "wabi/imaging/hgi/blitCmds.h"
#include "wabi/imaging/hgiMetal/api.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class HgiMetal;

/// \class HgiMetalBlitCmds
///
/// Metal implementation of HgiBlitCmds.
///
class HgiMetalBlitCmds final : public HgiBlitCmds {
 public:
  HGIMETAL_API
  ~HgiMetalBlitCmds() override;

  HGIMETAL_API
  void PushDebugGroup(const char *label) override;

  HGIMETAL_API
  void PopDebugGroup() override;

  HGIMETAL_API
  void CopyTextureGpuToCpu(HgiTextureGpuToCpuOp const &copyOp) override;

  HGIMETAL_API
  void CopyTextureCpuToGpu(HgiTextureCpuToGpuOp const &copyOp) override;

  HGIMETAL_API
  void CopyBufferGpuToGpu(HgiBufferGpuToGpuOp const &copyOp) override;

  HGIMETAL_API
  void CopyBufferCpuToGpu(HgiBufferCpuToGpuOp const &copyOp) override;

  HGIMETAL_API
  void CopyBufferGpuToCpu(HgiBufferGpuToCpuOp const &copyOp) override;

  HGIMETAL_API
  void CopyTextureToBuffer(HgiTextureToBufferOp const &copyOp) override;

  HGIMETAL_API
  void CopyBufferToTexture(HgiBufferToTextureOp const &copyOp) override;

  HGIMETAL_API
  void GenerateMipMaps(HgiTextureHandle const &texture) override;

  HGIMETAL_API
  void MemoryBarrier(HgiMemoryBarrier barrier) override;

 protected:
  friend class HgiMetal;

  HGIMETAL_API
  HgiMetalBlitCmds(HgiMetal *hgi);

  HGIMETAL_API
  bool _Submit(Hgi *hgi, HgiSubmitWaitType wait) override;

 private:
  HgiMetalBlitCmds() = delete;
  HgiMetalBlitCmds &operator=(const HgiMetalBlitCmds &) = delete;
  HgiMetalBlitCmds(const HgiMetalBlitCmds &) = delete;

  void _CreateEncoder();

  HgiMetal *_hgi;
  id<MTLCommandBuffer> _commandBuffer;
  id<MTLBlitCommandEncoder> _blitEncoder;
  NSString *_label;
  bool _secondaryCommandBuffer;

  // BlitCmds is used only one frame so storing multi-frame state on BlitCmds
  // will not survive.
};

WABI_NAMESPACE_END

#endif
