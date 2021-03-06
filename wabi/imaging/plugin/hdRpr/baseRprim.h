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

#ifndef HDRPR_BASE_RPRIM_H
#define HDRPR_BASE_RPRIM_H

#include "rprApi.h"

#include "renderDelegate.h"
#include "renderParam.h"

#include "wabi/imaging/hd/sceneDelegate.h"

WABI_NAMESPACE_BEGIN

template<typename Base> class HdRprBaseRprim : public Base
{
 public:

  HdRprBaseRprim(SdfPath const &id HDRPR_INSTANCER_ID_ARG_DECL) : Base(id HDRPR_INSTANCER_ID_ARG)
  {}
  ~HdRprBaseRprim() override = default;

  void Finalize(HdRenderParam *renderParam) override
  {
    if (!m_materialId.IsEmpty()) {
      auto rprRenderParam = static_cast<HdRprRenderParam *>(renderParam);
      rprRenderParam->UnsubscribeFromMaterialUpdates(m_materialId, Base::GetId());
    }

    Base::Finalize(renderParam);
  }

 protected:

  void UpdateMaterialId(HdSceneDelegate *sceneDelegate, HdRprRenderParam *renderParam)
  {
    auto newMaterialId = sceneDelegate->GetMaterialId(Base::GetId());
    if (m_materialId != newMaterialId) {
      if (!m_materialId.IsEmpty()) {
        renderParam->UnsubscribeFromMaterialUpdates(m_materialId, Base::GetId());
      }
      renderParam->SubscribeForMaterialUpdates(newMaterialId, Base::GetId());

      m_materialId = newMaterialId;
    }
  }

  void UpdateVisibility(HdSceneDelegate *sceneDelegate)
  {
    // XXX (Hydra): HdRprim::CanSkipDirtyBitPropagationAndSync decides whether Rprim can be skipped
    // from syncing. In short, when Rprim is invisible this function tells HdRenderIndex::SyncAll
    // to skip the primitive. Ideally, that's what we want but not in the case of RPR. If the
    // invisible Rprim has active material reference and the referenced material was changed we
    // must resync the Rprim. As there is no way to change the behavior of
    // HdRprim::CanSkipDirtyBitPropagationAndSync (it's not virtual) to not skip when material was
    // changed, we make our Rprim to be always visible for this function - it uses
    // HdRprim::IsVisible to check the visibility.
    this->_sharedData.visible = true;

    m_isVisible = sceneDelegate->GetVisible(Base::GetId());
  }

  uint32_t GetVisibilityMask() const
  {
    if (!m_isVisible) {
      // If Rprim is explicitly made invisible, ignore custom visibility mask
      return kInvisible;
    }

    return m_visibilityMask;
  }

 protected:

  SdfPath m_materialId;
  bool m_isVisible = false;
  uint32_t m_visibilityMask = 0;
};

WABI_NAMESPACE_END

#endif  // HDRPR_BASE_RPRIM_H
