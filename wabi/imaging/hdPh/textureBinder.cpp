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
#include "wabi/imaging/garch/glApi.h"
#include "wabi/wabi.h"

#include "wabi/imaging/hd/vtBufferSource.h"
#include "wabi/imaging/hdPh/ptexTextureObject.h"
#include "wabi/imaging/hdPh/resourceBinder.h"
#include "wabi/imaging/hdPh/samplerObject.h"
#include "wabi/imaging/hdPh/textureBinder.h"
#include "wabi/imaging/hdPh/textureHandle.h"
#include "wabi/imaging/hdPh/textureObject.h"
#include "wabi/imaging/hdPh/udimTextureObject.h"
#include "wabi/imaging/hgiGL/sampler.h"
#include "wabi/imaging/hgiGL/texture.h"

WABI_NAMESPACE_BEGIN

static const HdTupleType _bindlessHandleTupleType{HdTypeUInt32Vec2, 1};

static TfToken _Concat(const TfToken &a, const TfToken &b)
{
  return TfToken(a.GetString() + b.GetString());
}

void HdPh_TextureBinder::GetBufferSpecs(const NamedTextureHandleVector &textures,
                                        const bool useBindlessHandles,
                                        HdBufferSpecVector *const specs)
{
  for (const NamedTextureHandle &texture : textures) {
    switch (texture.type) {
      case HdTextureType::Uv:
        if (useBindlessHandles) {
          specs->emplace_back(texture.name, _bindlessHandleTupleType);
        }
        else {
          specs->emplace_back(_Concat(texture.name, HdPh_ResourceBindingSuffixTokens->valid),
                              HdTupleType{HdTypeBool, 1});
        }
        break;
      case HdTextureType::Field:
        if (useBindlessHandles) {
          specs->emplace_back(texture.name, _bindlessHandleTupleType);
        }
        else {
          specs->emplace_back(_Concat(texture.name, HdPh_ResourceBindingSuffixTokens->valid),
                              HdTupleType{HdTypeBool, 1});
        }
        specs->emplace_back(
            _Concat(texture.name, HdPh_ResourceBindingSuffixTokens->samplingTransform),
            HdTupleType{HdTypeDoubleMat4, 1});
        break;
      case HdTextureType::Ptex:
        if (useBindlessHandles) {
          specs->emplace_back(texture.name, _bindlessHandleTupleType);
          specs->emplace_back(_Concat(texture.name, HdPh_ResourceBindingSuffixTokens->layout),
                              _bindlessHandleTupleType);
        }
        break;
      case HdTextureType::Udim:
        if (useBindlessHandles) {
          specs->emplace_back(texture.name, _bindlessHandleTupleType);
          specs->emplace_back(_Concat(texture.name, HdPh_ResourceBindingSuffixTokens->layout),
                              _bindlessHandleTupleType);
        }
        break;
    }
  }
}

namespace {

// A bindless GL sampler buffer.
// This identifies a texture as a 64-bit handle, passed to GLSL as "uvec2".
// See https://www.khronos.org/opengl/wiki/Bindless_Texture
class HdPh_BindlessSamplerBufferSource : public HdBufferSource {
 public:
  HdPh_BindlessSamplerBufferSource(TfToken const &name, const GLuint64EXT value)
      : HdBufferSource(),
        _name(name),
        _value(value)
  {}

  ~HdPh_BindlessSamplerBufferSource() override = default;

  TfToken const &GetName() const override
  {
    return _name;
  }
  void const *GetData() const override
  {
    return &_value;
  }
  HdTupleType GetTupleType() const override
  {
    return _bindlessHandleTupleType;
  }
  size_t GetNumElements() const override
  {
    return 1;
  }
  void GetBufferSpecs(HdBufferSpecVector *specs) const override
  {
    specs->emplace_back(_name, GetTupleType());
  }
  bool Resolve() override
  {
    if (!_TryLock())
      return false;
    _SetResolved();
    return true;
  }

 protected:
  bool _CheckValid() const override
  {
    return true;
  }

 private:
  const TfToken _name;
  const GLuint64EXT _value;
};

class _ComputeBufferSourcesFunctor {
 public:
  static void Compute(TfToken const &name,
                      HdPhUvTextureObject const &texture,
                      HdPhUvSamplerObject const &sampler,
                      const bool useBindlessHandles,
                      HdBufferSourceSharedPtrVector *const sources)
  {
    if (useBindlessHandles) {
      sources->push_back(std::make_shared<HdPh_BindlessSamplerBufferSource>(
          name, sampler.GetGLTextureSamplerHandle()));
    }
    else {
      sources->push_back(std::make_shared<HdVtBufferSource>(
          _Concat(name, HdPh_ResourceBindingSuffixTokens->valid), VtValue(texture.IsValid())));
    }
  }

  static void Compute(TfToken const &name,
                      HdPhFieldTextureObject const &texture,
                      HdPhFieldSamplerObject const &sampler,
                      const bool useBindlessHandles,
                      HdBufferSourceSharedPtrVector *const sources)
  {
    sources->push_back(std::make_shared<HdVtBufferSource>(
        _Concat(name, HdPh_ResourceBindingSuffixTokens->samplingTransform),
        VtValue(texture.GetSamplingTransform())));

    if (useBindlessHandles) {
      sources->push_back(std::make_shared<HdPh_BindlessSamplerBufferSource>(
          name, sampler.GetGLTextureSamplerHandle()));
    }
    else {
      sources->push_back(std::make_shared<HdVtBufferSource>(
          _Concat(name, HdPh_ResourceBindingSuffixTokens->valid), VtValue(texture.IsValid())));
    }
  }

  static void Compute(TfToken const &name,
                      HdPhPtexTextureObject const &texture,
                      HdPhPtexSamplerObject const &sampler,
                      const bool useBindlessHandles,
                      HdBufferSourceSharedPtrVector *const sources)
  {
    if (!useBindlessHandles) {
      return;
    }

    sources->push_back(std::make_shared<HdPh_BindlessSamplerBufferSource>(
        name, sampler.GetTexelsGLTextureHandle()));

    sources->push_back(std::make_shared<HdPh_BindlessSamplerBufferSource>(
        _Concat(name, HdPh_ResourceBindingSuffixTokens->layout),
        sampler.GetLayoutGLTextureHandle()));
  }

  static void Compute(TfToken const &name,
                      HdPhUdimTextureObject const &texture,
                      HdPhUdimSamplerObject const &sampler,
                      const bool useBindlessHandles,
                      HdBufferSourceSharedPtrVector *const sources)
  {
    if (!useBindlessHandles) {
      return;
    }

    sources->push_back(std::make_shared<HdPh_BindlessSamplerBufferSource>(
        name, sampler.GetTexelsGLTextureHandle()));

    sources->push_back(std::make_shared<HdPh_BindlessSamplerBufferSource>(
        _Concat(name, HdPh_ResourceBindingSuffixTokens->layout),
        sampler.GetLayoutGLTextureHandle()));
  }
};

void _BindTexture(const GLenum target,
                  HgiTextureHandle const &textureHandle,
                  HgiSamplerHandle const &samplerHandle,
                  const TfToken &name,
                  HdPh_ResourceBinder const &binder,
                  const bool bind)
{
  const HdBinding binding = binder.GetBinding(name);
  const int samplerUnit   = binding.GetTextureUnit();

  glActiveTexture(GL_TEXTURE0 + samplerUnit);

  const HgiTexture *const tex     = textureHandle.Get();
  const HgiGLTexture *const glTex = dynamic_cast<const HgiGLTexture *>(tex);

  if (tex && !glTex) {
    TF_CODING_ERROR("Phoenix texture binder only supports OpenGL");
  }

  const GLuint texName = (bind && glTex) ? glTex->GetTextureId() : 0;
  glBindTexture(target, texName);

  const HgiSampler *const sampler     = samplerHandle.Get();
  const HgiGLSampler *const glSampler = dynamic_cast<const HgiGLSampler *>(sampler);

  if (sampler && !glSampler) {
    TF_CODING_ERROR("Phoenix texture binder only supports OpenGL");
  }

  const GLuint samplerName = (bind && glSampler) ? glSampler->GetSamplerId() : 0;
  glBindSampler(samplerUnit, samplerName);
}

class _BindFunctor {
 public:
  static void Compute(TfToken const &name,
                      HdPhUvTextureObject const &texture,
                      HdPhUvSamplerObject const &sampler,
                      HdPh_ResourceBinder const &binder,
                      const bool bind)
  {
    _BindTexture(GL_TEXTURE_2D, texture.GetTexture(), sampler.GetSampler(), name, binder, bind);
  }

  static void Compute(TfToken const &name,
                      HdPhFieldTextureObject const &texture,
                      HdPhFieldSamplerObject const &sampler,
                      HdPh_ResourceBinder const &binder,
                      const bool bind)
  {
    _BindTexture(GL_TEXTURE_3D, texture.GetTexture(), sampler.GetSampler(), name, binder, bind);
  }

  static void Compute(TfToken const &name,
                      HdPhPtexTextureObject const &texture,
                      HdPhPtexSamplerObject const &sampler,
                      HdPh_ResourceBinder const &binder,
                      const bool bind)
  {
    const HdBinding texelBinding = binder.GetBinding(name);
    const int texelSamplerUnit   = texelBinding.GetTextureUnit();

    glActiveTexture(GL_TEXTURE0 + texelSamplerUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, bind ? texture.GetTexelTexture()->GetRawResource() : 0);

    HgiSampler *const texelSampler = sampler.GetTexelsSampler().Get();

    const HgiGLSampler *const glSampler = bind ? dynamic_cast<HgiGLSampler *>(texelSampler) :
                                                 nullptr;

    if (glSampler) {
      glBindSampler(texelSamplerUnit, (GLuint)glSampler->GetSamplerId());
    }
    else {
      glBindSampler(texelSamplerUnit, 0);
    }

    const HdBinding layoutBinding = binder.GetBinding(
        _Concat(name, HdPh_ResourceBindingSuffixTokens->layout));
    const int layoutSamplerUnit = layoutBinding.GetTextureUnit();

    glActiveTexture(GL_TEXTURE0 + layoutSamplerUnit);
    glBindTexture(GL_TEXTURE_1D_ARRAY, bind ? texture.GetLayoutTexture()->GetRawResource() : 0);
  }

  static void Compute(TfToken const &name,
                      HdPhUdimTextureObject const &texture,
                      HdPhUdimSamplerObject const &sampler,
                      HdPh_ResourceBinder const &binder,
                      const bool bind)
  {
    const HdBinding texelBinding = binder.GetBinding(name);
    const int texelSamplerUnit   = texelBinding.GetTextureUnit();

    glActiveTexture(GL_TEXTURE0 + texelSamplerUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, bind ? texture.GetTexelTexture()->GetRawResource() : 0);

    HgiSampler *const texelSampler = sampler.GetTexelsSampler().Get();

    const HgiGLSampler *const glSampler = bind ? dynamic_cast<HgiGLSampler *>(texelSampler) :
                                                 nullptr;

    if (glSampler) {
      glBindSampler(texelSamplerUnit, (GLuint)glSampler->GetSamplerId());
    }
    else {
      glBindSampler(texelSamplerUnit, 0);
    }

    const HdBinding layoutBinding = binder.GetBinding(
        _Concat(name, HdPh_ResourceBindingSuffixTokens->layout));
    const int layoutSamplerUnit = layoutBinding.GetTextureUnit();

    glActiveTexture(GL_TEXTURE0 + layoutSamplerUnit);
    glBindTexture(GL_TEXTURE_1D, bind ? texture.GetLayoutTexture()->GetRawResource() : 0);
  }
};

template<HdTextureType textureType, class Functor, typename... Args>
void _CastAndCompute(HdPhShaderCode::NamedTextureHandle const &namedTextureHandle, Args &&...args)
{
  // e.g. HdPhUvTextureObject
  using TextureObject = HdPhTypedTextureObject<textureType>;
  // e.g. HdPhUvSamplerObject
  using SamplerObject = HdPhTypedSamplerObject<textureType>;

  if (!namedTextureHandle.handle) {
    TF_CODING_ERROR("Invalid texture handle in texture binder.");
    return;
  }

  const TextureObject *const typedTexture = dynamic_cast<TextureObject *>(
      namedTextureHandle.handle->GetTextureObject().get());
  if (!typedTexture) {
    TF_CODING_ERROR("Bad texture object");
    return;
  }

  const SamplerObject *const typedSampler = dynamic_cast<SamplerObject *>(
      namedTextureHandle.handle->GetSamplerObject().get());
  if (!typedSampler) {
    TF_CODING_ERROR("Bad sampler object");
    return;
  }

  Functor::Compute(
      namedTextureHandle.name, *typedTexture, *typedSampler, std::forward<Args>(args)...);
}

template<class Functor, typename... Args>
void _Dispatch(HdPhShaderCode::NamedTextureHandle const &namedTextureHandle, Args &&...args)
{
  switch (namedTextureHandle.type) {
    case HdTextureType::Uv:
      _CastAndCompute<HdTextureType::Uv, Functor>(namedTextureHandle, std::forward<Args>(args)...);
      break;
    case HdTextureType::Field:
      _CastAndCompute<HdTextureType::Field, Functor>(namedTextureHandle,
                                                     std::forward<Args>(args)...);
      break;
    case HdTextureType::Ptex:
      _CastAndCompute<HdTextureType::Ptex, Functor>(namedTextureHandle,
                                                    std::forward<Args>(args)...);
      break;
    case HdTextureType::Udim:
      _CastAndCompute<HdTextureType::Udim, Functor>(namedTextureHandle,
                                                    std::forward<Args>(args)...);
      break;
  }
}

template<class Functor, typename... Args>
void _Dispatch(HdPhShaderCode::NamedTextureHandleVector const &textures, Args &&...args)
{
  for (const HdPhShaderCode::NamedTextureHandle &texture : textures) {
    _Dispatch<Functor>(texture, std::forward<Args>(args)...);
  }
}

}  // end anonymous namespace

void HdPh_TextureBinder::ComputeBufferSources(const NamedTextureHandleVector &textures,
                                              const bool useBindlessHandles,
                                              HdBufferSourceSharedPtrVector *const sources)
{
  _Dispatch<_ComputeBufferSourcesFunctor>(textures, useBindlessHandles, sources);
}

void HdPh_TextureBinder::BindResources(HdPh_ResourceBinder const &binder,
                                       const bool useBindlessHandles,
                                       const NamedTextureHandleVector &textures)
{
  if (useBindlessHandles) {
    return;
  }

  _Dispatch<_BindFunctor>(textures, binder, /* bind = */ true);
}

void HdPh_TextureBinder::UnbindResources(HdPh_ResourceBinder const &binder,
                                         const bool useBindlessHandles,
                                         const NamedTextureHandleVector &textures)
{
  if (useBindlessHandles) {
    return;
  }

  _Dispatch<_BindFunctor>(textures, binder, /* bind = */ false);
}

WABI_NAMESPACE_END
