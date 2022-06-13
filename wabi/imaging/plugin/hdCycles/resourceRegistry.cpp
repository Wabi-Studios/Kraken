//  Copyright 2021 Tangent Animation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
//  including without limitation, as related to merchantability and fitness
//  for a particular purpose.
//
//  In no event shall any copyright holder be liable for any damages of any kind
//  arising from the use of this software, whether in contract, tort or otherwise.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "resourceRegistry.h"
#include "renderDelegate.h"
#include "renderParam.h"

#include <wabi/base/work/loops.h>
#include <wabi/usd/sdf/path.h>

#include <render/object.h>
#include <render/scene.h>

WABI_NAMESPACE_USING

HdCyclesResourceRegistry::HdCyclesResourceRegistry(HdCyclesRenderDelegate *renderDelegate)
  : m_renderDelegate{renderDelegate}
{}

void HdCyclesResourceRegistry::_Commit()
{
  // This function is under heavy wip. In ideal situation committing all resources to cycles should
  // happen in one place only.

  auto scene = m_renderDelegate->GetCyclesRenderParam()->GetCyclesScene();
  ccl::thread_scoped_lock scene_lock{scene->mutex};
  // TODO: acquire display lock

  // State used to control session/scene update reset
  std::atomic_bool requires_reset{false};

  // * bind objects to the scene
  for (auto &object_source : m_objects) {
    if (!object_source.second.value->IsValid()) {
      continue;
    }

    if (object_source.second.value->IsResolved()) {
      continue;
    }

    object_source.second.value->Resolve();
  }

  // * commit all pending object resources
  using ValueType = HdInstanceRegistry<HdCyclesObjectSourceSharedPtr>::const_iterator::value_type;
  WorkParallelForEach(
    m_objects.begin(),
    m_objects.end(),
    [&requires_reset, scene](const ValueType &object_source) {
      // resolve per object
      size_t num_resolved_sources = object_source.second.value->ResolvePendingSources();
      if (num_resolved_sources > 0) {
        requires_reset = true;
      }
    });

  // * notify session that new resources have been committed and reset is required
  if (requires_reset) {
    // TODO: After we are done removing scene and session mutations from *::Sync. We can request
    // update and reset
  }
}

void HdCyclesResourceRegistry::_GarbageCollect()
{
  auto scene = m_renderDelegate->GetCyclesRenderParam()->GetCyclesScene();
  ccl::thread_scoped_lock scene_lock{scene->mutex};

  // delete unique objects
  m_objects.GarbageCollect();
}

HdInstance<HdCyclesObjectSourceSharedPtr> HdCyclesResourceRegistry::GetObjectInstance(
  const SdfPath &id)
{
  return m_objects.GetInstance(id.GetHash());
}
