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

#ifndef HDCYCLES_RENDERPASSSTATE_H
#define HDCYCLES_RENDERPASSSTATE_H

#include <wabi/imaging/hd/renderPassState.h>

WABI_NAMESPACE_BEGIN

class HdCyclesRenderDelegate;

class HdCyclesRenderPassState : public HdRenderPassState
{
 public:

  explicit HdCyclesRenderPassState(const HdCyclesRenderDelegate *renderDelegate);

  void Prepare(const HdResourceRegistrySharedPtr &resourceRegistry) override;

 private:

  const HdCyclesRenderDelegate *m_renderDelegate;
};

using HdCyclesRenderPassStateSharedPtr = std::shared_ptr<HdCyclesRenderPassState>;

WABI_NAMESPACE_END

#endif  // HDCYCLES_RENDERPASSSTATE_H
