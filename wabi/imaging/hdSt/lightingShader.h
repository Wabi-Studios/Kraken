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
#ifndef WABI_IMAGING_HD_ST_LIGHTING_SHADER_H
#define WABI_IMAGING_HD_ST_LIGHTING_SHADER_H

#include "wabi/wabi.h"
#include "wabi/imaging/hdSt/api.h"
#include "wabi/imaging/hdSt/shaderCode.h"
#include "wabi/base/gf/matrix4d.h"

WABI_NAMESPACE_BEGIN

using HdStLightingShaderSharedPtr = std::shared_ptr<class HdStLightingShader>;

/// \class HdStLightingShader
///
/// A lighting shader base class.
///
class HdStLightingShader : public HdStShaderCode {
public:
    HDST_API
    HdStLightingShader();
    HDST_API
    virtual ~HdStLightingShader();

    /// Sets camera state.
    virtual void SetCamera(GfMatrix4d const &worldToViewMatrix,
                           GfMatrix4d const &projectionMatrix) = 0;

private:

    // No copying
    HdStLightingShader(const HdStLightingShader &)                     = delete;
    HdStLightingShader &operator =(const HdStLightingShader &)         = delete;
};


WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_ST_LIGHTING_SHADER_H
