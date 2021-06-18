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
// quat.template.h file to make changes.

#ifndef WABI_BASE_GF_QUATF_H
#define WABI_BASE_GF_QUATF_H

/// \file gf/quatf.h
/// \ingroup group_gf_LinearAlgebra

#include "wabi/base/gf/api.h"
#include "wabi/base/gf/declare.h"
#include "wabi/base/gf/traits.h"
#include "wabi/base/gf/vec3f.h"
#include "wabi/wabi.h"

#include <boost/functional/hash.hpp>

#include <iosfwd>

WABI_NAMESPACE_BEGIN

template<>
struct GfIsGfQuat<class GfQuatf>
{
  static const bool value = true;
};

/// Return the dot (inner) product of two quaternions.
float GfDot(const GfQuatf &q1, const GfQuatf &q2);

/// \class GfQuatf
/// \ingroup group_gf_LinearAlgebra
///
/// Basic type: a quaternion, a complex number with a real coefficient and
/// three imaginary coefficients, stored as a 3-vector.
///
class GfQuatf
{
 public:
  typedef float ScalarType;
  typedef GfVec3f ImaginaryType;

  /// Default constructor leaves the quaternion undefined.
  GfQuatf()
  {}

  /// Initialize the real coefficient to \p realVal and the imaginary
  /// coefficients to zero.
  ///
  /// Since quaternions typically must be normalized, reasonable values for
  /// \p realVal are -1, 0, or 1.  Other values are legal but are likely to
  /// be meaningless.
  ///
  explicit GfQuatf(float realVal)
    : _imaginary(0),
      _real(realVal)
  {}

  /// Initialize the real and imaginary coefficients.
  GfQuatf(float real, float i, float j, float k)
    : _imaginary(i, j, k),
      _real(real)
  {}

  /// Initialize the real and imaginary coefficients.
  GfQuatf(float real, const GfVec3f &imaginary)
    : _imaginary(imaginary),
      _real(real)
  {}

  /// Construct from GfQuatd.
  GF_API
  explicit GfQuatf(class GfQuatd const &other);
  /// Implicitly convert from GfQuath.
  GF_API
  GfQuatf(class GfQuath const &other);

  /// Return the identity quaternion, with real coefficient 1 and an
  /// imaginary coefficients all zero.
  static GfQuatf GetIdentity()
  {
    return GfQuatf(1.0);
  }

  /// Return the real coefficient.
  float GetReal() const
  {
    return _real;
  }

  /// Set the real coefficient.
  void SetReal(float real)
  {
    _real = real;
  }

  /// Return the imaginary coefficient.
  const GfVec3f &GetImaginary() const
  {
    return _imaginary;
  }

  /// Set the imaginary coefficients.
  void SetImaginary(const GfVec3f &imaginary)
  {
    _imaginary = imaginary;
  }

  /// Set the imaginary coefficients.
  void SetImaginary(float i, float j, float k)
  {
    _imaginary.Set(i, j, k);
  }

  /// Return geometric length of this quaternion.
  float GetLength() const
  {
    return GfSqrt(_GetLengthSquared());
  }

  /// length of this quaternion is smaller than \p eps, return the identity
  /// quaternion.
  GfQuatf GetNormalized(float eps = GF_MIN_VECTOR_LENGTH) const
  {
    GfQuatf ret(*this);
    ret.Normalize(eps);
    return ret;
  }

  /// Normalizes this quaternion in place to unit length, returning the
  /// length before normalization. If the length of this quaternion is
  /// smaller than \p eps, this sets the quaternion to identity.
  GF_API
  float Normalize(float eps = GF_MIN_VECTOR_LENGTH);

  /// Return this quaternion's conjugate, which is the quaternion with the
  /// same real coefficient and negated imaginary coefficients.
  GfQuatf GetConjugate() const
  {
    return GfQuatf(GetReal(), -GetImaginary());
  }

  /// Return this quaternion's inverse, or reciprocal.  This is the
  /// quaternion's conjugate divided by it's squared length.
  GfQuatf GetInverse() const
  {
    return GetConjugate() / _GetLengthSquared();
  }

  /// Transform the GfVec3f point. If the quaternion is normalized,
  /// the transformation is a rotation. Given a GfQuatf q, q.Transform(point)
  /// is equivalent to:
  ///
  ///     (q * GfQuatf(0, point) * q.GetInverse()).GetImaginary()
  ///
  /// but is more efficient.
  GF_API
  GfVec3f Transform(const GfVec3f &point) const;

  /// Hash.
  friend inline size_t hash_value(const GfQuatf &q)
  {
    size_t h = boost::hash<ScalarType>()(q.GetReal());
    boost::hash_combine(h, q.GetImaginary());
    return h;
  }

  /// Component-wise negation.
  GfQuatf operator-() const
  {
    return GfQuatf(-GetReal(), -GetImaginary());
  }

  /// Component-wise quaternion equality test. The real and imaginary parts
  /// must match exactly for quaternions to be considered equal.
  bool operator==(const GfQuatf &q) const
  {
    return (GetReal() == q.GetReal() && GetImaginary() == q.GetImaginary());
  }

  /// Component-wise quaternion inequality test. The real and imaginary
  /// parts must match exactly for quaternions to be considered equal.
  bool operator!=(const GfQuatf &q) const
  {
    return !(*this == q);
  }

  /// Post-multiply quaternion \p q into this quaternion.
  GF_API
  GfQuatf &operator*=(const GfQuatf &q);

  /// Multiply this quaternion's coefficients by \p s.
  GfQuatf &operator*=(float s)
  {
    _real *= s;
    _imaginary *= s;
    return *this;
  }

  /// Divide this quaternion's coefficients by \p s.
  GfQuatf &operator/=(float s)
  {
    _real /= s;
    _imaginary /= s;
    return *this;
  }

  /// Add quaternion \p q to this quaternion.
  GfQuatf &operator+=(const GfQuatf &q)
  {
    _real += q._real;
    _imaginary += q._imaginary;
    return *this;
  }

  /// Component-wise unary difference operator.
  GfQuatf &operator-=(const GfQuatf &q)
  {
    _real -= q._real;
    _imaginary -= q._imaginary;
    return *this;
  }

  /// Component-wise binary sum operator.
  friend GfQuatf operator+(const GfQuatf &q1, const GfQuatf &q2)
  {
    return GfQuatf(q1) += q2;
  }

  /// Component-wise binary difference operator.
  friend GfQuatf operator-(const GfQuatf &q1, const GfQuatf &q2)
  {
    return GfQuatf(q1) -= q2;
  }

  /// Returns the product of quaternions \p q1 and \p q2.
  friend GfQuatf operator*(const GfQuatf &q1, const GfQuatf &q2)
  {
    return GfQuatf(q1) *= q2;
  }

  /// Returns the product of quaternion \p q and scalar \p s.
  friend GfQuatf operator*(const GfQuatf &q, float s)
  {
    return GfQuatf(q) *= s;
  }

  /// Returns the product of quaternion \p q and scalar \p s.
  friend GfQuatf operator*(float s, const GfQuatf &q)
  {
    return GfQuatf(q) *= s;
  }

  /// Returns the product of quaternion \p q and scalar 1 / \p s.
  friend GfQuatf operator/(const GfQuatf &q, float s)
  {
    return GfQuatf(q) /= s;
  }

 private:
  /// Imaginary part
  GfVec3f _imaginary;

  /// Real part
  float _real;

  /// Returns the square of the length
  float _GetLengthSquared() const
  {
    return GfDot(*this, *this);
  }
};

/// Spherically linearly interpolate between \p q0 and \p q1.
///
/// If the interpolant \p alpha is zero, then the result is \p q0, while
/// \p alpha of one yields \p q1.
GF_API GfQuatf GfSlerp(double alpha, const GfQuatf &q0, const GfQuatf &q1);

GF_API GfQuatf GfSlerp(const GfQuatf &q0, const GfQuatf &q1, double alpha);

inline float GfDot(GfQuatf const &q1, GfQuatf const &q2)
{
  return GfDot(q1.GetImaginary(), q2.GetImaginary()) + q1.GetReal() * q2.GetReal();
}

/// Output a GfQuatd using the format (re, i, j, k)
/// \ingroup group_gf_DebuggingOutput
GF_API std::ostream &operator<<(std::ostream &, GfQuatf const &);

WABI_NAMESPACE_END

#endif  // WABI_BASE_GF_QUATF_H
