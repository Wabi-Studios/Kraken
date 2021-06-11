//
// Copyright 2016 Pixar
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
#ifndef WABI_IMAGING_HD_ST_GLSLFX_SHADER_H
#define WABI_IMAGING_HD_ST_GLSLFX_SHADER_H

#include "wabi/imaging/hd/version.h"
#include "wabi/imaging/hdPh/api.h"
#include "wabi/imaging/hdPh/surfaceShader.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

using HioGlslfxSharedPtr = std::shared_ptr<class HioGlslfx>;

// XXX: DOCS!
class HdPhGLSLFXShader final : public HdPhSurfaceShader {
 public:
  HDPH_API
  HdPhGLSLFXShader(HioGlslfxSharedPtr const &glslfx);
  HDPH_API
  ~HdPhGLSLFXShader() override;

  /// If the prim is based on asset, reload that asset.
  HDPH_API
  void Reload() override;

 private:
  HioGlslfxSharedPtr _glslfx;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_ST_GLSLFX_SHADER_H
