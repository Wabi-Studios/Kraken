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
///
/// \file camera/wrapConformWindow.h

#include "wabi/imaging/cameraUtil/conformWindow.h"

#include "wabi/base/gf/camera.h"
#include "wabi/base/gf/frustum.h"
#include "wabi/base/gf/range2d.h"
#include "wabi/base/gf/vec2d.h"
#include "wabi/base/gf/vec4d.h"

#include "wabi/base/tf/pyEnum.h"

#include <boost/python.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

void wrapConformWindow()
{
  typedef GfRange2d (*Signature1)(const GfRange2d &, CameraUtilConformWindowPolicy, double);

  typedef GfVec2d (*Signature2)(const GfVec2d &, CameraUtilConformWindowPolicy, double);

  typedef GfVec4d (*Signature3)(const GfVec4d &, CameraUtilConformWindowPolicy, double);

  typedef GfMatrix4d (*Signature4)(const GfMatrix4d &, CameraUtilConformWindowPolicy, double);

  typedef void (*Signature5)(GfCamera *, CameraUtilConformWindowPolicy, double);

  typedef void (*Signature6)(GfFrustum *, CameraUtilConformWindowPolicy, double);

  def("ConformedWindow",
      (Signature1)&CameraUtilConformedWindow,
      (arg("window"), arg("policy"), arg("targetAspect")));
  def("ConformedWindow",
      (Signature2)&CameraUtilConformedWindow,
      (arg("window"), arg("policy"), arg("targetAspect")));
  def("ConformedWindow",
      (Signature3)&CameraUtilConformedWindow,
      (arg("window"), arg("policy"), arg("targetAspect")));
  def("ConformedWindow",
      (Signature4)&CameraUtilConformedWindow,
      (arg("window"), arg("policy"), arg("targetAspect")));

  def("ConformWindow",
      (Signature5)&CameraUtilConformWindow,
      (arg("camera"), arg("policy"), arg("targetAspect")));

  def("ConformWindow",
      (Signature6)&CameraUtilConformWindow,
      (arg("frustum"), arg("policy"), arg("targetAspect")));

  TfPyWrapEnum<CameraUtilConformWindowPolicy>();
}
