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
// vec.template.h file to make changes.

#ifndef WABI_BASE_GF_VEC2F_H
#define WABI_BASE_GF_VEC2F_H

/// \file gf/vec2f.h
/// \ingroup group_gf_LinearAlgebra

#include "wabi/base/gf/api.h"
#include "wabi/base/gf/limits.h"
#include "wabi/base/gf/math.h"
#include "wabi/base/gf/traits.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/wabi.h"

#include <boost/functional/hash.hpp>

#include <cmath>
#include <cstddef>

#include <iosfwd>

WABI_NAMESPACE_BEGIN

class GfVec2f;

template<> struct GfIsGfVec<class GfVec2f> {
  static const bool value = true;
};

/// \class GfVec2f
/// \ingroup group_gf_LinearAlgebra
///
/// Basic type for a vector of 2 float components.
///
/// Represents a vector of 2 components of type \c float.
/// It is intended to be fast and simple.
///
class GfVec2f {
 public:
  /// Scalar element type and dimension.
  typedef float ScalarType;
  static const size_t dimension = 2;

  /// Default constructor does no initialization.
  GfVec2f() = default;

  /// Initialize all elements to a single value.
  constexpr explicit GfVec2f(float value) : _data{value, value}
  {}

  /// Initialize all elements with explicit arguments.
  constexpr GfVec2f(float s0, float s1) : _data{s0, s1}
  {}

  /// Construct with pointer to values.
  template<class Scl> constexpr explicit GfVec2f(Scl const *p) : _data{p[0], p[1]}
  {}

  /// Construct from GfVec2d.
  explicit GfVec2f(class GfVec2d const &other);

  /// Implicitly convert from GfVec2h.
  GfVec2f(class GfVec2h const &other);

  /// Implicitly convert from GfVec2i.
  GfVec2f(class GfVec2i const &other);

  /// Create a unit vector along the X-axis.
  static GfVec2f XAxis()
  {
    GfVec2f result(0);
    result[0] = 1;
    return result;
  }
  /// Create a unit vector along the Y-axis.
  static GfVec2f YAxis()
  {
    GfVec2f result(0);
    result[1] = 1;
    return result;
  }

  /// Create a unit vector along the i-th axis, zero-based.  Return the zero
  /// vector if \p i is greater than or equal to 2.
  static GfVec2f Axis(size_t i)
  {
    GfVec2f result(0);
    if (i < 2)
      result[i] = 1;
    return result;
  }

  /// Set all elements with passed arguments.
  GfVec2f &Set(float s0, float s1)
  {
    _data[0] = s0;
    _data[1] = s1;
    return *this;
  }

  /// Set all elements with a pointer to data.
  GfVec2f &Set(float const *a)
  {
    return Set(a[0], a[1]);
  }

  /// Direct data access.
  float const *data() const
  {
    return _data;
  }
  float *data()
  {
    return _data;
  }
  float const *GetArray() const
  {
    return data();
  }

  /// Indexing.
  float const &operator[](size_t i) const
  {
    return _data[i];
  }
  float &operator[](size_t i)
  {
    return _data[i];
  }

  /// Hash.
  friend inline size_t hash_value(GfVec2f const &vec)
  {
    size_t h = 0;
    boost::hash_combine(h, vec[0]);
    boost::hash_combine(h, vec[1]);
    return h;
  }

  /// Equality comparison.
  bool operator==(GfVec2f const &other) const
  {
    return _data[0] == other[0] && _data[1] == other[1];
  }
  bool operator!=(GfVec2f const &other) const
  {
    return !(*this == other);
  }

  // TODO Add inequality for other vec types...
  /// Equality comparison.
  GF_API
  bool operator==(class GfVec2d const &other) const;
  /// Equality comparison.
  GF_API
  bool operator==(class GfVec2h const &other) const;
  /// Equality comparison.
  GF_API
  bool operator==(class GfVec2i const &other) const;

  /// Create a vec with negated elements.
  GfVec2f operator-() const
  {
    return GfVec2f(-_data[0], -_data[1]);
  }

  /// Addition.
  GfVec2f &operator+=(GfVec2f const &other)
  {
    _data[0] += other[0];
    _data[1] += other[1];
    return *this;
  }
  friend GfVec2f operator+(GfVec2f const &l, GfVec2f const &r)
  {
    return GfVec2f(l) += r;
  }

  /// Subtraction.
  GfVec2f &operator-=(GfVec2f const &other)
  {
    _data[0] -= other[0];
    _data[1] -= other[1];
    return *this;
  }
  friend GfVec2f operator-(GfVec2f const &l, GfVec2f const &r)
  {
    return GfVec2f(l) -= r;
  }

  /// Multiplication by scalar.
  GfVec2f &operator*=(double s)
  {
    _data[0] *= s;
    _data[1] *= s;
    return *this;
  }
  GfVec2f operator*(double s) const
  {
    return GfVec2f(*this) *= s;
  }
  friend GfVec2f operator*(double s, GfVec2f const &v)
  {
    return v * s;
  }

  /// Division by scalar.
  // TODO should divide by the scalar type.
  GfVec2f &operator/=(double s)
  {
    // TODO This should not multiply by 1/s, it should do the division.
    // Doing the division is more numerically stable when s is close to
    // zero.
    return *this *= (1.0 / s);
  }
  GfVec2f operator/(double s) const
  {
    return *this * (1.0 / s);
  }

  /// See GfDot().
  float operator*(GfVec2f const &v) const
  {
    return _data[0] * v[0] + _data[1] * v[1];
  }

  /// Returns the projection of \p this onto \p v. That is:
  /// \code
  /// v * (*this * v)
  /// \endcode
  GfVec2f GetProjection(GfVec2f const &v) const
  {
    return v * (*this * v);
  }

  /// Returns the orthogonal complement of \p this->GetProjection(b).
  /// That is:
  /// \code
  ///  *this - this->GetProjection(b)
  /// \endcode
  GfVec2f GetComplement(GfVec2f const &b) const
  {
    return *this - this->GetProjection(b);
  }

  /// Squared length.
  float GetLengthSq() const
  {
    return *this * *this;
  }

  /// Length
  float GetLength() const
  {
    // TODO should use GfSqrt.
    return sqrt(GetLengthSq());
  }

  /// Normalizes the vector in place to unit length, returning the
  /// length before normalization. If the length of the vector is
  /// smaller than \p eps, then the vector is set to vector/\c eps.
  /// The original length of the vector is returned. See also GfNormalize().
  ///
  /// \todo This was fixed for bug 67777. This is a gcc64 optimizer bug.
  /// By tickling the code, it no longer tries to write into
  /// an illegal memory address (in the code section of memory).
  float Normalize(float eps = GF_MIN_VECTOR_LENGTH)
  {
    // TODO this seems suspect...  suggest dividing by length so long as
    // length is not zero.
    float length = GetLength();
    *this /= (length > eps) ? length : eps;
    return length;
  }

  GfVec2f GetNormalized(float eps = GF_MIN_VECTOR_LENGTH) const
  {
    GfVec2f normalized(*this);
    normalized.Normalize(eps);
    return normalized;
  }

 private:
  float _data[2];
};

/// Output a GfVec2f.
/// \ingroup group_gf_DebuggingOutput
GF_API std::ostream &operator<<(std::ostream &, GfVec2f const &);

WABI_NAMESPACE_END

#include "wabi/base/gf/vec2d.h"
#include "wabi/base/gf/vec2h.h"
#include "wabi/base/gf/vec2i.h"

WABI_NAMESPACE_BEGIN

inline GfVec2f::GfVec2f(class GfVec2d const &other)
{
  _data[0] = other[0];
  _data[1] = other[1];
}
inline GfVec2f::GfVec2f(class GfVec2h const &other)
{
  _data[0] = other[0];
  _data[1] = other[1];
}
inline GfVec2f::GfVec2f(class GfVec2i const &other)
{
  _data[0] = other[0];
  _data[1] = other[1];
}

/// Returns component-wise multiplication of vectors \p v1 and \p v2.
inline GfVec2f GfCompMult(GfVec2f const &v1, GfVec2f const &v2)
{
  return GfVec2f(v1[0] * v2[0], v1[1] * v2[1]);
}

/// Returns component-wise quotient of vectors \p v1 and \p v2.
inline GfVec2f GfCompDiv(GfVec2f const &v1, GfVec2f const &v2)
{
  return GfVec2f(v1[0] / v2[0], v1[1] / v2[1]);
}

/// Returns the dot (inner) product of two vectors.
inline float GfDot(GfVec2f const &v1, GfVec2f const &v2)
{
  return v1 * v2;
}

/// Returns the geometric length of \c v.
inline float GfGetLength(GfVec2f const &v)
{
  return v.GetLength();
}

/// Normalizes \c *v in place to unit length, returning the length before
/// normalization. If the length of \c *v is smaller than \p eps then \c *v is
/// set to \c *v/eps.  The original length of \c *v is returned.
inline float GfNormalize(GfVec2f *v, float eps = GF_MIN_VECTOR_LENGTH)
{
  return v->Normalize(eps);
}

/// Returns a normalized (unit-length) vector with the same direction as \p v.
/// If the length of this vector is smaller than \p eps, the vector divided by
/// \p eps is returned.
inline GfVec2f GfGetNormalized(GfVec2f const &v, float eps = GF_MIN_VECTOR_LENGTH)
{
  return v.GetNormalized(eps);
}

/// Returns the projection of \p a onto \p b. That is:
/// \code
/// b * (a * b)
/// \endcode
inline GfVec2f GfGetProjection(GfVec2f const &a, GfVec2f const &b)
{
  return a.GetProjection(b);
}

/// Returns the orthogonal complement of \p a.GetProjection(b). That is:
/// \code
///  a - a.GetProjection(b)
/// \endcode
inline GfVec2f GfGetComplement(GfVec2f const &a, GfVec2f const &b)
{
  return a.GetComplement(b);
}

/// Tests for equality within a given tolerance, returning \c true if the
/// length of the difference vector is less than or equal to \p tolerance.
inline bool GfIsClose(GfVec2f const &v1, GfVec2f const &v2, double tolerance)
{
  GfVec2f delta = v1 - v2;
  return delta.GetLengthSq() <= tolerance * tolerance;
}

WABI_NAMESPACE_END

#endif  // WABI_BASE_GF_VEC2F_H
