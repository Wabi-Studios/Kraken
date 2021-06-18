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
#ifndef WABI_IMAGING_HGIVULKAN_SHADERPROGRAM_H
#define WABI_IMAGING_HGIVULKAN_SHADERPROGRAM_H

#include <vector>

#include "wabi/imaging/hgi/shaderProgram.h"

#include "wabi/imaging/hgiVulkan/api.h"
#include "wabi/imaging/hgiVulkan/shaderFunction.h"

WABI_NAMESPACE_BEGIN

class HgiVulkanDevice;

///
/// \class HgiVulkanShaderProgram
///
/// Vulkan implementation of HgiShaderProgram
///
class HgiVulkanShaderProgram final : public HgiShaderProgram
{
 public:
  HGIVULKAN_API
  ~HgiVulkanShaderProgram() override = default;

  HGIVULKAN_API
  bool IsValid() const override;

  HGIVULKAN_API
  std::string const &GetCompileErrors() override;

  HGIVULKAN_API
  size_t GetByteSizeOfResource() const override;

  HGIVULKAN_API
  uint64_t GetRawResource() const override;

  /// Returns the shader functions that are part of this program.
  HGIVULKAN_API
  HgiShaderFunctionHandleVector const &GetShaderFunctions() const;

  /// Returns the device used to create this object.
  HGIVULKAN_API
  HgiVulkanDevice *GetDevice() const;

  /// Returns the (writable) inflight bits of when this object was trashed.
  HGIVULKAN_API
  uint64_t &GetInflightBits();

 protected:
  friend class HgiVulkan;

  HGIVULKAN_API
  HgiVulkanShaderProgram(HgiVulkanDevice *device, HgiShaderProgramDesc const &desc);

 private:
  HgiVulkanShaderProgram() = delete;
  HgiVulkanShaderProgram &operator=(const HgiVulkanShaderProgram &) = delete;
  HgiVulkanShaderProgram(const HgiVulkanShaderProgram &) = delete;

  HgiVulkanDevice *_device;
  uint64_t _inflightBits;
};

WABI_NAMESPACE_END

#endif