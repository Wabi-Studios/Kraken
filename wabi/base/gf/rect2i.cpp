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

#include "wabi/base/gf/rect2i.h"
#include "wabi/wabi.h"

#include "wabi/base/gf/ostreamHelpers.h"

#include "wabi/base/tf/type.h"

#include <ostream>

WABI_NAMESPACE_BEGIN

// CODE_COVERAGE_OFF_GCOV_BUG
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<GfRect2i>();
}
// CODE_COVERAGE_ON_GCOV_BUG

GfRect2i GfRect2i::GetNormalized() const
{
  GfVec2i min, max;

  if (_max[0] < _min[0]) {
    min[0] = _max[0];
    max[0] = _min[0];
  } else {
    min[0] = _min[0];
    max[0] = _max[0];
  }

  if (_max[1] < _min[1]) {
    min[1] = _max[1];
    max[1] = _min[1];
  } else {
    min[1] = _min[1];
    max[1] = _max[1];
  }

  return GfRect2i(min, max);
}

std::ostream &operator<<(std::ostream &out, const GfRect2i &r)
{
  return out << '[' << Gf_OstreamHelperP(r.GetMin()) << ":" << Gf_OstreamHelperP(r.GetMax())
             << ']';
}

WABI_NAMESPACE_END
