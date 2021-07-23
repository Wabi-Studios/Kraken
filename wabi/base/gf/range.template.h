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
// range.template.h file to make changes.

#ifndef WABI_BASE_GF_{{UPPER(RNG)[2 : ] } } _H
#define WABI_BASE_GF_ \
  {                   \
    {                 \
      UPPER(RNG)[2:]  \
    }                 \
  }                   \
  _H

/// \file gf/range{{ SUFFIX }}.h
/// \ingroup group_gf_BasicGeometry

#include "wabi/wabi.h"

#include "wabi/base/gf/api.h"
{
  % if DIM > 1 %
}
#include "wabi/base/gf/vec{{ DIM }}d.h"
#include "wabi/base/gf/vec{{ DIM }}f.h"
{
  % endif %
}
#include "wabi/base/gf/traits.h"

#include <boost/functional/hash.hpp>

#include <cfloat>
#include <cstddef>
#include <iosfwd>

WABI_NAMESPACE_BEGIN

class GfRange
{
  {
    DIM
  }
} d;
class GfRange
{
  {
    DIM
  }
} f;

template<>
struct GfIsGfRange<class {{RNG}}>
{
  static const bool value = true;
};

/// \class {{ RNG }}
/// \ingroup group_gf_BasicGeometry
///
/// Basic type: {{ DIM }}-dimensional floating point range.
///
/// This class represents a {{ DIM }}-dimensional range (or interval) All
/// operations are component-wise and conform to interval mathematics. An
/// empty range is one where max < min.
/// The default empty is [FLT_MAX,-FLT_MAX]
class
{
  {
    RNG
  }
}
{
 public:
  /// Helper typedef.
  typedef
  {
    {
      MINMAX
    }
  }
  MinMaxType;

  {
    % if DIM == 1 %
  }
  static const size_t dimension = 1;
  typedef MinMaxType ScalarType;
  {
    % else %
  }
  static const size_t dimension = { {MINMAX} } ::dimension;
  typedef
  {
    {
      MINMAX
    }
  }
  ::ScalarType ScalarType;
  {
    % endif %
  }

  /// Sets the range to an empty interval
  // TODO check whether this can be deprecated.
  void inline SetEmpty()
  {
    {
      % if DIM == 1 %
    }
    _min = FLT_MAX;
    _max = -FLT_MAX;
    { % else % } {{LIST("_min[%(i)s]", sep = " = ")}} = FLT_MAX;
    {{LIST("_max[%(i)s]", sep = " = ")}} = -FLT_MAX;
    {
      % endif %
    }
  }

  /// The default constructor creates an empty range.
  {{RNG}}()
  {
    SetEmpty();
  }

  /// This constructor initializes the minimum and maximum points.
  {{RNG}}(
    {
      {
        MINMAXPARM
      }
    } min,
    {
      {
        MINMAXPARM
      }
    } max)
    : _min(min),
      _max(max){}

  /// Returns the minimum value of the range.
  {
    {
      MINMAXPARM
    }
  }
  GetMin() const
  {
    return _min;
  }

  /// Returns the maximum value of the range.
  {
    {
      MINMAXPARM
    }
  }
  GetMax() const
  {
    return _max;
  }

  /// Returns the size of the range.
  {
    {
      MINMAX
    }
  }
  GetSize() const
  {
    return _max - _min;
  }

  /// Returns the midpoint of the range, that is, 0.5*(min+max).
  /// Note: this returns zero in the case of default-constructed ranges,
  /// or ranges set via SetEmpty().
  {
    {
      MINMAX
    }
  }
  GetMidpoint() const
  {
    return static_cast<ScalarType>(0.5) * _min + static_cast<ScalarType>(0.5) * _max;
  }

  /// Sets the minimum value of the range.
  void SetMin({
    {
      MINMAXPARM
    }
  } min)
  {
    _min = min;
  }

  /// Sets the maximum value of the range.
  void SetMax({
    {
      MINMAXPARM
    }
  } max)
  {
    _max = max;
  }

  /// Returns whether the range is empty (max < min).
  bool IsEmpty() const
  {
    {
      % if DIM == 1 %
    }
    return _min > _max;
    {
      % else %
    }
    return {{LIST("_min[%(i)s] > _max[%(i)s]", sep = " || ")}};
    {
      % endif %
    }
  }

  /// Modifies the range if necessary to surround the given value.
  /// \deprecated Use UnionWith() instead.
  void ExtendBy({
    {
      MINMAXPARM
    }
  } point)
  {
    UnionWith(point);
  }

  /// Modifies the range if necessary to surround the given range.
  /// \deprecated Use UnionWith() instead.
  void ExtendBy(const {{RNG}} & range)
  {
    UnionWith(range);
  }

  /// Returns true if the \p point is located inside the range. As with all
  /// operations of this type, the range is assumed to include its extrema.
  bool Contains({
    {
      MINMAXPARM
    }
  } point) const
  {
    {
      % if DIM == 1 %
    }
    return (point >= _min && point <= _max);
    {
      % else %
    }
    return (
      {{LIST("point[%(i)s] >= _min[%(i)s] && point[%(i)s] <= _max[%(i)s]", sep = "\n             && ")}});
    {
      % endif %
    }
  }

  /// Returns true if the \p range is located entirely inside the range. As
  /// with all operations of this type, the ranges are assumed to include
  /// their extrema.
  bool Contains(const {{RNG}} & range) const
  {
    return Contains(range._min) && Contains(range._max);
  }

  /// Returns true if the \p point is located inside the range. As with all
  /// operations of this type, the range is assumed to include its extrema.
  /// \deprecated Use Contains() instead.
  bool IsInside({
    {
      MINMAXPARM
    }
  } point) const
  {
    return Contains(point);
  }

  /// Returns true if the \p range is located entirely inside the range. As
  /// with all operations of this type, the ranges are assumed to include
  /// their extrema.
  /// \deprecated Use Contains() instead.
  bool IsInside(const {{RNG}} & range) const
  {
    return Contains(range);
  }

  /// Returns true if the \p range is located entirely outside the range. As
  /// with all operations of this type, the ranges are assumed to include
  /// their extrema.
  bool IsOutside(const {{RNG}} & range) const
  {
    {
      % if DIM == 1 %
    }
    return (range._max < _min || range._min > _max);
    {
      % else %
    }
    return ({{LIST("(range._max[%(i)s] < _min[%(i)s] || range._min[%(i)s] > _max[%(i)s])",
                   sep = "\n             || ")}});
    {
      % endif %
    }
  }

  /// Returns the smallest \c {{ RNG }} which contains both \p a and \p b.
  static
  {
    {
      RNG
    }
  }
  GetUnion(const {{RNG}} & a, const {{RNG}} & b)
  {
    {
      {
        RNG
      }
    }
    res = a;
    _FindMin(res._min, b._min);
    _FindMax(res._max, b._max);
    return res;
  }

  /// Extend \p this to include \p b.
  const {{RNG}} & UnionWith(const {{RNG}} & b)
  {
    _FindMin(_min, b._min);
    _FindMax(_max, b._max);
    return *this;
  }

  /// Extend \p this to include \p b.
  const {{RNG}} & UnionWith({
    {
      MINMAXPARM
    }
  } b)
  {
    _FindMin(_min, b);
    _FindMax(_max, b);
    return *this;
  }

  /// Returns the smallest \c {{ RNG }} which contains both \p a and \p b
  /// \deprecated Use GetUnion() instead.
  static
  {
    {
      RNG
    }
  }
  Union(const {{RNG}} & a, const {{RNG}} & b)
  {
    return GetUnion(a, b);
  }

  /// Extend \p this to include \p b.
  /// \deprecated Use UnionWith() instead.
  const {{RNG}} & Union(const {{RNG}} & b)
  {
    return UnionWith(b);
  }

  /// Extend \p this to include \p b.
  /// \deprecated Use UnionWith() instead.
  const {{RNG}} & Union({
    {
      MINMAXPARM
    }
  } b)
  {
    return UnionWith(b);
  }

  /// Returns a \c {{ RNG }} that describes the intersection of \p a and \p b.
  static
  {
    {
      RNG
    }
  }
  GetIntersection(const {{RNG}} & a, const {{RNG}} & b)
  {
    {
      {
        RNG
      }
    }
    res = a;
    _FindMax(res._min, b._min);
    _FindMin(res._max, b._max);
    return res;
  }

  /// Returns a \c {{ RNG }} that describes the intersection of \p a and \p b.
  /// \deprecated Use GetIntersection() instead.
  static
  {
    {
      RNG
    }
  }
  Intersection(const {{RNG}} & a, const {{RNG}} & b)
  {
    return GetIntersection(a, b);
  }

  /// Modifies this range to hold its intersection with \p b and returns the
  /// result
  const {{RNG}} & IntersectWith(const {{RNG}} & b)
  {
    _FindMax(_min, b._min);
    _FindMin(_max, b._max);
    return *this;
  }

  /// Modifies this range to hold its intersection with \p b and returns the
  /// result.
  /// \deprecated Use IntersectWith() instead.
  const {{RNG}} & Intersection(const {{RNG}} & b)
  {
    return IntersectWith(b);
  }

  /// unary sum.
  {
    {
      RNG
    }
  }
  operator+=(const {{RNG}} & b)
  {
    _min += b._min;
    _max += b._max;
    return *this;
  }

  /// unary difference.
  {
    {
      RNG
    }
  }
  operator-=(const {{RNG}} & b)
  {
    _min -= b._max;
    _max -= b._min;
    return *this;
  }

  /// unary multiply.
  {
    {
      RNG
    }
  }
  operator*=(double m)
  {
    if (m > 0)
    {
      _min *= m;
      _max *= m;
    } else
    {
      {
        {
          MINMAX
        }
      }
      tmp = _min;
      _min = _max * m;
      _max = tmp * m;
    }
    return *this;
  }

  /// unary division.
  {
    {
      RNG
    }
  }
  operator/=(double m)
  {
    return *this *= (1.0 / m);
  }

  /// binary sum.
  {
    {
      RNG
    }
  }
  operator+(const {{RNG}} & b) const
  {
    return {{RNG}}(_min + b._min, _max + b._max);
  }

  /// binary difference.
  {
    {
      RNG
    }
  }
  operator-(const {{RNG}} & b) const
  {
    return {{RNG}}(_min - b._max, _max - b._min);
  }

  /// scalar multiply.
  friend
  {
    {
      RNG
    }
  }
  operator*(double m, const {{RNG}} & r)
  {
    return (m > 0 ? {{RNG}}(r._min * m, r._max * m) : {{RNG}}(r._max * m, r._min * m));
  }

  /// scalar multiply.
  friend
  {
    {
      RNG
    }
  }
  operator*(const {{RNG}} & r, double m)
  {
    return (m > 0 ? {{RNG}}(r._min * m, r._max * m) : {{RNG}}(r._max * m, r._min * m));
  }

  /// scalar divide.
  friend
  {
    {
      RNG
    }
  }
  operator/(const {{RNG}} & r, double m)
  {
    return r * (1.0 / m);
  }

  /// hash.
  friend inline size_t hash_value(const {{RNG}} & r)
  {
    size_t h = 0;
    boost::hash_combine(h, r._min);
    boost::hash_combine(h, r._max);
    return h;
  }

  /// The min and max points must match exactly for equality.
  bool operator==(const {{RNG}} & b) const
  {
    return (_min == b._min && _max == b._max);
  }

  bool operator!=(const {{RNG}} & b) const
  {
    return !(*this == b);
  }

  {% for S in SCALARS if S != SCL %
  }
  /// Compare this range to a {{ RNGNAME(DIM, S) }}.
  ///
  /// The values must match exactly and it does exactly what you might
  /// expect when comparing float and double values.
  GF_API inline bool operator==(const {{RNGNAME(DIM, S)}} & other) const;
  GF_API inline bool operator!=(const {{RNGNAME(DIM, S)}} & other) const;
  { % endfor % }

  /// Compute the squared distance from a point to the range.
  GF_API double GetDistanceSquared({
    {
      MINMAXPARM
    }
  } p) const;

  {
    % if DIM == 2 %
  }
  /// Returns the ith corner of the range, in the following order:
  /// SW, SE, NW, NE.
  GF_API
  {
    {
      MINMAX
    }
  }
  GetCorner(size_t i) const;

  /// Returns the ith quadrant of the range, in the following order:
  /// SW, SE, NW, NE.
  GF_API
  {
    {
      RNG
    }
  }
  GetQuadrant(size_t i) const;

  /// The unit square.
  GF_API
  static const
  {
    {
      RNG
    }
  }
  UnitSquare;
  { % elif DIM == 3 % }  /// Returns the ith corner of the range, in the following order:
                         /// LDB, RDB, LUB, RUB, LDF, RDF, LUF, RUF. Where L/R is left/right,
                         /// D/U is down/up, and B/F is back/front.
  GF_API
  {
    {
      MINMAX
    }
  }
  GetCorner(size_t i) const;

  /// Returns the ith octant of the range, in the following order:
  /// LDB, RDB, LUB, RUB, LDF, RDF, LUF, RUF. Where L/R is left/right,
  /// D/U is down/up, and B/F is back/front.
  GF_API
  {
    {
      RNG
    }
  }
  GetOctant(size_t i) const;

  /// The unit cube.
  GF_API
  static const
  {
    {
      RNG
    }
  }
  UnitCube;
  {
    % endif %
  }

 private:
  /// Minimum and maximum points.
  {
    {
      MINMAX
    }
  }
  _min, _max;

  /// Extends minimum point if necessary to contain given point.
  static void _FindMin({{MINMAX}} & dest, {
    {
      MINMAXPARM
    }
  } point)
  {
    {
      % if DIM == 1 %
    }
    if (point < dest)
      dest = point;
    { %
      else % } {{LIST("if (point[%(i)s] < dest[%(i)s]) dest[%(i)s] = point[%(i)s];", sep = "\n        ")}} {
      % endif %
    }
  }

  /// Extends maximum point if necessary to contain given point.
  static void _FindMax({{MINMAX}} & dest, {
    {
      MINMAXPARM
    }
  } point)
  {
    {
      % if DIM == 1 %
    }
    if (point > dest)
      dest = point;
    { %
      else % } {{LIST("if (point[%(i)s] > dest[%(i)s]) dest[%(i)s] = point[%(i)s];", sep = "\n        ")}} {
      % endif %
    }
  }
};

/// Output a {{ RNG }}.
/// \ingroup group_gf_DebuggingOutput
GF_API std::ostream &operator<<(std::ostream &, {
  {
    RNG
  }
} const &);

{% for S in SCALARS if S != SCL %
}
WABI_NAMESPACE_END
#include "wabi/base/gf/range{{ DIM }}{{ S[0] }}.h"
WABI_NAMESPACE_BEGIN{ % endfor % }

{% for S in SCALARS if S != SCL %
}
inline bool
{
  {
    RNG
  }
}
::operator==(const {{RNGNAME(DIM, S)}} & other) const
{
  return _min == {{MINMAX}}(other.GetMin()) && _max == {{MINMAX}}(other.GetMax());
}

inline bool
{
  {
    RNG
  }
}
::operator!=(const {{RNGNAME(DIM, S)}} & other) const
{
  return !(*this == other);
}

{ % endfor % }

WABI_NAMESPACE_END

#endif  // WABI_BASE_GF_{{ UPPER(RNG)[2:] }}_H
