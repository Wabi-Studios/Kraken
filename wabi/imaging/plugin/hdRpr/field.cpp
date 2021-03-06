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

#include "field.h"
#include "renderParam.h"

WABI_NAMESPACE_BEGIN

HdRprField::HdRprField(SdfPath const &id) : HdField(id) {}

void HdRprField::Sync(HdSceneDelegate *sceneDelegate,
                      HdRenderParam *renderParam,
                      HdDirtyBits *dirtyBits)
{
  if (*dirtyBits & DirtyParams) {
    auto rprRenderParam = static_cast<HdRprRenderParam *>(renderParam);
    rprRenderParam->NotifyVolumesAboutFieldChange(sceneDelegate, GetId());
  }

  *dirtyBits = DirtyBits::Clean;
}

HdDirtyBits HdRprField::GetInitialDirtyBitsMask() const
{
  return DirtyBits::DirtyParams;
}

WABI_NAMESPACE_END
