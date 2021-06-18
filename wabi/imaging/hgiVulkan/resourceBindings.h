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
#ifndef WABI_IMAGING_HGI_VULKAN_RESOURCEBINDINGS_H
#define WABI_IMAGING_HGI_VULKAN_RESOURCEBINDINGS_H

#include "wabi/imaging/hgi/resourceBindings.h"
#include "wabi/imaging/hgiVulkan/api.h"
#include "wabi/imaging/hgiVulkan/vulkan.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class HgiVulkanDevice;

///
/// \class HgiVulkanResourceBindings
///
/// Vulkan implementation of HgiResourceBindings.
///
///
class HgiVulkanResourceBindings final : public HgiResourceBindings
{
 public:
  HGIVULKAN_API
  ~HgiVulkanResourceBindings() override;

  /// Binds the resources to GPU.
  HGIVULKAN_API
  void BindResources(VkCommandBuffer cb, VkPipelineBindPoint bindPoint, VkPipelineLayout layout);

  /// Returns the device used to create this object.
  HGIVULKAN_API
  HgiVulkanDevice *GetDevice() const;

  /// Returns the (writable) inflight bits of when this object was trashed.
  HGIVULKAN_API
  uint64_t &GetInflightBits();

 protected:
  friend class HgiVulkan;

  HGIVULKAN_API
  HgiVulkanResourceBindings(HgiVulkanDevice *device, HgiResourceBindingsDesc const &desc);

 private:
  HgiVulkanResourceBindings() = delete;
  HgiVulkanResourceBindings &operator=(const HgiVulkanResourceBindings &) = delete;
  HgiVulkanResourceBindings(const HgiVulkanResourceBindings &) = delete;

  HgiVulkanDevice *_device;
  uint64_t _inflightBits;

  VkDescriptorPool _vkDescriptorPool;
  VkDescriptorSetLayout _vkDescriptorSetLayout;
  VkDescriptorSet _vkDescriptorSet;
};

WABI_NAMESPACE_END

#endif
