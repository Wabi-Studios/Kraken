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

#ifndef HDCYCLES_RESOURCEREGISTRY_H
#define HDCYCLES_RESOURCEREGISTRY_H

#include "objectSource.h"

#include <wabi/imaging/hd/bufferSource.h>
#include <wabi/imaging/hd/instanceRegistry.h>
#include <wabi/imaging/hd/resourceRegistry.h>

#include <tbb/concurrent_vector.h>

namespace ccl {
class Scene;
}

WABI_NAMESPACE_BEGIN

class HdCyclesRenderDelegate;

class HdCyclesResourceRegistry final : public HdResourceRegistry {
 public:
  explicit HdCyclesResourceRegistry(HdCyclesRenderDelegate *renderDelegate);

  HdInstance<HdCyclesObjectSourceSharedPtr> GetObjectInstance(const SdfPath &id);

 private:
  void _Commit() override;
  void _GarbageCollect() override;

  HdCyclesRenderDelegate *m_renderDelegate;
  HdInstanceRegistry<HdCyclesObjectSourceSharedPtr> m_objects;
};

using HdCyclesResourceRegistrySharedPtr = std::shared_ptr<HdCyclesResourceRegistry>;

WABI_NAMESPACE_END

#endif  // HDCYCLES_RESOURCEREGISTRY_H
