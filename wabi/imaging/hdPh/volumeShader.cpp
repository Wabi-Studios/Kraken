//
// Copyright 2019 Pixar
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
#include "wabi/imaging/hdPh/volumeShader.h"

#include "wabi/base/tf/staticTokens.h"
#include "wabi/imaging/glf/contextCaps.h"
#include "wabi/imaging/hd/renderDelegate.h"
#include "wabi/imaging/hd/vtBufferSource.h"
#include "wabi/imaging/hdPh/field.h"
#include "wabi/imaging/hdPh/materialParam.h"
#include "wabi/imaging/hdPh/resourceBinder.h"
#include "wabi/imaging/hdPh/resourceRegistry.h"
#include "wabi/imaging/hdPh/textureBinder.h"
#include "wabi/imaging/hdPh/textureHandle.h"
#include "wabi/imaging/hdPh/textureObject.h"
#include "wabi/imaging/hdPh/tokens.h"
#include "wabi/imaging/hdPh/volume.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_PRIVATE_TOKENS(
  _tokens,
  (stepSize)(stepSizeLighting)

    (sampleDistance)(volumeBBoxInverseTransform)(volumeBBoxLocalMin)(volumeBBoxLocalMax));

HdPh_VolumeShader::HdPh_VolumeShader(HdRenderDelegate *const renderDelegate)
  : _renderDelegate(renderDelegate),
    _lastRenderSettingsVersion(0),
    _stepSize(HdPhVolume::defaultStepSize),
    _stepSizeLighting(HdPhVolume::defaultStepSizeLighting),
    _fillsPointsBar(false)
{}

HdPh_VolumeShader::~HdPh_VolumeShader() = default;

void HdPh_VolumeShader::AddBindings(HdBindingRequestVector *const customBindings)
{
  HdPhSurfaceShader::AddBindings(customBindings);
  customBindings->push_back(HdBindingRequest(HdBinding::UNIFORM, _tokens->stepSize, HdTypeFloat));
  customBindings->push_back(HdBindingRequest(HdBinding::UNIFORM, _tokens->stepSizeLighting, HdTypeFloat));
}

void HdPh_VolumeShader::BindResources(const int program,
                                      HdPh_ResourceBinder const &binder,
                                      HdRenderPassState const &state)
{
  HdPhSurfaceShader::BindResources(program, binder, state);

  const int currentRenderSettingsVersion = _renderDelegate->GetRenderSettingsVersion();

  if (_lastRenderSettingsVersion != currentRenderSettingsVersion) {
    _lastRenderSettingsVersion = currentRenderSettingsVersion;
    _stepSize = _renderDelegate->GetRenderSetting<float>(HdPhRenderSettingsTokens->volumeRaymarchingStepSize,
                                                         HdPhVolume::defaultStepSize);
    _stepSizeLighting = _renderDelegate->GetRenderSetting<float>(
      HdPhRenderSettingsTokens->volumeRaymarchingStepSizeLighting, HdPhVolume::defaultStepSizeLighting);
  }

  binder.BindUniformf(_tokens->stepSize, 1, &_stepSize);
  binder.BindUniformf(_tokens->stepSizeLighting, 1, &_stepSizeLighting);
}

void HdPh_VolumeShader::UnbindResources(const int program,
                                        HdPh_ResourceBinder const &binder,
                                        HdRenderPassState const &state)
{
  HdPhSurfaceShader::UnbindResources(program, binder, state);
}

void HdPh_VolumeShader::SetPointsBar(HdBufferArrayRangeSharedPtr const &pointsBar)
{
  _pointsBar = pointsBar;
}

void HdPh_VolumeShader::SetFillsPointsBar(const bool fillsPointsBar)
{
  _fillsPointsBar = fillsPointsBar;
}

static TfToken _ConcatFallback(const TfToken &token)
{
  return TfToken(token.GetString() + HdPh_ResourceBindingSuffixTokens->fallback.GetString());
}

void HdPh_VolumeShader::GetParamsAndBufferSpecsForBBoxAndSampleDistance(
  HdPh_MaterialParamVector *const params,
  HdBufferSpecVector *const specs)
{
  {
    params->emplace_back(
      HdPh_MaterialParam::ParamTypeFallback, _tokens->volumeBBoxInverseTransform, VtValue(GfMatrix4d()));

    static const TfToken sourceName(_ConcatFallback(_tokens->volumeBBoxInverseTransform));
    specs->emplace_back(sourceName, HdTupleType{HdTypeDoubleMat4, 1});
  }

  {
    params->emplace_back(
      HdPh_MaterialParam::ParamTypeFallback, _tokens->volumeBBoxLocalMin, VtValue(GfVec3d()));

    static const TfToken sourceName(_ConcatFallback(_tokens->volumeBBoxLocalMin));
    specs->emplace_back(sourceName, HdTupleType{HdTypeDoubleVec3, 1});
  }

  {
    params->emplace_back(
      HdPh_MaterialParam::ParamTypeFallback, _tokens->volumeBBoxLocalMax, VtValue(GfVec3d()));

    static const TfToken sourceName(_ConcatFallback(_tokens->volumeBBoxLocalMax));
    specs->emplace_back(sourceName, HdTupleType{HdTypeDoubleVec3, 1});
  }

  {
    params->emplace_back(HdPh_MaterialParam::ParamTypeFallback, _tokens->sampleDistance, VtValue(100000.0f));

    static const TfToken sourceName(_ConcatFallback(_tokens->sampleDistance));
    specs->emplace_back(sourceName, HdTupleType{HdTypeFloat, 1});
  }
}

void HdPh_VolumeShader::GetBufferSourcesForBBoxAndSampleDistance(
  const std::pair<GfBBox3d, float> &bboxAndSampleDistance,
  HdBufferSourceSharedPtrVector *const sources)
{
  const GfBBox3d &bbox = bboxAndSampleDistance.first;
  const GfRange3d &range = bbox.GetRange();

  {
    static const TfToken sourceName(_ConcatFallback(_tokens->volumeBBoxInverseTransform));
    sources->push_back(std::make_shared<HdVtBufferSource>(sourceName, VtValue(bbox.GetInverseMatrix())));
  }

  {
    static const TfToken sourceName(_ConcatFallback(_tokens->volumeBBoxLocalMin));
    sources->push_back(std::make_shared<HdVtBufferSource>(sourceName, VtValue(GetSafeMin(range))));
  }

  {
    static const TfToken sourceName(_ConcatFallback(_tokens->volumeBBoxLocalMax));
    sources->push_back(std::make_shared<HdVtBufferSource>(sourceName, VtValue(GetSafeMax(range))));
  }

  {
    const float sampleDistance = bboxAndSampleDistance.second;

    static const TfToken sourceName(_ConcatFallback(_tokens->sampleDistance));
    sources->push_back(std::make_shared<HdVtBufferSource>(sourceName, VtValue(sampleDistance)));
  }
}

GfVec3d HdPh_VolumeShader::GetSafeMin(const GfRange3d &range)
{
  if (range.IsEmpty()) {
    return GfVec3d(0.0, 0.0, 0.0);
  }
  return range.GetMin();
}

GfVec3d HdPh_VolumeShader::GetSafeMax(const GfRange3d &range)
{
  if (range.IsEmpty()) {
    return GfVec3d(0.0, 0.0, 0.0);
  }
  return range.GetMax();
}

namespace {

// Square of length of a 3-vector
float _SqrLengthThreeVector(const double *const v)
{
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

// Assuming the bounding box comes from a grid (range is the
// bounding box of active voxels and matrix the grid transform),
// compute the distance between samples.
//
// Note that this assumes that the bounding box transform is an
// affine transformation obtained by composing scales with rotation.
// (More generally, we would need to take the minimum of the singular
// values from the SVD of the 3x3-matrix).
float _ComputeSampleDistance(const GfBBox3d &bbox)
{
  const GfMatrix4d &m = bbox.GetMatrix();

  // Take minimum of lengths of images of the x-, y-, and z-vector.
  return sqrt(
    std::min({_SqrLengthThreeVector(m[0]), _SqrLengthThreeVector(m[1]), _SqrLengthThreeVector(m[2])}));
}

// Compute the bounding box and sample distance from all the fields in
// this volume.
std::pair<GfBBox3d, float> _ComputeBBoxAndSampleDistance(
  const HdPhShaderCode::NamedTextureHandleVector &textures)
{
  // Computed by combining all bounding boxes.
  GfBBox3d bbox;
  // Computes as minimum of all sampling distances.
  // (Initialized to large value rather than IEEE754-infinity which might
  // not be converted to correctly to GLSL: if there is
  // no texture, the ray marcher simply obtains a point outside the
  // bounding box after one step and stops).
  float sampleDistance = 1000000.0;

  for (const HdPhShaderCode::NamedTextureHandle &texture : textures) {
    HdPhTextureObjectSharedPtr const &textureObject = texture.handle->GetTextureObject();

    if (const HdPhFieldTextureObject *const fieldTex = dynamic_cast<HdPhFieldTextureObject *>(
          textureObject.get())) {
      const GfBBox3d &fieldBbox = fieldTex->GetBoundingBox();
      bbox = GfBBox3d::Combine(bbox, fieldBbox);
      sampleDistance = std::min(sampleDistance, _ComputeSampleDistance(fieldBbox));
    }
  }

  return {bbox, sampleDistance};
}

// Compute 8 vertices of a bounding box.

VtValue _ComputePoints(const GfBBox3d &bbox)
{
  VtVec3fArray points(8);

  size_t i = 0;

  const GfMatrix4d &transform = bbox.GetMatrix();
  const GfRange3d &range = bbox.GetRange();

  // Use vertices of a cube shrunk to point for empty bounding box
  // (to avoid min and max being large floating point numbers).
  const GfVec3d min = HdPh_VolumeShader::GetSafeMin(range);
  const GfVec3d max = HdPh_VolumeShader::GetSafeMax(range);

  for (const double x : {min[0], max[0]}) {
    for (const double y : {min[1], max[1]}) {
      for (const double z : {min[2], max[2]}) {
        points[i] = GfVec3f(transform.Transform(GfVec3d(x, y, z)));
        i++;
      }
    }
  }

  return VtValue(points);
}

}  // end anonymous namespace

void HdPh_VolumeShader::AddResourcesFromTextures(ResourceContext &ctx) const
{
  const bool bindlessTextureEnabled = GlfContextCaps::GetInstance().bindlessTextureEnabled;

  HdBufferSourceSharedPtrVector shaderBarSources;

  // Fills in sampling transforms for textures and also texture
  // handles for bindless textures.
  HdPh_TextureBinder::ComputeBufferSources(
    GetNamedTextureHandles(), bindlessTextureEnabled, &shaderBarSources);

  if (_fillsPointsBar) {
    // Compute volume bounding box from field bounding boxes
    const std::pair<GfBBox3d, float> bboxAndSampleDistance = _ComputeBBoxAndSampleDistance(
      GetNamedTextureHandles());

    const GfBBox3d &bbox = bboxAndSampleDistance.first;

    // Use as points
    ctx.AddSource(_pointsBar, std::make_shared<HdVtBufferSource>(HdTokens->points, _ComputePoints(bbox)));

    // And let the shader know for raymarching bounds.
    GetBufferSourcesForBBoxAndSampleDistance(bboxAndSampleDistance, &shaderBarSources);
  }

  if (!shaderBarSources.empty()) {
    ctx.AddSources(GetShaderData(), std::move(shaderBarSources));
  }
}

void HdPh_VolumeShader::SetFieldDescriptors(const HdVolumeFieldDescriptorVector &fieldDescs)
{
  _fieldDescriptors = fieldDescs;
}

void HdPh_VolumeShader::UpdateTextureHandles(HdSceneDelegate *const sceneDelegate)
{
  TRACE_FUNCTION();

  HdPhResourceRegistrySharedPtr const resourceRegistry = std::static_pointer_cast<HdPhResourceRegistry>(
    sceneDelegate->GetRenderIndex().GetResourceRegistry());

  NamedTextureHandleVector textureHandles = GetNamedTextureHandles();

  if (!TF_VERIFY(textureHandles.size() == _fieldDescriptors.size())) {
    return;
  }

  const bool bindlessTextureEnabled = GlfContextCaps::GetInstance().bindlessTextureEnabled;

  // Walk through the vector of named texture handles and field descriptors
  // simultaneously.
  for (size_t i = 0; i < textureHandles.size(); i++) {
    // To allocate the texture and update it in the vector ...

    // use field descriptor to find field prim, ...
    const HdVolumeFieldDescriptor &fieldDesc = _fieldDescriptors[i];

    const HdPhField *const fieldPrim = dynamic_cast<HdPhField *>(
      sceneDelegate->GetRenderIndex().GetBprim(fieldDesc.fieldPrimType, fieldDesc.fieldId));

    // ask field prim for texture information, ...
    const HdPhTextureIdentifier &textureId = TF_VERIFY(fieldPrim) ? fieldPrim->GetTextureIdentifier() :
                                                                    HdPhTextureIdentifier();
    const HdTextureType textureType = textureHandles[i].type;
    const size_t textureMemory = TF_VERIFY(fieldPrim) ? fieldPrim->GetTextureMemory() : 0;
    static const HdSamplerParameters samplerParams{
      HdWrapBlack, HdWrapBlack, HdWrapBlack, HdMinFilterLinear, HdMagFilterLinear};

    // allocate texture handle and assign it.
    textureHandles[i].handle = resourceRegistry->AllocateTextureHandle(
      textureId, textureType, samplerParams, textureMemory, bindlessTextureEnabled, shared_from_this());
  }

  // And update!
  SetNamedTextureHandles(textureHandles);
}

WABI_NAMESPACE_END
