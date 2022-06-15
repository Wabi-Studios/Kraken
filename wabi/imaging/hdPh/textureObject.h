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
#ifndef WABI_IMAGING_HD_ST_TEXTURE_OBJECT_H
#define WABI_IMAGING_HD_ST_TEXTURE_OBJECT_H

#include "wabi/imaging/hdPh/api.h"
#include "wabi/wabi.h"

#include "wabi/imaging/hd/enums.h"
#include "wabi/imaging/hd/types.h"
#include "wabi/imaging/hdPh/textureIdentifier.h"

#include "wabi/imaging/hgi/handle.h"
#include "wabi/imaging/hio/image.h"

#include "wabi/base/gf/bbox3d.h"
#include "wabi/base/tf/declarePtrs.h"

#include <memory>

WABI_NAMESPACE_BEGIN

class Hgi;
using HgiTextureHandle = HgiHandle<class HgiTexture>;
class HdPh_TextureObjectRegistry;
struct HgiTextureDesc;
class HdPhTextureCpuData;
class HdPhResourceRegistry;

using HdPhTextureObjectSharedPtr = std::shared_ptr<class HdPhTextureObject>;

/// \class HdPhTextureObject
///
/// Base class for a texture object. The actual GPU resources will be
/// allocated during the commit phase.
///
class HdPhTextureObject : public std::enable_shared_from_this<HdPhTextureObject>
{
 public:

  /// Get texture identifier
  ///
  const HdPhTextureIdentifier &GetTextureIdentifier() const
  {
    return _textureId;
  }

  /// Get the target memory for the texture.
  ///
  size_t GetTargetMemory() const
  {
    return _targetMemory;
  }

  /// Set the target memory (in bytes).
  ///
  /// When uploading the texture to the GPU, it will be downsampled
  /// to meet this target memory.
  ///
  HDPH_API
  void SetTargetMemory(size_t);

  /// Is texture valid? Only correct after commit phase.
  ///
  /// E.g., no file at given file path. Consulted by clients to
  /// determine whether to use the fallback value.
  ///
  HDPH_API
  virtual bool IsValid() const = 0;

  /// Get texture type
  ///
  HDPH_API
  virtual HdTextureType GetTextureType() const = 0;

  HDPH_API
  virtual ~HdPhTextureObject();

 protected:

  HdPhTextureObject(const HdPhTextureIdentifier &textureId,
                    HdPh_TextureObjectRegistry *textureObjectRegistry);

  HDPH_API
  HdPhResourceRegistry *_GetResourceRegistry() const;

  HDPH_API
  Hgi *_GetHgi() const;

  HDPH_API
  std::string _GetDebugName(const HdPhTextureIdentifier &textureId) const;

  HDPH_API
  bool _GetPremultiplyAlpha(const HdPhSubtextureIdentifier *const subId) const;

  HDPH_API
  HioImage::SourceColorSpace _GetSourceColorSpace(
    const HdPhSubtextureIdentifier *const subId) const;

  /// Load texture to CPU (thread-safe)
  ///
  HDPH_API
  virtual void _Load() = 0;

  /// Commit texture to GPU (not thread-safe)
  ///
  HDPH_API
  virtual void _Commit() = 0;

  /// Add signed number to total texture memory amount maintained by
  /// registry.
  ///
  HDPH_API
  void _AdjustTotalTextureMemory(int64_t memDiff);

  /// Compute memory of texture and add to total texture memory
  /// amount maintained by registry.
  HDPH_API
  void _AddToTotalTextureMemory(const HgiTextureHandle &texture);

  /// Compute memory of texture and subtract to total texture memory
  /// amount maintained by registry.
  HDPH_API
  void _SubtractFromTotalTextureMemory(const HgiTextureHandle &texture);

 private:

  friend class HdPh_TextureObjectRegistry;

  HdPh_TextureObjectRegistry *const _textureObjectRegistry;
  const HdPhTextureIdentifier _textureId;
  size_t _targetMemory;
};

/// \class HdPhUvTextureObject
///
/// A base class for uv textures.
///
class HdPhUvTextureObject : public HdPhTextureObject
{
 public:

  ~HdPhUvTextureObject() override;

  /// Get the handle to the actual GPU resource.
  ///
  /// Only valid after commit phase.
  ///
  HgiTextureHandle const &GetTexture() const
  {
    return _gpuTexture;
  }

  /// Opinion about wrapS and wrapT parameters from the texture file.
  ///
  /// Only valid after commit phase. Can be HdWrapNoOpinion.
  const std::pair<HdWrap, HdWrap> &GetWrapParameters() const
  {
    return _wrapParameters;
  }

  HDPH_API
  HdTextureType GetTextureType() const override final;

 protected:

  HdPhUvTextureObject(const HdPhTextureIdentifier &textureId,
                      HdPh_TextureObjectRegistry *textureObjectRegistry);

  void _SetWrapParameters(const std::pair<HdWrap, HdWrap> &wrapParameters);

  void _SetCpuData(std::unique_ptr<HdPhTextureCpuData> &&);
  HdPhTextureCpuData *_GetCpuData() const;

  void _CreateTexture(const HgiTextureDesc &desc);
  void _GenerateMipmaps();
  void _DestroyTexture();

 private:

  std::pair<HdWrap, HdWrap> _wrapParameters;
  std::unique_ptr<HdPhTextureCpuData> _cpuData;
  HgiTextureHandle _gpuTexture;
};

/// \class HdAssetStUvTextureObject
///
/// A uv texture loading the asset identified by the texture identifier.
///
class HdPhAssetUvTextureObject final : public HdPhUvTextureObject
{
 public:

  HDPH_API
  HdPhAssetUvTextureObject(const HdPhTextureIdentifier &textureId,
                           HdPh_TextureObjectRegistry *textureObjectRegistry);

  HDPH_API
  ~HdPhAssetUvTextureObject() override;

  HDPH_API
  bool IsValid() const override;

 protected:

  HDPH_API
  void _Load() override;

  HDPH_API
  void _Commit() override;
};

/// \class HdPhFieldTextureObject
///
/// A uvw texture with a bounding box describing how to transform it.
///
class HdPhFieldTextureObject final : public HdPhTextureObject
{
 public:

  HDPH_API
  HdPhFieldTextureObject(const HdPhTextureIdentifier &textureId,
                         HdPh_TextureObjectRegistry *textureObjectRegistry);

  HDPH_API
  ~HdPhFieldTextureObject() override;

  /// Get the handle to the actual GPU resource.
  ///
  /// Only valid after commit phase.
  ///
  HgiTextureHandle const &GetTexture() const
  {
    return _gpuTexture;
  }

  /// The box the texture fills out.
  ///
  /// Only valid after the commit phase.
  ///
  const GfBBox3d &GetBoundingBox() const
  {
    return _bbox;
  }

  /// The sampling transform.
  ///
  /// Only valid after the commit phase.
  ///
  const GfMatrix4d &GetSamplingTransform() const
  {
    return _samplingTransform;
  }

  HDPH_API
  bool IsValid() const override;

  HDPH_API
  HdTextureType GetTextureType() const override;

 protected:

  HDPH_API
  void _Load() override;

  HDPH_API
  void _Commit() override;

 private:

  std::unique_ptr<HdPhTextureCpuData> _cpuData;
  GfBBox3d _bbox;
  GfMatrix4d _samplingTransform;
  HgiTextureHandle _gpuTexture;
};

template<HdTextureType textureType> struct HdPh_TypedTextureObjectHelper;

/// \class HdPhTypedTextureObject
///
/// A template alias such that, e.g., HdPhUvTextureObject can be
/// accessed as HdPhTypedTextureObject<HdTextureType::Uv>.
///
template<HdTextureType textureType>
using HdPhTypedTextureObject = typename HdPh_TypedTextureObjectHelper<textureType>::type;

template<> struct HdPh_TypedTextureObjectHelper<HdTextureType::Uv>
{
  using type = HdPhUvTextureObject;
};

template<> struct HdPh_TypedTextureObjectHelper<HdTextureType::Field>
{
  using type = HdPhFieldTextureObject;
};

WABI_NAMESPACE_END

#endif
