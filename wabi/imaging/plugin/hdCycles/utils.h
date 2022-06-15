//  Copyright 2020 Tangent Animation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
//  including without limitation, as related to merchantability and fitness
//  for a particular purpose.
//
//  In no event shall any copyright holder be liable for any damages of any kind
//  arising from the use of this software, whether in contract, tort or otherwise.
//  See the License for the specific language governing permissions and
//  limitations under the License.

/// @file utils.h
///
/// General utilities for Hydra Cycles
#ifndef HD_CYCLES_UTILS_H
#define HD_CYCLES_UTILS_H

#include "api.h"

#include "hdcycles.h"

#include "Mikktspace/mikktspace.h"

#include <render/mesh.h>
#include <render/object.h>
#include <render/scene.h>
#include <render/session.h>
#include <render/shader.h>
#include <util/util_math_float2.h>
#include <util/util_math_float3.h>
#include <util/util_transform.h>

#include <wabi/base/gf/matrix4d.h>
#include <wabi/base/gf/matrix4f.h>
#include <wabi/base/gf/vec2f.h>
#include <wabi/base/gf/vec3f.h>
#include <wabi/base/gf/vec4f.h>
#include <wabi/base/tf/stringUtils.h>
#include <wabi/base/vt/value.h>
#include <wabi/imaging/hd/basisCurves.h>
#include <wabi/imaging/hd/mesh.h>
#include <wabi/imaging/hd/points.h>
#include <wabi/imaging/hd/sceneDelegate.h>
#include <wabi/imaging/hd/timeSampleArray.h>
#include <wabi/imaging/hd/volume.h>
#include <wabi/wabi.h>

#include <iostream>

namespace ccl
{
  class Mesh;
}

WABI_NAMESPACE_BEGIN

class HdCyclesMesh;

/* =========- Texture ========== */

bool HdCyclesPathIsUDIM(const ccl::string &a_filepath);

void HdCyclesParseUDIMS(const ccl::string &a_filepath, ccl::vector<int> &a_tiles);

/**
 * @brief Cycles specific conversion required to get generated coord offsets
 *
 * @param a_transform
 * @param a_loc
 * @param a_size
 * @return * Cycles
 */
void HdCyclesMeshTextureSpace(ccl::Geometry *a_geom, ccl::float3 &a_loc, ccl::float3 &a_size);

/* ========== Material ========== */

ccl::Shader *HdCyclesCreateDefaultShader();

ccl::Shader *HdCyclesCreateObjectColorSurface();

ccl::Shader *HdCyclesCreateAttribColorSurface();

/**
 * @brief Helper function to dump shader graph if
 * CYCLES_DUMP_SHADER_GRAPH_DIR environment variable
 * is set.
 *
 * @return true if dumped
 */
bool _DumpGraph(ccl::ShaderGraph *shaderGraph, const char *name);

/* ========= Conversion ========= */

const char *_HdInterpolationStr(const HdInterpolation &i);

/**
 * @brief Create Cycles Transform from given HdSceneDelegate and SdfPath
 *
 * @param delegate
 * @param id
 * @return Cycles Transform
 */
HdTimeSampleArray<GfMatrix4d, HD_CYCLES_MOTION_STEPS> HdCyclesSetTransform(
  ccl::Object *object,
  HdSceneDelegate *delegate,
  const SdfPath &id,
  bool use_motion);

ccl::Transform HdCyclesExtractTransform(HdSceneDelegate *delegate, const SdfPath &id);

/**
 * @brief Convert USD Camera space to Cycles camera space
 *
 * @param a_cameraTransform
 * @return GfMatrix4d
 */
GfMatrix4d ConvertCameraTransform(const GfMatrix4d &a_cameraTransform);

/**
 * @brief Convert GfMatrix4d to Cycles Transform representation
 *
 * @param mat
 * @return ccl::Transform
 */
ccl::Transform mat4d_to_transform(const GfMatrix4d &mat);

/**
 * @brief Convert GfMatrix4f to Cycles Transform representation
 *
 * @param mat
 * @return ccl::Transform
 */
ccl::Transform mat4f_to_transform(const GfMatrix4f &mat);

template<typename T, typename U> inline U to_cycles(const T &vec) noexcept {}

template<> inline float to_cycles<float, float>(const float &v) noexcept;
template<> inline float to_cycles<double, float>(const double &v) noexcept;
template<> inline float to_cycles<int, float>(const int &v) noexcept;

template<> inline ccl::float2 to_cycles<GfVec2f, ccl::float2>(const GfVec2f &v) noexcept;
template<> inline ccl::float2 to_cycles<GfVec2h, ccl::float2>(const GfVec2h &v) noexcept;
template<> inline ccl::float2 to_cycles<GfVec2d, ccl::float2>(const GfVec2d &v) noexcept;
template<> inline ccl::float2 to_cycles<GfVec2i, ccl::float2>(const GfVec2i &v) noexcept;

template<> inline ccl::float3 to_cycles<GfVec3f, ccl::float3>(const GfVec3f &v) noexcept;
template<> inline ccl::float3 to_cycles<GfVec3h, ccl::float3>(const GfVec3h &v) noexcept;
template<> inline ccl::float3 to_cycles<GfVec3d, ccl::float3>(const GfVec3d &v) noexcept;
template<> inline ccl::float3 to_cycles<GfVec3i, ccl::float3>(const GfVec3i &v) noexcept;

template<> inline ccl::float4 to_cycles<GfVec3f, ccl::float4>(const GfVec3f &v) noexcept;
template<> inline ccl::float4 to_cycles<GfVec3h, ccl::float4>(const GfVec3h &v) noexcept;
template<> inline ccl::float4 to_cycles<GfVec3d, ccl::float4>(const GfVec3d &v) noexcept;
template<> inline ccl::float4 to_cycles<GfVec3i, ccl::float4>(const GfVec3i &v) noexcept;

/**
 * @brief Convert GfVec2i to Cycles int2 representation
 *
 * @param a_vec
 * @return Cycles int2
 */
ccl::int2 vec2i_to_int2(const GfVec2i &a_vec);

/**
 * @brief Convert int2 to GfVec2i representation
 *
 * @param a_int
 * @return GfVec2i
 */
GfVec2i int2_to_vec2i(const ccl::int2 &a_int);

/**
 * @brief Convert int2 to GfVec2f representation
 *
 * @param a_int
 * @return GfVec2f
 */
GfVec2f int2_to_vec2f(const ccl::int2 &a_int);

/**
 * @brief Convert GfVec2f to Cycles float2 representation
 *
 * @param a_vec
 * @return Cycles float2
 */
ccl::float2 vec2f_to_float2(const GfVec2f &a_vec);

/**
 * @brief Convert GfVec2f to Cycles int2 representation
 *
 * @param a_vec
 * @return Cycles int2
 */
ccl::int2 vec2f_to_int2(const GfVec2f &a_vec);

/**
 * @brief Convert GfVec2i to Cycles float2 representation
 *
 * @param a_vec
 * @return Cycles float2
 */
ccl::float2 vec2i_to_float2(const GfVec2i &a_vec);

/**
 * @brief Convert GfVec2d to Cycles float2 representation
 *
 * @param a_vec
 * @return Cycles float2
 */
ccl::float2 vec2d_to_float2(const GfVec2d &a_vec);

/**
 * @brief Convert GfVec3f to Cycles float2 representation
 *
 * @param a_vec
 * @return Cycles float2
 */
ccl::float2 vec3f_to_float2(const GfVec3f &a_vec);

/**
 * @brief Convert Float to Cycles float3 representation
 *
 * @param a_vec
 * @return Cycles float3
 */
ccl::float3 float_to_float3(const float &a_vec);

/**
 * @brief Convert GfVec2f to Cycles float3 representation
 *
 * @param a_vec
 * @return Cycles float3
 */
ccl::float3 vec2f_to_float3(const GfVec2f &a_vec);

/**
 * @brief Convert GfVec3f to Cycles float3 representation
 *
 * @param a_vec
 * @return Cycles float3
 */
ccl::float3 vec3f_to_float3(const GfVec3f &a_vec);

/**
 * @brief Convert GfVec3i to Cycles float3 representation
 *
 * @param a_vec
 * @return Cycles float3
 */
ccl::float3 vec3i_to_float3(const GfVec3i &a_vec);

/**
 * @brief Convert GfVec3d to Cycles float3 representation
 *
 * @param a_vec
 * @return Cycles float3
 */
ccl::float3 vec3d_to_float3(const GfVec3d &a_vec);

/**
 * @brief Lossy convert GfVec4f to Cycles float3 representation
 *
 * @param a_vec
 * @return Cycles float3
 */
ccl::float3 vec4f_to_float3(const GfVec4f &a_vec);

/**
 * @brief Convert float to Cycles float4
 * Fills all components with float
 *
 * @param a_vec
 * @param a_alpha
 * @return Cycles float4
 */
ccl::float4 vec1f_to_float4(const float &a_vec);

/**
 * @brief Convert GfVec2f to Cycles float4
 * Z is set to a default of 0
 * Alpha is set to a default of 1
 *
 * @param a_vec
 * @param a_z
 * @param a_alpha
 * @return Cycles float4
 */
ccl::float4 vec2f_to_float4(const GfVec2f &a_vec, float a_z = 0.0f, float a_alpha = 1.0f);

/**
 * @brief Convert GfVec3f to Cycles float4 representation with alpha option
 *
 * @param a_vec
 * @param a_alpha
 * @return Cycles float4
 */
ccl::float4 vec3f_to_float4(const GfVec3f &a_vec, float a_alpha = 1.0f);

/**
 * @brief Convert GfVec4f to Cycles float4 representation
 *
 * @param a_vec
 * @return Cycles float4
 */
ccl::float4 vec4f_to_float4(const GfVec4f &a_vec);

/* ========= Primvars ========= */

// HdCycles primvar handling. Designed reference based on HdArnold implementation

struct HdCyclesPrimvar
{
  VtValue value;                  // Copy-on-write value of the primvar
  TfToken role;                   // Role of the primvar
  HdInterpolation interpolation;  // Type of interpolation used for the value
  bool dirtied;                   // If the primvar has been dirtied

  HdCyclesPrimvar(const VtValue &a_value, const TfToken &a_role, HdInterpolation a_interpolation)
    : value(a_value),
      role(a_role),
      interpolation(a_interpolation),
      dirtied(true)
  {}
};

using HdCyclesPrimvarMap = std::unordered_map<TfToken, HdCyclesPrimvar, TfToken::HashFunctor>;

// Get Computed primvars
bool HdCyclesGetComputedPrimvars(HdSceneDelegate *a_delegate,
                                 const SdfPath &a_id,
                                 HdDirtyBits a_dirtyBits,
                                 HdCyclesPrimvarMap &a_primvars);

// Get Non-computed primvars
bool HdCyclesGetPrimvars(HdSceneDelegate *a_delegate,
                         const SdfPath &a_id,
                         HdDirtyBits a_dirtyBits,
                         bool a_multiplePositionKeys,
                         HdCyclesPrimvarMap &a_primvars);

typedef std::map<HdInterpolation, HdPrimvarDescriptorVector> HdCyclesPDPIMap;

void HdCyclesPopulatePrimvarDescsPerInterpolation(HdSceneDelegate *a_sceneDelegate,
                                                  SdfPath const &a_id,
                                                  HdCyclesPDPIMap *a_primvarDescsPerInterpolation);

bool HdCyclesIsPrimvarExists(TfToken const &a_name,
                             HdCyclesPDPIMap const &a_primvarDescsPerInterpolation,
                             HdInterpolation *a_interpolation = nullptr);

using HdCyclesSampledPrimvarType = HdTimeSampleArray<VtValue, HD_CYCLES_MAX_PRIMVAR_SAMPLES>;

/* ======== VtValue Utils ========= */

template<typename F> void _CheckForBoolValue(const VtValue &value, F &&f)
{
  if (value.IsHolding<bool>()) {
    f(value.UncheckedGet<bool>());
  } else if (value.IsHolding<int>()) {
    f(value.UncheckedGet<int>() != 0);
  } else if (value.IsHolding<long>()) {
    f(value.UncheckedGet<long>() != 0);
  }
}

template<typename F> void _CheckForIntValue(const VtValue &value, F &&f)
{
  if (value.IsHolding<int>()) {
    f(value.UncheckedGet<int>());
  } else if (value.IsHolding<long>()) {
    f(static_cast<int>(value.UncheckedGet<long>()));
  }
}

template<typename F> void _CheckForFloatValue(const VtValue &value, F &&f)
{
  if (value.IsHolding<float>()) {
    f(value.UncheckedGet<float>());
  }
}

template<typename F> void _CheckForDoubleValue(const VtValue &value, F &&f)
{
  if (value.IsHolding<double>()) {
    f(value.UncheckedGet<double>());
  }
}

template<typename F> void _CheckForStringValue(const VtValue &value, F &&f)
{
  if (value.IsHolding<std::string>()) {
    f(value.UncheckedGet<std::string>());
  }
}

template<typename F> void _CheckForVec2iValue(const VtValue &value, F &&f)
{
  if (value.IsHolding<GfVec2i>()) {
    f(value.UncheckedGet<GfVec2i>());
  }
}

// Get value

template<typename T>
T _HdCyclesGetVtValue(VtValue a_value,
                      T a_default,
                      bool *a_hasChanged = nullptr,
                      bool a_checkWithDefault = false)
{
  if (!a_value.IsEmpty()) {
    if (a_value.IsHolding<T>()) {
      T val = a_value.UncheckedGet<T>();
      if (a_hasChanged) {
        if (a_checkWithDefault && val != a_default)
          *a_hasChanged = true;
        else
          *a_hasChanged = true;
      }
      return val;
    }
  }
  return a_default;
}

// Bool specialization

template<>
bool _HdCyclesGetVtValue<bool>(VtValue a_value,
                               bool a_default,
                               bool *a_hasChanged,
                               bool a_checkWithDefault);

// Get abitrary param

template<typename T>
T _HdCyclesGetParam(HdSceneDelegate *a_scene, SdfPath a_id, TfToken a_token, T a_default)
{
  // TODO: This is not Get() Because of the reasons listed here:
  // https://groups.google.com/g/usd-interest/c/k-N05Ac7SRk/m/RtK5HvglAQAJ
  // We may need to fix this in newer versions of USD
  VtValue val = a_scene->GetLightParamValue(a_id, a_token);
  return _HdCyclesGetVtValue<T>(val, a_default);
}

// Get mesh param

template<typename T>
T _HdCyclesGetMeshParam(const HdPrimvarDescriptor &a_pvd,
                        HdDirtyBits *a_dirtyBits,
                        const SdfPath &a_id,
                        HdMesh *a_mesh,
                        HdSceneDelegate *a_scene,
                        TfToken a_token,
                        T a_default)
{
  // TODO: Optimize this
  // Needed because our current schema stores tokens with primvars: prefix
  // however the HdPrimvarDescriptor omits this.
  // Solution could be to remove from usdCycles schema and add in all settings
  // providers (houdini_cycles, blender exporter)
  if ("primvars:" + a_pvd.name.GetString() == a_token.GetString()) {
    if (HdChangeTracker::IsPrimvarDirty(*a_dirtyBits, a_id, a_token)) {
      VtValue v;
      v = a_mesh->GetPrimvar(a_scene, a_token);
      return _HdCyclesGetVtValue<T>(v, a_default);
    }
  }
  return a_default;
}

// Get curve param

// This needs to be refactored.
template<typename T>
T _HdCyclesGetCurvePrimvar(const HdPrimvarDescriptor &a_pvd,
                           HdDirtyBits *a_dirtyBits,
                           const SdfPath &a_id,
                           HdBasisCurves *a_curve,
                           HdSceneDelegate *a_scene,
                           TfToken a_token,
                           T a_default)
{
  // Needed because our current schema stores tokens with primvars: prefix
  // however the HdPrimvarDescriptor omits this.
  // Solution could be to remove from usdCycles schema and add in all settings
  // providers (houdini_cycles, blender exporter)
  if ("primvars:" + a_pvd.name.GetString() == a_token.GetString()) {
    VtValue v;
    v = a_curve->GetPrimvar(a_scene, a_token);
    if (v.IsHolding<T>()) {
      return v.UncheckedGet<T>();
    } else {
      return a_default;
    }
  }
  return a_default;
}

// Get curve param

template<typename T>
T _HdCyclesGetCurveParam(HdDirtyBits *a_dirtyBits,
                         const SdfPath &a_id,
                         HdBasisCurves *a_curves,
                         HdSceneDelegate *a_scene,
                         TfToken a_token,
                         T a_default)
{
  if (HdChangeTracker::IsPrimvarDirty(*a_dirtyBits, a_id, a_token)) {
    VtValue v;
    v = a_curves->GetPrimvar(a_scene, a_token);
    return _HdCyclesGetVtValue<T>(v, a_default);
  }
  return a_default;
}

// Get light param

template<typename T>
T _HdCyclesGetLightParam(const SdfPath &a_id,
                         HdSceneDelegate *a_scene,
                         TfToken a_token,
                         T a_default)
{
  VtValue v = a_scene->GetLightParamValue(a_id, a_token);
  return _HdCyclesGetVtValue<T>(v, a_default);
}

// Get camera param

template<typename T>
T _HdCyclesGetCameraParam(HdSceneDelegate *a_scene, SdfPath a_id, TfToken a_token, T a_default)
{
  VtValue v = a_scene->GetCameraParamValue(a_id, a_token);
  return _HdCyclesGetVtValue<T>(v, a_default);
}

// Get Volume param

template<typename T>
T _HdCyclesGetVolumeParam(const HdPrimvarDescriptor &a_pvd,
                          HdDirtyBits *a_dirtyBits,
                          const SdfPath &a_id,
                          HdVolume *a_volume,
                          HdSceneDelegate *a_scene,
                          TfToken a_token,
                          T a_default)
{
  // TODO: Optimize this
  // Needed because our current schema stores tokens with primvars: prefix
  // however the HdPrimvarDescriptor omits this.
  // Solution could be to remove from usdCycles schema and add in all settings
  // providers (houdini_cycles, blender exporter)
  if ("primvars:" + a_pvd.name.GetString() == a_token.GetString()) {
    if (HdChangeTracker::IsPrimvarDirty(*a_dirtyBits, a_id, a_token)) {
      VtValue v;
      v = a_volume->GetPrimvar(a_scene, a_token);
      return _HdCyclesGetVtValue<T>(v, a_default);
    }
  }
  return a_default;
}

// Get Points param

template<typename T>
T _HdCyclesGetPointsParam(const HdPrimvarDescriptor &a_pvd,
                          HdDirtyBits *a_dirtyBits,
                          const SdfPath &a_id,
                          HdPoints *a_point,
                          HdSceneDelegate *a_scene,
                          TfToken a_token,
                          T a_default)
{
  // TODO: Optimize this
  // Needed because our current schema stores tokens with primvars: prefix
  // however the HdPrimvarDescriptor omits this.
  // Solution could be to remove from usdCycles schema and add in all settings
  // providers (houdini_cycles, blender exporter)
  if ("primvars:" + a_pvd.name.GetString() == a_token.GetString()) {
    if (HdChangeTracker::IsPrimvarDirty(*a_dirtyBits, a_id, a_token)) {
      VtValue v;
      v = a_point->GetPrimvar(a_scene, a_token);
      return _HdCyclesGetVtValue<T>(v, a_default);
    }
  }
  return a_default;
}

/* ========= MikkTSpace ========= */

int mikk_get_num_faces(const SMikkTSpaceContext *context);

int mikk_get_num_verts_of_face(const SMikkTSpaceContext *context, const int face_num);

int mikk_vertex_index(const ccl::Mesh *mesh, const int face_num, const int vert_num);

int mikk_corner_index(const ccl::Mesh *mesh, const int face_num, const int vert_num);

void mikk_get_position(const SMikkTSpaceContext *context,
                       float P[3],
                       const int face_num,
                       const int vert_num);

void mikk_get_texture_coordinate(const SMikkTSpaceContext *context,
                                 float uv[2],
                                 const int face_num,
                                 const int vert_num);

void mikk_get_normal(const SMikkTSpaceContext *context,
                     float N[3],
                     const int face_num,
                     const int vert_num);

void mikk_set_tangent_space(const SMikkTSpaceContext *context,
                            const float T[],
                            const float sign,
                            const int face_num,
                            const int vert_num);

void mikk_compute_tangents(const char *layer_name,
                           ccl::Mesh *mesh,
                           bool need_sign,
                           bool active_render);

WABI_NAMESPACE_END

#endif  // HD_CYCLES_UTILS_H
