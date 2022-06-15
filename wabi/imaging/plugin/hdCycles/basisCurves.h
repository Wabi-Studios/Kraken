//  Copyright 2021 Tangent Animation
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

#ifndef HD_CYCLES_BASIS_CURVES_H
#define HD_CYCLES_BASIS_CURVES_H

#include "api.h"

#include "attributeSource.h"
#include "hdcycles.h"
#include "objectSource.h"
#include "renderDelegate.h"
#include "utils.h"

#include <util/util_transform.h>

#include <wabi/base/gf/matrix4f.h>
#include <wabi/imaging/hd/basisCurves.h>
#include <wabi/wabi.h>

namespace ccl
{
  class Mesh;
  class Scene;
  class Object;
  class Camera;
  class Hair;
  class Geometry;
}  // namespace ccl

WABI_NAMESPACE_BEGIN

class HdSceneDelegate;
class HdCyclesRenderDelegate;

/**
 * @brief Cycles Basis Curve Rprim mapped to Cycles Basis Curve
 *
 */
class HdCyclesBasisCurves final : public HdBasisCurves
{
 public:

  /**
   * @brief Construct a new HdCycles Basis Curve object
   *
   * @param renderDelegate Pointer to the Render Delegate.
   * @param id Path to the basis curves.
   */
  HdCyclesBasisCurves(HdCyclesRenderDelegate *renderDelegate, const SdfPath &id);
  /**
   * @brief Destroy the HdCycles Basis Curves object
   *
   */
  virtual ~HdCyclesBasisCurves();

  /**
   * @brief Pull invalidated material data and prepare/update the core Cycles
   * representation.
   *
   * This must be thread safe.
   *
   * @param sceneDelegate The data source for the basis curve
   * @param renderParam State
   * @param dirtyBits Which bits of scene data has changed
   */
  void Sync(HdSceneDelegate *sceneDelegate,
            HdRenderParam *renderParam,
            HdDirtyBits *dirtyBits,
            TfToken const &reprSelector) override;

  /**
   * @brief Inform the scene graph which state needs to be downloaded in
   * the first Sync() call
   *
   * @return The initial dirty state this basis curve wants to query
   */
  HdDirtyBits GetInitialDirtyBitsMask() const override;

  /**
   * @return Return true if this light is valid.
   */
  bool IsValid() const;

  /**
   * @brief Not Implemented
   */
  void Finalize(HdRenderParam *renderParam) override;

 protected:

  /**
   * @brief Initialize the given representation of this Rprim.
   * This is called prior to syncing the prim.
   *
   * @param reprToken The name of the repr to initialize
   * @param dirtyBits In/Out dirty values
   */
  void _InitRepr(TfToken const &reprToken, HdDirtyBits *dirtyBits) override;

  /**
   * @brief Set additional dirty bits
   *
   * @param bits
   * @return New value of dirty bits
   */
  HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;

  /**
   * @brief Add Color and arbitrary primvar attributes to curves
   * Specifically only uniform varying are supported with the Cycles API
   * This means vertex varying primvars are lossy and grabbed from the root.
   *
   * @param name Name of the color primvar
   * @param value VtValue holding data (floats-float4)
   * @param interpolation Interpolation of colors
   */
  void _AddColors(TfToken name, VtValue value, HdInterpolation interpolation);

  /**
   * @brief Add UV specific attributes to curves.
   * Specifically only uniform varying are supported with the Cycles API
   * This means vertex varying uvs are lossy and grabbed from the root.
   *
   * @param name Name of the UV Set
   * @param uvs VtValue holding uvs
   * @param interpolation Interpolation of uvs
   */
  void _AddUVS(TfToken name, VtValue uvs, HdInterpolation interpolation);

  void _PopulateMotion(HdSceneDelegate *sceneDelegate, const SdfPath &id);

  /**
   * @brief Populate generated coordinates for basisCurves
   *
   */
  void _PopulateGenerated();

 protected:

  VtVec3fArray m_points;
  VtVec3fArray m_normals;
  VtFloatArray m_widths;
  HdBasisCurvesTopology m_topology;
  HdInterpolation m_widthsInterpolation;
  VtIntArray m_indices;
  GfMatrix4f m_transform;

  bool m_motionBlur;
  int m_motionTransformSteps;
  int m_motionDeformSteps;

  unsigned int m_visibilityFlags;

  bool m_visCamera;
  bool m_visDiffuse;
  bool m_visGlossy;
  bool m_visScatter;
  bool m_visShadow;
  bool m_visTransmission;

  ccl::CurveShapeType m_curveShape;
  int m_curveResolution;

  ccl::vector<ccl::Shader *> m_usedShaders;

 private:

  /**
   * @brief Populate the Cycles mesh representation from delegate's data
   */
  void _PopulateCurveMesh(HdRenderParam *renderParam);

  /**
   * @brief Manually create ribbon geometry for curves
   *
   * @param a_camera Optional camera to orient towards
   */
  void _CreateRibbons(ccl::Camera *a_camera = nullptr);

  /**
   * @brief Manually create tube/bevelled geometry for curves
   *
   */
  void _CreateTubeMesh();

  /**
   * @brief Properly populate native cycles curves with curve data
   *
   * @param a_scene Scene to add to
   */
  void _CreateCurves(ccl::Scene *a_scene);

  ccl::Object *m_cyclesObject;
  ccl::Mesh *m_cyclesMesh;
  ccl::Hair *m_cyclesHair;
  ccl::Geometry *m_cyclesGeometry;

  HdCyclesObjectSourceSharedPtr m_object_source;
  HdCyclesRenderDelegate *m_renderDelegate;
};

class HdBbHairAttributeSource : public HdBbAttributeSource
{
 public:

  HdBbHairAttributeSource(TfToken name,
                          const TfToken &role,
                          const VtValue &value,
                          ccl::Hair *hair,
                          const HdInterpolation &interpolation);
};

WABI_NAMESPACE_END

#endif  // HD_CYCLES_BASIS_CURVES_H
