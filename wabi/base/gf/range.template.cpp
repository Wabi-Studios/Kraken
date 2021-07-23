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

#include "wabi/base/gf/range{{ SUFFIX }}.h"
{% for S in SCALARS if S != SCL %
}
#include "wabi/base/gf/range{{ DIM }}{{ S[0] }}.h"
{
  % endfor %
}

#include "wabi/base/gf/math.h"
#include "wabi/base/gf/ostreamHelpers.h"
#include "wabi/base/tf/type.h"

#include <cfloat>
#include <ostream>

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<{{RNG}}>();
}

std::ostream &operator<<(std::ostream &out, {
  {
    RNG
  }
} const &r)
{
  return out << '[' << Gf_OstreamHelperP(r.GetMin()) << "..." << Gf_OstreamHelperP(r.GetMax()) << ']';
}

double
{
  {
    RNG
  }
}
::GetDistanceSquared({
  {
    MINMAXPARM
  }
} p) const
{
  double dist = 0.0;

  {
    % if DIM == 1 %
  }
  if (p < _min)
  {
    // p is left of box
    dist += GfSqr(_min - p);
  } else if (p > _max)
  {
    // p is right of box
    dist += GfSqr(p - _max);
  }
  { % endif % } {
    % if DIM >= 2 %
  }
  if (p[0] < _min[0])
  {
    // p is left of box
    dist += GfSqr(_min[0] - p[0]);
  } else if (p[0] > _max[0])
  {
    // p is right of box
    dist += GfSqr(p[0] - _max[0]);
  }
  if (p[1] < _min[1])
  {
    // p is front of box
    dist += GfSqr(_min[1] - p[1]);
  } else if (p[1] > _max[1])
  {
    // p is back of box
    dist += GfSqr(p[1] - _max[1]);
  }
  { % endif % } {
    % if DIM == 3 %
  }
  if (p[2] < _min[2])
  {
    // p is below of box
    dist += GfSqr(_min[2] - p[2]);
  } else if (p[2] > _max[2])
  {
    // p is above of box
    dist += GfSqr(p[2] - _max[2]);
  }
  {
    % endif %
  }

  return dist;
}
{
  % if DIM == 2 %
}

{{MINMAX}} {
  {
    RNG
  }
}
::GetCorner(size_t i) const
{
  if (i > 3)
  {
    TF_CODING_ERROR("Invalid corner %zu > 3.", i);
    return _min;
  }

  return {{MINMAX}}((i & 1 ? _max : _min)[0], (i & 2 ? _max : _min)[1]);
}

{{RNG}} {
  {
    RNG
  }
}
::GetQuadrant(size_t i) const
{
  if (i > 3)
  {
    TF_CODING_ERROR("Invalid quadrant %zu > 3.", i);
    return {{RNG}}();
  }

  {
    {
      MINMAX
    }
  }
  a = GetCorner(i);
  {
    {
      MINMAX
    }
  }
  b = .5 * (_min + _max);

  return {{RNG}}({{MINMAX}}(GfMin(a[0], b[0]), GfMin(a[1], b[1])),
                 {{MINMAX}}(GfMax(a[0], b[0]), GfMax(a[1], b[1])));
}

const {{RNG}}
{
  {
    RNG
  }
}
::UnitSquare({{MINMAX}}(0, 0), {{MINMAX}}(1, 1));
{ % elif DIM == 3 % }

{{MINMAX}} {
  {
    RNG
  }
}
::GetCorner(size_t i) const
{
  if (i > 7)
  {
    TF_CODING_ERROR("Invalid corner %zu > 7.", i);
    return _min;
  }
  return {{MINMAX}}((i & 1 ? _max : _min)[0], (i & 2 ? _max : _min)[1], (i & 4 ? _max : _min)[2]);
}

{{RNG}} {
  {
    RNG
  }
}
::GetOctant(size_t i) const
{
  if (i > 7)
  {
    TF_CODING_ERROR("Invalid octant %zu > 7.", i);
    return {{RNG}}();
  }

  {
    {
      MINMAX
    }
  }
  a = GetCorner(i);
  {
    {
      MINMAX
    }
  }
  b = .5 * (_min + _max);

  return {{RNG}}({{MINMAX}}(GfMin(a[0], b[0]), GfMin(a[1], b[1]), GfMin(a[2], b[2])),
                 {{MINMAX}}(GfMax(a[0], b[0]), GfMax(a[1], b[1]), GfMax(a[2], b[2])));
}

const {{RNG}}
{
  {
    RNG
  }
}
::UnitCube({{MINMAX}}(0, 0, 0), {{MINMAX}}(1, 1, 1));
{ % endif % }

WABI_NAMESPACE_END
