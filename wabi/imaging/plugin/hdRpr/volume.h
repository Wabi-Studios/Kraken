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

#ifndef HDRPR_VOLUME_H
#define HDRPR_VOLUME_H

#include "wabi/base/gf/matrix4f.h"
#include "wabi/imaging/hd/volume.h"

WABI_NAMESPACE_BEGIN

struct HdRprApiVolume;

class HdRprVolume : public HdVolume
{
 public:

  HdRprVolume(SdfPath const &id);
  ~HdRprVolume() override = default;

  void Sync(HdSceneDelegate *sceneDelegate,
            HdRenderParam *renderParam,
            HdDirtyBits *dirtyBits,
            TfToken const &reprName) override;

  void Finalize(HdRenderParam *renderParam) override;

  HdDirtyBits GetInitialDirtyBitsMask() const override;

 protected:

  HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;

  void _InitRepr(TfToken const &reprName, HdDirtyBits *dirtyBits) override;

 private:

  HdRprApiVolume *m_rprVolume = nullptr;
  GfMatrix4f m_transform;

  std::map<SdfPath, std::shared_ptr<HdRprVolume>> m_fieldSubscriptions;
};

WABI_NAMESPACE_END

#endif  // HDRPR_VOLUME_H
