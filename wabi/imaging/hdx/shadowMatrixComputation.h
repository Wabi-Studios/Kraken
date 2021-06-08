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
#ifndef WABI_IMAGING_HDX_SHADOW_MATRIX_COMPUTATION_H
#define WABI_IMAGING_HDX_SHADOW_MATRIX_COMPUTATION_H

#include "wabi/base/gf/matrix4d.h"
#include "wabi/imaging/cameraUtil/conformWindow.h"
#include "wabi/imaging/hdx/api.h"
#include "wabi/imaging/hdx/version.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class CameraUtilFraming;

// Interface class for computing the shadow matrix
// for a given viewport.
class HdxShadowMatrixComputation {
 public:
  // For legacy clients using viewport, will be removed eventually.
  virtual std::vector<GfMatrix4d> Compute(const GfVec4f &viewport,
                                          CameraUtilConformWindowPolicy policy) = 0;

  // For modern clients using camera framing API.
  virtual std::vector<GfMatrix4d> Compute(const CameraUtilFraming &framing,
                                          CameraUtilConformWindowPolicy policy) = 0;

 protected:
  HdxShadowMatrixComputation()          = default;
  virtual ~HdxShadowMatrixComputation() = default;

 private:
  HdxShadowMatrixComputation(const HdxShadowMatrixComputation &) = delete;
  HdxShadowMatrixComputation &operator=(const HdxShadowMatrixComputation &) = delete;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HDX_SHADOW_MATRIX_COMPUTATION_H
