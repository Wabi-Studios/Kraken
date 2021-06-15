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
#include "wabi/imaging/hdPh/field.h"
#include "wabi/imaging/hdPh/fieldSubtextureIdentifier.h"

#include "wabi/imaging/hd/sceneDelegate.h"

#include "wabi/usd/sdf/assetPath.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_PRIVATE_TOKENS(_tokens,
                         (fieldIndex)(fieldPurpose)(textureMemory)

                           (openvdbAsset)(field3dAsset));

HdPhField::HdPhField(SdfPath const &id, TfToken const &fieldType)
  : HdField(id),
    _fieldType(fieldType),
    _textureMemory(0),
    _isInitialized(false)
{}

HdPhField::~HdPhField() = default;

void HdPhField::Sync(HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam, HdDirtyBits *dirtyBits)
{
  if (*dirtyBits & DirtyParams) {

    // Get asset path from scene delegate.
    //
    const VtValue filePathValue = sceneDelegate->Get(GetId(), HdFieldTokens->filePath);
    const SdfAssetPath filePath = filePathValue.Get<SdfAssetPath>();

    // Resolve asset path
    //
    // Assuming that correct resolve context is set when HdPhField::Sync is
    // called.
    const TfToken resolvedFilePath = TfToken(filePath.GetResolvedPath());

    const VtValue fieldNameValue = sceneDelegate->Get(GetId(), HdFieldTokens->fieldName);
    const TfToken &fieldName = fieldNameValue.Get<TfToken>();

    const VtValue fieldIndexValue = sceneDelegate->Get(GetId(), _tokens->fieldIndex);

    const int fieldIndex = fieldIndexValue.Get<int>();

    if (_fieldType == _tokens->openvdbAsset) {
      _textureId = HdPhTextureIdentifier(
        resolvedFilePath, std::make_unique<HdPhOpenVDBAssetSubtextureIdentifier>(fieldName, fieldIndex));
    }
    else {
      const VtValue fieldPurposeValue = sceneDelegate->Get(GetId(), _tokens->fieldPurpose);
      const TfToken &fieldPurpose = fieldPurposeValue.Get<TfToken>();

      _textureId = HdPhTextureIdentifier(
        resolvedFilePath,
        std::make_unique<HdPhField3DAssetSubtextureIdentifier>(fieldName, fieldIndex, fieldPurpose));
    }

    const VtValue textureMemoryValue = sceneDelegate->Get(GetId(), _tokens->textureMemory);
    _textureMemory = 1048576 * textureMemoryValue.GetWithDefault<float>(0.0f);

    if (_isInitialized) {
      // Force volume prim to pick up the new field resource and
      // recompute bounding box.
      //
      // XXX:-matthias
      // Ideally, this would be more fine-grained than blasting all
      // rprims.
      HdChangeTracker &changeTracker = sceneDelegate->GetRenderIndex().GetChangeTracker();
      changeTracker.MarkAllRprimsDirty(HdChangeTracker::DirtyVolumeField);
    }
  }

  _isInitialized = true;

  *dirtyBits = Clean;
}

HdDirtyBits HdPhField::GetInitialDirtyBitsMask() const
{
  return DirtyBits::AllDirty;
}

const TfTokenVector &HdPhField::GetSupportedBprimTypes()
{
  static const TfTokenVector result = {_tokens->openvdbAsset, _tokens->field3dAsset};
  return result;
}

bool HdPhField::IsSupportedBprimType(const TfToken &bprimType)
{
  return bprimType == _tokens->openvdbAsset || bprimType == _tokens->field3dAsset;
}

WABI_NAMESPACE_END
