/************************************************************************
Copyright 2020 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#ifndef HDRPR_MESH_H
#define HDRPR_MESH_H

#include "baseRprim.h"

#include "wabi/base/gf/matrix4f.h"
#include "wabi/base/gf/vec2f.h"
#include "wabi/base/vt/array.h"
#include "wabi/imaging/hd/mesh.h"
#include "wabi/imaging/hd/vertexAdjacency.h"

namespace rpr {
class Shape;
}

WABI_NAMESPACE_BEGIN

class HdRprApi;
class RprUsdMaterial;

class HdRprMesh final : public HdRprBaseRprim<HdMesh> {
 public:
  HF_MALLOC_TAG_NEW("new HdRprMesh");

  HdRprMesh(SdfPath const &id HDRPR_INSTANCER_ID_ARG_DECL);
  ~HdRprMesh() override = default;

  void Sync(HdSceneDelegate *sceneDelegate,
            HdRenderParam *renderParam,
            HdDirtyBits *dirtyBits,
            TfToken const &reprName) override;

  void Finalize(HdRenderParam *renderParam) override;

  HdDirtyBits GetInitialDirtyBitsMask() const override;

 protected:
  HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;

  void _InitRepr(TfToken const &reprName, HdDirtyBits *dirtyBits) override;

 private:
  RprUsdMaterial const *GetFallbackMaterial(
      HdSceneDelegate *sceneDelegate,
      HdRprApi *rprApi,
      HdDirtyBits dirtyBits,
      std::map<HdInterpolation, HdPrimvarDescriptorVector> const &primvarDescsPerInterpolation);

 private:
  std::vector<rpr::Shape *> m_rprMeshes;
  std::vector<std::vector<rpr::Shape *>> m_rprMeshInstances;
  RprUsdMaterial *m_fallbackMaterial = nullptr;

  static constexpr int kDefaultNumTimeSamples = 2;
  HdTimeSampleArray<GfMatrix4d, kDefaultNumTimeSamples> m_transformSamples;

  HdMeshTopology m_topology;
  HdGeomSubsets m_geomSubsets;
  VtArray<VtVec3fArray> m_pointSamples;
  VtIntArray m_faceVertexCounts;
  VtIntArray m_faceVertexIndices;
  bool m_enableSubdiv = false;

  Hd_VertexAdjacency m_adjacency;
  bool m_adjacencyValid = false;

  VtArray<VtVec3fArray> m_normalSamples;
  VtIntArray m_normalIndices;
  bool m_normalsValid    = false;
  bool m_authoredNormals = false;
  bool m_smoothNormals   = false;

  VtArray<VtVec2fArray> m_uvSamples;
  VtIntArray m_uvIndices;

  HdDisplayStyle m_displayStyle;
  int m_refineLevel = 0;

  int m_id = -1;
  bool m_ignoreContour;
  std::string m_cryptomatteName;
  size_t m_numGeometrySamples = 1;
};

WABI_NAMESPACE_END

#endif  // HDRPR_MESH_H
