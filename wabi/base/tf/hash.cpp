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

#include "wabi/wabi.h"
#include "wabi/base/tf/hash.h"
#include "wabi/base/tf/enum.h"
#include "wabi/base/tf/stringUtils.h"
#include "wabi/base/tf/token.h"
#include "wabi/base/tf/type.h"
#include "wabi/base/arch/hash.h"

#include <cstring>

WABI_NAMESPACE_BEGIN

void Tf_HashState::_AppendBytes(char const *bytes, size_t numBytes)
{
  _state = ArchHash64(bytes, numBytes, _state);
  _didOne = true;
}

size_t TfHashCharPtr::operator()(const char *ptr) const
{
  return TfHash()(static_cast<const void *>(ptr));
}

size_t TfHashCString::operator()(const char *ptr) const
{
  return ptr ? ArchHash(ptr, std::strlen(ptr)) : 0;
}

bool TfEqualCString::operator()(const char *lhs, const char *rhs) const
{
  return std::strcmp(lhs, rhs) == 0;
}

WABI_NAMESPACE_END
