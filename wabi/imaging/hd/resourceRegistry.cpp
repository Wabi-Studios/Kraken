/*
 * Copyright 2021 Pixar. All Rights Reserved.
 *
 * Portions of this file are derived from original work by Pixar
 * distributed with Universal Scene Description, a project of the
 * Academy Software Foundation (ASWF). https://www.aswf.io/
 *
 * Licensed under the Apache License, Version 2.0 (the "Apache License")
 * with the following modification; you may not use this file except in
 * compliance with the Apache License and the following modification:
 * Section 6. Trademarks. is deleted and replaced with:
 *
 * 6. Trademarks. This License does not grant permission to use the trade
 *    names, trademarks, service marks, or product names of the Licensor
 *    and its affiliates, except as required to comply with Section 4(c)
 *    of the License and to reproduce the content of the NOTICE file.
 *
 * You may obtain a copy of the Apache License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Apache License with the above modification is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the Apache License for the
 * specific language governing permissions and limitations under the
 * Apache License.
 *
 * Modifications copyright (C) 2020-2021 Wabi.
 */
#include "wabi/imaging/hd/resourceRegistry.h"
#include "wabi/imaging/hd/tokens.h"

WABI_NAMESPACE_BEGIN

HdResourceRegistry::HdResourceRegistry()
{}

HdResourceRegistry::~HdResourceRegistry()
{}

void HdResourceRegistry::GarbageCollect()
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  HD_PERF_COUNTER_INCR(HdPerfTokens->garbageCollected);

  // Prompt derived registries to collect their garbage.
  _GarbageCollect();
}

void HdResourceRegistry::Commit()
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // Prompt derived registries to commit in-flight data.
  _Commit();
}

void HdResourceRegistry::InvalidateShaderRegistry()
{
  // Derived classes that hold shaders will override this,
  // but the base registry has nothing to do.
}

void HdResourceRegistry::ReloadResource(TfToken const &resourceType, std::string const &path)
{}

VtDictionary HdResourceRegistry::GetResourceAllocation() const
{
  return VtDictionary();
}

void HdResourceRegistry::_Commit()
{}

void HdResourceRegistry::_GarbageCollect()
{}

WABI_NAMESPACE_END
