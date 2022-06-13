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
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// range.template.cpp file to make changes.

#include "wabi/base/gf/range1f.h"
#include "wabi/base/gf/range1d.h"

#include "wabi/base/gf/math.h"
#include "wabi/base/gf/ostreamHelpers.h"
#include "wabi/base/tf/type.h"

#include <cfloat>
#include <ostream>

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<GfRange1f>();
}

std::ostream &operator<<(std::ostream &out, GfRange1f const &r)
{
  return out << '[' << Gf_OstreamHelperP(r.GetMin()) << "..." << Gf_OstreamHelperP(r.GetMax())
             << ']';
}

double GfRange1f::GetDistanceSquared(float p) const
{
  double dist = 0.0;

  if (p < _min) {
    // p is left of box
    dist += GfSqr(_min - p);
  } else if (p > _max) {
    // p is right of box
    dist += GfSqr(p - _max);
  }

  return dist;
}

WABI_NAMESPACE_END
