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
#ifndef WABI_IMAGING_HGI_VULKAN_SAMPLER_H
#define WABI_IMAGING_HGI_VULKAN_SAMPLER_H

#include "wabi/imaging/hgi/sampler.h"
#include "wabi/imaging/hgiVulkan/api.h"
#include "wabi/imaging/hgiVulkan/vulkan.h"


WABI_NAMESPACE_BEGIN

class HgiVulkanDevice;


///
/// \class HgiVulkanSampler
///
/// Vulkan implementation of HgiSampler
///
class HgiVulkanSampler final : public HgiSampler
{
 public:

  HGIVULKAN_API
  ~HgiVulkanSampler() override;

  HGIVULKAN_API
  uint64_t GetRawResource() const override;

  /// Returns the vulkan sampler object.
  HGIVULKAN_API
  VkSampler GetVulkanSampler() const;

  /// Returns the device used to create this object.
  HGIVULKAN_API
  HgiVulkanDevice *GetDevice() const;

  /// Returns the (writable) inflight bits of when this object was trashed.
  HGIVULKAN_API
  uint64_t &GetInflightBits();

 protected:

  friend class HgiVulkan;

  HGIVULKAN_API
  HgiVulkanSampler(HgiVulkanDevice *device, HgiSamplerDesc const &desc);

 private:

  HgiVulkanSampler() = delete;
  HgiVulkanSampler &operator=(const HgiVulkanSampler &) = delete;
  HgiVulkanSampler(const HgiVulkanSampler &) = delete;

  VkSampler _vkSampler;

  HgiVulkanDevice *_device;
  uint64_t _inflightBits;
};


WABI_NAMESPACE_END

#endif