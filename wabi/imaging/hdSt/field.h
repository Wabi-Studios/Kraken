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
#ifndef WABI_IMAGING_HD_ST_FIELD_H
#define WABI_IMAGING_HD_ST_FIELD_H

#include "wabi/wabi.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/imaging/hd/field.h"
#include "wabi/imaging/hdSt/api.h"

#include "wabi/imaging/hdSt/textureIdentifier.h"

WABI_NAMESPACE_BEGIN

///
/// Represents a Field Buffer Prim.
///
class HdStField : public HdField {
public:
    /// For now, only fieldType HdStTokens->openvdbAsset is supported.
    HDST_API
    HdStField(SdfPath const & id, TfToken const & fieldType);
    HDST_API
    ~HdStField() override;

    /// Loads field as 3d texture to generate GetFieldResource.
    HDST_API
    void Sync(HdSceneDelegate *sceneDelegate,
              HdRenderParam   *renderParam,
              HdDirtyBits     *dirtyBits) override;

    HDST_API
    HdDirtyBits GetInitialDirtyBitsMask() const override;

    /// Initialized by Sync.
    HDST_API
    HdStTextureIdentifier const &GetTextureIdentifier() const {
        return _textureId;
    }

    /// Get memory request for this field
    size_t GetTextureMemory() const { return _textureMemory; }

    /// Bprim types handled by this class
    HDST_API
    static const TfTokenVector &GetSupportedBprimTypes();

    /// Can bprim type be handled by this class
    HDST_API
    static bool IsSupportedBprimType(const TfToken &bprimType);

private:
    const TfToken _fieldType;

    HdStTextureIdentifier _textureId;
    size_t _textureMemory;

    bool _isInitialized : 1;
};

WABI_NAMESPACE_END

#endif //WABI_IMAGING_HD_ST_FIELD_H
