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

#ifndef WABI_BASE_TRACE_KEY_H
#define WABI_BASE_TRACE_KEY_H

#include "wabi/base/trace/staticKeyData.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
/// \class TraceKey
///
/// A wrapper around a TraceStaticKeyData pointer that is stored in TraceEvent
/// instances.
///
class TraceKey {
 public:
  /// Constructor.
  constexpr TraceKey(const TraceStaticKeyData &data) : _ptr(&data)
  {}

  /// Equality comparison.
  bool operator==(const TraceKey &other) const
  {
    if (_ptr == other._ptr) {
      return true;
    }
    else {
      return *_ptr == *other._ptr;
    }
  }

  /// Hash function.
  size_t Hash() const
  {
    return reinterpret_cast<size_t>(_ptr) / sizeof(TraceStaticKeyData);
  }

  /// A Hash functor which may be used to store keys in a TfHashMap.
  struct HashFunctor {
    size_t operator()(const TraceKey &key) const
    {
      return key.Hash();
    }
  };

 private:
  const TraceStaticKeyData *_ptr;

  // TraceCollection converts TraceKeys to TfTokens for visitors.
  friend class TraceCollection;
};

WABI_NAMESPACE_END

#endif  // WABI_BASE_TRACE_KEY_H
