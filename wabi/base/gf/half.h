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
#ifndef WABI_BASE_GF_HALF_H
#define WABI_BASE_GF_HALF_H

/// \file gf/half.h
///
/// This header serves to simply bring in the half float datatype and
/// provide a hash_value function.  For documentation, of the half type,
/// please see the half header in ilmbase_half.h.

#include "wabi/base/gf/ilmbase_half.h"
#include "wabi/base/gf/ilmbase_halfLimits.h"
#include "wabi/base/gf/traits.h"
#include "wabi/wabi.h"

#include <type_traits>

WABI_NAMESPACE_BEGIN

/// A 16-bit floating point data type.
using GfHalf = wabi_half::half;

namespace wabi_half
{
/// Overload hash_value for half.
template<typename Half>
inline typename std::enable_if<std::is_same<Half, half>::value, size_t>::type hash_value(const Half &h)
{
  return h.bits();
}
}  // namespace wabi_half

template<>
struct GfIsFloatingPoint<GfHalf> : public std::integral_constant<bool, true>
{
};

WABI_NAMESPACE_END

#endif  // WABI_BASE_GF_HALF_H
