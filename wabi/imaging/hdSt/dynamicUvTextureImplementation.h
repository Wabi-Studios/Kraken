//
// Copyright 2020 Pixar
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
#ifndef WABI_IMAGING_HD_ST_DYNAMIC_UV_TEXTURE_IMPLEMENTATION_H
#define WABI_IMAGING_HD_ST_DYNAMIC_UV_TEXTURE_IMPLEMENTATION_H

#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class HdStDynamicUvTextureObject;

/// \class HdStDynamicUvTextureImplementation
///
/// Allows external clients to specify how a UV texture is loaded from, e.g.,
/// a file and how it is committed to the GPU.
///
class HdStDynamicUvTextureImplementation
{
public:
    /// Called during the load phase of the Storm texture system
    /// when a texture file is supposed to be loaded to the CPU.
    ///
    /// This method has to be thread-safe.
    ///
    virtual void Load(HdStDynamicUvTextureObject *textureObject) = 0;

    /// Called during the commit phase of the Storm texture system
    /// when the CPU texture is committed to the GPU.
    virtual void Commit(HdStDynamicUvTextureObject *textureObject) = 0;

    /// Queried by, e.g., the material system to determine whether
    /// to use, e.g., the fallback value of a texture node.
    virtual bool IsValid(const HdStDynamicUvTextureObject *textureObject) = 0;
};

WABI_NAMESPACE_END

#endif
