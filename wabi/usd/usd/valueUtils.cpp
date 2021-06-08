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

#include "wabi/usd/usd/valueUtils.h"

WABI_NAMESPACE_BEGIN

void Usd_MergeTimeSamples(std::vector<double> *const timeSamples,
                          const std::vector<double> &additionalTimeSamples,
                          std::vector<double> *tempUnionTimeSamples)
{
  std::vector<double> temp;
  if (!tempUnionTimeSamples)
    tempUnionTimeSamples = &temp;

  tempUnionTimeSamples->resize(timeSamples->size() + additionalTimeSamples.size());

  const auto &it = std::set_union(timeSamples->begin(),
                                  timeSamples->end(),
                                  additionalTimeSamples.begin(),
                                  additionalTimeSamples.end(),
                                  tempUnionTimeSamples->begin());
  tempUnionTimeSamples->resize(std::distance(tempUnionTimeSamples->begin(), it));
  timeSamples->swap(*tempUnionTimeSamples);
}

// Apply the offset to the value if it's holding the templated type.
template<class T>
static bool _TryApplyLayerOffsetToValue(VtValue *value, const SdfLayerOffset &offset)
{
  if (value->IsHolding<T>()) {
    T v;
    value->UncheckedSwap(v);
    Usd_ApplyLayerOffsetToValue(&v, offset);
    value->UncheckedSwap(v);
    return true;
  }
  return false;
}

void Usd_ApplyLayerOffsetToValue(VtValue *value, const SdfLayerOffset &offset)
{
  // Try applying the offset for each of our supported value types.
  _TryApplyLayerOffsetToValue<SdfTimeCode>(value, offset) ||
      _TryApplyLayerOffsetToValue<VtArray<SdfTimeCode>>(value, offset) ||
      _TryApplyLayerOffsetToValue<VtDictionary>(value, offset) ||
      _TryApplyLayerOffsetToValue<SdfTimeSampleMap>(value, offset);
}

WABI_NAMESPACE_END
