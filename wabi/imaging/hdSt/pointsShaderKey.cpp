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
#include "wabi/wabi.h"
#include "wabi/imaging/hdSt/pointsShaderKey.h"
#include "wabi/base/tf/staticTokens.h"

WABI_NAMESPACE_BEGIN


TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    ((baseGLSLFX,         "points.glslfx"))

    // point id mixins (for point picking & selection)
    ((pointIdVS,                "PointId.Vertex.PointParam"))
    ((pointIdSelDecodeUtilsVS,  "Selection.DecodeUtils"))
    ((pointIdSelPointSelVS,     "Selection.Vertex.PointSel"))
    ((pointIdFS,                "PointId.Fragment.PointParam"))

    // main for all the shader stages
    ((mainVS,                   "Point.Vertex"))
    ((mainFS,                   "Point.Fragment"))

    // terminals        
    ((commonFS,                 "Fragment.CommonTerminals"))
    ((surfaceFS,                "Fragment.Surface"))
    ((noScalarOverrideFS,       "Fragment.NoScalarOverride"))

    // instancing       
    ((instancing,               "Instancing.Transform"))
);

HdSt_PointsShaderKey::HdSt_PointsShaderKey()
    : glslfx(_tokens->baseGLSLFX)
{
    VS[0] = _tokens->instancing;
    VS[1] = _tokens->mainVS;
    VS[2] = _tokens->pointIdVS;
    VS[3] = _tokens->pointIdSelDecodeUtilsVS;
    VS[4] = _tokens->pointIdSelPointSelVS;
    VS[5] = TfToken();

    // Common must be first as it defines terminal interfaces
    FS[0] = _tokens->commonFS;
    FS[1] = _tokens->surfaceFS;
    FS[2] = _tokens->noScalarOverrideFS;
    FS[3] = _tokens->mainFS;
    FS[4] = _tokens->pointIdFS;
    FS[5] = TfToken();
}

HdSt_PointsShaderKey::~HdSt_PointsShaderKey()
{
}

WABI_NAMESPACE_END

