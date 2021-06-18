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
#ifndef WABI_IMAGING_CAMERA_UTIL_SCREEN_WINDOW_PARAMETERS_H
#define WABI_IMAGING_CAMERA_UTIL_SCREEN_WINDOW_PARAMETERS_H

#include "wabi/base/gf/camera.h"
#include "wabi/imaging/cameraUtil/api.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

/// \class CameraUtilScreenWindowParameters
///
/// Given a camera object, compute parameters suitable for setting up
/// RenderMan.
///
class CameraUtilScreenWindowParameters
{
 public:
  /// Constructs screenwindow parameter. The optional \p fitDirection
  /// indicates in which direction the screenwindow will have length 2.
  CAMERAUTIL_API
  CameraUtilScreenWindowParameters(const GfCamera &camera,
                                   GfCamera::FOVDirection fitDirection = GfCamera::FOVHorizontal);

  /// The vector (left, right, bottom, top) defining the rectangle in the
  /// image plane.
  /// Give these parameters to RiScreenWindow.
  const GfVec4d &GetScreenWindow() const
  {
    return _screenWindow;
  }

  /// The field of view. More precisely, the full angle perspective field
  /// of view (in degrees) between screen space coordinates (-1,0) and
  /// (1,0).
  /// Give these parameters to RiProjection as parameter after
  /// "perspective".
  double GetFieldOfView() const
  {
    return _fieldOfView;
  }

  /// Returns the inverse of the transform for a camera that is y-Up
  /// and z-facing (vs the OpenGL camera that is (-z)-facing).
  /// Write this transform with RiConcatTransform before
  /// RiWorldBegin.
  const GfMatrix4d &GetZFacingViewMatrix() const
  {
    return _zFacingViewMatrix;
  }

 private:
  GfVec4d _screenWindow;
  double _fieldOfView;
  GfMatrix4d _zFacingViewMatrix;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_CAMERA_UTIL_SCREEN_WINDOW_PARAMETERS_H
