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

#ifndef HDRPR_CAMERA_H
#define HDRPR_CAMERA_H

#include "wabi/base/gf/range1f.h"
#include "wabi/base/gf/vec2f.h"
#include "wabi/imaging/hd/camera.h"
#include "wabi/imaging/hd/sceneDelegate.h"

WABI_NAMESPACE_BEGIN

class HdRprCamera : public HdCamera
{
 public:

  enum Projection
  {
    Perspective = 0,
    Orthographic
  };

  HdRprCamera(SdfPath const &id);
  ~HdRprCamera() override = default;

  void Sync(HdSceneDelegate *sceneDelegate,
            HdRenderParam *renderParam,
            HdDirtyBits *dirtyBits) override;

  HdDirtyBits GetInitialDirtyBitsMask() const override;

  void Finalize(HdRenderParam *renderParam) override;

  bool GetApertureSize(GfVec2f *value) const;
  bool GetApertureOffset(GfVec2f *value) const;
  bool GetFocalLength(float *value) const;
  bool GetFStop(float *value) const;
  bool GetFocusDistance(float *value) const;
  bool GetShutterOpen(double *value) const;
  bool GetShutterClose(double *value) const;
  bool GetClippingRange(GfRange1f *value) const;
  bool GetProjection(Projection *value) const;
  HdTimeSampleArray<GfMatrix4d, 2> const &GetTransformSamples() const
  {
    return m_transform;
  }
  int GetApertureBlades() const
  {
    return m_apertureBlades;
  }

  HdDirtyBits GetDirtyBits() const
  {
    return m_rprDirtyBits;
  }
  void CleanDirtyBits() const
  {
    m_rprDirtyBits = HdCamera::Clean;
  }

 private:

  float m_horizontalAperture;
  float m_verticalAperture;
  float m_horizontalApertureOffset;
  float m_verticalApertureOffset;
  float m_focalLength;
  float m_fStop;
  float m_focusDistance;
  int m_apertureBlades;
  double m_shutterOpen;
  double m_shutterClose;
  GfRange1f m_clippingRange;
  Projection m_projection = Perspective;
  HdTimeSampleArray<GfMatrix4d, 2> m_transform;

  mutable HdDirtyBits m_rprDirtyBits = HdCamera::AllDirty;
};

WABI_NAMESPACE_END

#endif  // HDRPR_CAMERA_H
