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
#ifndef WABI_IMAGING_HD_ST_DYNAMIC_UV_TEXTURE_OBJECT_H
#define WABI_IMAGING_HD_ST_DYNAMIC_UV_TEXTURE_OBJECT_H

#include "wabi/imaging/hdPh/api.h"
#include "wabi/wabi.h"

#include "wabi/imaging/hdPh/textureObject.h"

WABI_NAMESPACE_BEGIN

class HdPhDynamicUvTextureImplementation;

/// \class HdPhDynamicUvTextureObject
///
/// A uv texture that is managed but not populated by the Phoenix texture system.
///
/// Clients can allocate an instance of this class through
/// HdPhResourceRegistry::AllocateTextureHandle or AllocateTextureObject
/// by passing an HdPhTextureIdentifier with a
/// HdPhDynamicUvSubtextureIdentifier.
///
/// The client can allocate the GPU resource with CreateTexture and populate it
/// by either giving data in the HgiTextureDesc or binding the texture as target
/// for a computation or render.
///
/// Bindless texture sampler handles can only be created correctly if
/// a client has created the texture before the texture commit phase
/// is finished.
///
class HdPhDynamicUvTextureObject final : public HdPhUvTextureObject
{
 public:

  HDPH_API
  HdPhDynamicUvTextureObject(const HdPhTextureIdentifier &textureId,
                             HdPh_TextureObjectRegistry *textureObjectRegistry);

  HDPH_API
  ~HdPhDynamicUvTextureObject() override;

  /// Allocate GPU resource using the texture descriptor. Populate
  /// if data are given in the descriptor.
  ///
  void CreateTexture(const HgiTextureDesc &desc)
  {
    _CreateTexture(desc);
  }

  /// Make GPU generate mipmaps.
  ///
  void GenerateMipmaps()
  {
    _GenerateMipmaps();
  }

  /// Release GPU resource.
  void DestroyTexture()
  {
    _DestroyTexture();
  }

  /// Set wrap mode hints used when a texture node's wrap mode
  /// is use metadata.
  ///
  /// This is typically called from HdPhDynamicUvTextureImplementation::Load
  /// when the texture file has wrap mode hints.
  void SetWrapParameters(const std::pair<HdWrap, HdWrap> &wrapParameters)
  {
    _SetWrapParameters(wrapParameters);
  }

  /// Save CPU data for this texture (transfering ownership).
  ///
  /// This is typically called from HdPhDynamicUvTextureImplementation::Load
  /// so that the CPU data can be uploaded during commit.
  ///
  /// To free the CPU data, call with nullptr.
  ///
  void SetCpuData(std::unique_ptr<HdPhTextureCpuData> &&cpuData)
  {
    _SetCpuData(std::move(cpuData));
  }

  /// Get the CPU data stored for this texture.
  ///
  /// Typically used in HdPhDynamicUvTextureImplementation::Commit to
  /// commit CPU data to GPU.
  HdPhTextureCpuData *GetCpuData() const
  {
    return _GetCpuData();
  }

  /// Always returns true - so that samplers for this texture are
  /// created.
  ///
  HDPH_API
  bool IsValid() const override;

 protected:

  HDPH_API
  void _Load() override;

  HDPH_API
  void _Commit() override;

 private:

  HdPhDynamicUvTextureImplementation *_GetImpl() const;
};

WABI_NAMESPACE_END

#endif
