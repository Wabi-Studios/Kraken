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
/// \file glRawContext.cpp

#include "wabi/imaging/glf/glRawContext.h"

WABI_NAMESPACE_BEGIN

//
// GlfGLRawContext
//

GlfGLRawContextSharedPtr GlfGLRawContext::New()
{
  return GlfGLRawContextSharedPtr(new GlfGLRawContext(GarchGLPlatformContextState()));
}

GlfGLRawContextSharedPtr GlfGLRawContext::New(const GarchGLPlatformContextState &state)
{
  return GlfGLRawContextSharedPtr(new GlfGLRawContext(state));
}

GlfGLRawContext::GlfGLRawContext(const GarchGLPlatformContextState &state)
  : _state(state)
{
  // Do nothing
}

GlfGLRawContext::~GlfGLRawContext()
{
  // Do nothing
}

bool GlfGLRawContext::IsValid() const
{
  return _state.IsValid();
}

void GlfGLRawContext::_MakeCurrent()
{
  _state.MakeCurrent();
}

bool GlfGLRawContext::_IsSharing(const GlfGLContextSharedPtr &rhs) const
{
  return false;
}

bool GlfGLRawContext::_IsEqual(const GlfGLContextSharedPtr &rhs) const
{
  if (const GlfGLRawContext *rhsRaw = dynamic_cast<const GlfGLRawContext *>(rhs.get()))
  {
    return _state == rhsRaw->_state;
  }
  return false;
}

WABI_NAMESPACE_END
