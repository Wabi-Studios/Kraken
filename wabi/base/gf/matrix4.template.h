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
// matrix4.template.h file to make changes.

{ % extends "matrix.template.h" % }

{
  % block includes %
}
#include "wabi/base/gf/homogeneous.h"
#include "wabi/base/gf/limits.h"
#include "wabi/base/gf/math.h"
#include "wabi/base/gf/vec3{{ SCL[0] }}.h"
{ % endblock % }

{
  % block forwardDeclarations %
}
class GfQuat
{
  {
    SCL[0]
  }
};
class GfRotation;
class GfMatrix3
{
  {
    SCL[0]
  }
};
{ % endblock % }

{ % block classDocs % }  /// <h3>3D Transformations</h3>
                         ///
                         /// The following methods interpret a {{ MAT }} as a 3D
                         /// transformation: SetRotate(), SetScale(), SetTranslate(), SetLookAt(),
                         /// Factor(), ExtractTranslation(), ExtractRotation(), Transform(), TransformDir().
                         /// By convention, vectors are treated primarily as row vectors,
                         /// implying the following:
                         /// \li Transformation matrices are organized to deal with row
                         ///        vectors, not column vectors. For example, the last row of a matrix
                         ///        contains the translation amounts.
                         /// \li Each of the Set() methods below completely rewrites the
                         ///        matrix; for example, SetTranslate() yields a matrix
                         ///        which does nothing but translate.
                         /// \li When multiplying two transformation matrices, the matrix
                         ///        on the left applies a more local transformation to a row
                         ///        vector. For example, if R represents a rotation
                         ///        matrix and T represents a translation matrix, the
                         ///        product R*T will rotate a row vector, then translate
                         ///        it.
{ % endblock classDocs % }

{ % block customConstructors % } {% for S in SCALARS %
}
/// Constructor.  Initialize the matrix from {{ DIM }} row vectors of
/// {{ S }}.  Each vector is expected to length {{ DIM }}.  If it is too
/// big, only the first {{ DIM }} items will be used.  If it is too small,
/// uninitialized elements will be filled in with the
/// corresponding elements from an identity matrix.
///
GF_API
explicit {{MAT}}(const std::vector<{{S}}> &r0,
                 const std::vector<{{S}}> &r1,
                 const std::vector<{{S}}> &r2,
                 const std::vector<{{S}}> &r3);

{ % endfor % }  /// Constructor. Initializes a transformation matrix to perform the
                /// indicated rotation and translation.
GF_API{{MAT}}(const GfRotation &rotate, const GfVec3{{SCL[0]}} & translate);

/// Constructor. Initializes a transformation matrix to perform the
/// indicated rotation and translation.
GF_API{{MAT}}(const GfMatrix3{{SCL[0]}} & rotmx, const GfVec3{{SCL[0]}} & translate);
{ % endblock customConstructors % }

{
  % block customFunctions %
}
/// Sets a row of the matrix from a Vec3.
/// The fourth element of the row is ignored.
void SetRow3(int i, const GfVec3{{SCL[0]}} & v)
{
  _mtx[i][0] = v[0];
  _mtx[i][1] = v[1];
  _mtx[i][2] = v[2];
}

/// Gets a row of the matrix as a Vec3.
GfVec3
{
  {
    SCL[0]
  }
}
GetRow3(int i) const
{
  return GfVec3{{SCL[0]}}(_mtx[i][0], _mtx[i][1], _mtx[i][2]);
}

/// Returns the determinant of the upper 3x3 matrix. This method is useful
/// when the matrix describes a linear transformation such as a rotation or
/// scale because the other values in the 4x4 matrix are not important.
double GetDeterminant3() const
{
  return _GetDeterminant3(0, 1, 2, 0, 1, 2);
}

/// Returns true, if the row vectors of the upper 3x3 matrix form an
/// orthogonal basis. Note they do not have to be unit length for this
/// test to return true.
bool HasOrthogonalRows3() const
{
  // XXX Should add GfAreOrthogonal(v0, v1, v2) (which also
  //     GfRotation::Decompose() could use).
  GfVec3
  {
    {
      SCL[0]
    }
  }
  axis0(GetRow3(0)), axis1(GetRow3(1)), axis2(GetRow3(2));
  return (GfAbs(GfDot(axis0, axis1)) < GF_MIN_ORTHO_TOLERANCE &&
          GfAbs(GfDot(axis0, axis2)) < GF_MIN_ORTHO_TOLERANCE &&
          GfAbs(GfDot(axis1, axis2)) < GF_MIN_ORTHO_TOLERANCE);
}

/// Makes the matrix orthonormal in place. This is an iterative method
/// that is much more stable than the previous cross/cross method.  If the
/// iterative method does not converge, a warning is issued.
///
/// Returns true if the iteration converged, false otherwise.  Leaves any
/// translation part of the matrix unchanged.  If \a issueWarning is true,
/// this method will issue a warning if the iteration does not converge,
/// otherwise it will be silent.
GF_API
bool Orthonormalize(bool issueWarning = true);

/// Returns an orthonormalized copy of the matrix.
GF_API
{
  {
    MAT
  }
}
GetOrthonormalized(bool issueWarning = true) const;

/// Returns the sign of the determinant of the upper 3x3 matrix, i.e. 1
/// for a right-handed matrix, -1 for a left-handed matrix, and 0 for a
/// singular matrix.
GF_API
double GetHandedness() const;

/// Returns true if the vectors in the upper 3x3 matrix form a
/// right-handed coordinate system.
bool IsRightHanded() const
{
  return GetHandedness() == 1.0;
}

/// Returns true if the vectors in the upper 3x3 matrix form a left-handed
/// coordinate system.
bool IsLeftHanded() const
{
  return GetHandedness() == -1.0;
}
{ % endblock customFunctions % }

  { % block customXformFunctions % }  /// Sets matrix to specify a uniform scaling by \e scaleFactor.
  GF_API{{MAT}} &
  SetScale({
    {
      SCL
    }
  } scaleFactor);

/// Returns the matrix with any scaling or shearing removed,
/// leaving only the rotation and translation.
/// If the matrix cannot be decomposed, returns the original matrix.
GF_API
{
  {
    MAT
  }
}
RemoveScaleShear() const;

/// \name 3D Transformation Utilities
/// @{

/// Sets the matrix to specify a rotation equivalent to \e rot,
/// and clears the translation.
GF_API{{MAT}} & SetRotate(const GfQuat{{SCL[0]}} & rot);

/// Sets the matrix to specify a rotation equivalent to \e rot,
/// without clearing the translation.
GF_API{{MAT}} & SetRotateOnly(const GfQuat{{SCL[0]}} & rot);

/// Sets the matrix to specify a rotation equivalent to \e rot,
/// and clears the translation.
GF_API{{MAT}} & SetRotate(const GfRotation &rot);

/// Sets the matrix to specify a rotation equivalent to \e rot,
/// without clearing the translation.
GF_API{{MAT}} & SetRotateOnly(const GfRotation &rot);

/// Sets the matrix to specify a rotation equivalent to \e mx,
/// and clears the translation.
GF_API{{MAT}} & SetRotate(const GfMatrix3{{SCL[0]}} & mx);

/// Sets the matrix to specify a rotation equivalent to \e mx,
/// without clearing the translation.
GF_API{{MAT}} & SetRotateOnly(const GfMatrix3{{SCL[0]}} & mx);

/// Sets the matrix to specify a nonuniform scaling in x, y, and z by
/// the factors in vector \e scaleFactors.
GF_API{{MAT}} & SetScale(const GfVec3{{SCL[0]}} & scaleFactors);

/// Sets matrix to specify a translation by the vector \e trans,
/// and clears the rotation.
GF_API{{MAT}} & SetTranslate(const GfVec3{{SCL[0]}} & trans);

/// Sets matrix to specify a translation by the vector \e trans,
/// without clearing the rotation.
GF_API{{MAT}} & SetTranslateOnly(const GfVec3{{SCL[0]}} & t);

/// Sets matrix to specify a rotation by \e rotate and a
/// translation by \e translate.
GF_API{{MAT}} & SetTransform(const GfRotation &rotate, const GfVec3{{SCL[0]}} & translate);

/// Sets matrix to specify a rotation by \e rotmx and a
/// translation by \e translate.
GF_API{{MAT}} & SetTransform(const GfMatrix3{{SCL[0]}} & rotmx, const GfVec3{{SCL[0]}} & translate);

/// Sets the matrix to specify a viewing matrix from parameters
/// similar to those used by <c>gluLookAt(3G)</c>. \e eyePoint
/// represents the eye point in world space. \e centerPoint
/// represents the world-space center of attention. \e upDirection
/// is a vector indicating which way is up.
GF_API{{MAT}} & SetLookAt(const GfVec3{{SCL[0]}} & eyePoint,
                          const GfVec3{{SCL[0]}} & centerPoint,
                          const GfVec3{{SCL[0]}} & upDirection);

/// Sets the matrix to specify a viewing matrix from a world-space
/// \e eyePoint and a world-space rotation that rigidly rotates the
/// orientation from its canonical frame, which is defined to be
/// looking along the <c>-z</c> axis with the <c>+y</c> axis as the up
/// direction.
GF_API{{MAT}} & SetLookAt(const GfVec3{{SCL[0]}} & eyePoint, const GfRotation &orientation);

/// Factors the matrix into 5 components:
/// \li <c>\e M = r * s * -r * u * t</c>
/// where
/// \li \e t is a translation.
/// \li \e u and \e r are rotations, and \e -r is the transpose
///     (inverse) of \e r. The \e u matrix may contain shear
///     information.
/// \li \e s is a scale.
/// Any projection information could be returned in matrix \e p,
/// but currently p is never modified.
///
/// Returns \c false if the matrix is singular (as determined by \e eps).
/// In that case, any zero scales in \e s are clamped to \e eps
/// to allow computation of \e u.
GF_API
bool Factor(
  {{MAT}} * r,
  GfVec3{{SCL[0]}} * s,
  {{MAT}} * u,
  GfVec3{{SCL[0]}} * t,
  {{MAT}} * p,
  { % if SCL == 'double' % } {
    {
      SCL
    }
  } eps = 1e-10) const;
{ % else % } {
  {
    SCL
  }
} eps = 1e-5) const;
{ % endif % }

/// Returns the translation part of the matrix, defined as the first three
/// elements of the last row.
GfVec3
{
  {
    SCL[0]
  }
}
ExtractTranslation() const
{
  return GfVec3{{SCL[0]}}(_mtx[3][0], _mtx[3][1], _mtx[3][2]);
}

/// Returns the rotation corresponding to this matrix. This works well
/// only if the matrix represents a rotation.
///
/// For good results, consider calling Orthonormalize() before calling
/// this method.
GF_API
GfRotation ExtractRotation() const;

/// Return the rotation corresponding to this matrix as a quaternion.
/// This works well only if the matrix represents a rotation.
///
/// For good results, consider calling Orthonormalize() before calling
/// this method.
GF_API
GfQuat
{
  {
    SCL[0]
  }
}
ExtractRotationQuat() const;

/// Decompose the rotation corresponding to this matrix about 3 orthogonal
/// axes.  If the axes are not orthogonal, warnings will be spewed.
///
/// This is a convenience method that is equivalent to calling
/// ExtractRotation().Decompose().
GF_API
GfVec3
{
  {
    SCL[0]
  }
}
DecomposeRotation(const GfVec3{{SCL[0]}} & axis0,
                  const GfVec3{{SCL[0]}} & axis1,
                  const GfVec3{{SCL[0]}} & axis2) const;

/// Returns the rotation corresponding to this matrix. This works well
/// only if the matrix represents a rotation.
///
/// For good results, consider calling Orthonormalize() before calling
/// this method.
GF_API
GfMatrix3
{
  {
    SCL[0]
  }
}
ExtractRotationMatrix() const;

/// Transforms the row vector \e vec by the matrix, returning the result.
/// This treats the vector as a 4-component vector whose fourth component
/// is 1.
GfVec3d Transform(const GfVec3d &vec) const
{
  return GfProject(GfVec4d({{LIST("vec[%(i)s] * _mtx[%(i)s][0]", sep = " + ", num = 3)}} + _mtx[3][0],
                           {{LIST("vec[%(i)s] * _mtx[%(i)s][1]", sep = " + ", num = 3)}} + _mtx[3][1],
                           {{LIST("vec[%(i)s] * _mtx[%(i)s][2]", sep = " + ", num = 3)}} + _mtx[3][2],
                           {{LIST("vec[%(i)s] * _mtx[%(i)s][3]", sep = " + ", num = 3)}} + _mtx[3][3]));
}

/// Transforms the row vector \e vec by the matrix, returning the result.
/// This treats the vector as a 4-component vector whose fourth component
/// is 1. This is an overloaded method; it differs from the other version
/// in that it returns a different value type.
GfVec3f Transform(const GfVec3f &vec) const
{
  {
    % if SCL == 'float' %
  }
        return (GfProject(GfVec4f(
{
    % else %}
        return GfVec3f(GfProject(GfVec4d(
{% endif %}
            {{ LIST("vec[%(i)s] * _mtx[%(i)s][0]", sep=" + ", num=3) }} + _mtx[3][0],
            {{ LIST("vec[%(i)s] * _mtx[%(i)s][1]", sep=" + ", num=3) }} + _mtx[3][1],
            {{ LIST("vec[%(i)s] * _mtx[%(i)s][2]", sep=" + ", num=3) }} + _mtx[3][2],
            {{ LIST("vec[%(i)s] * _mtx[%(i)s][3]", sep=" + ", num=3) }} + _mtx[3][3])));
}

/// Transforms row vector \e vec by the matrix, returning the result. This
/// treats the vector as a direction vector, so the translation
/// information in the matrix is ignored. That is, it treats the vector as
/// a 4-component vector whose fourth component is 0.
GfVec3d TransformDir(const GfVec3d &vec) const
{
  return GfVec3d({{LIST("vec[%(i)s] * _mtx[%(i)s][0]", sep = " + ", num = 3)}},
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][1]", sep = " + ", num = 3)}},
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][2]", sep = " + ", num = 3)}});
}

/// Transforms row vector \e vec by the matrix, returning the result. This
/// treats the vector as a direction vector, so the translation
/// information in the matrix is ignored. That is, it treats the vector as
/// a 4-component vector whose fourth component is 0.  This is an
/// overloaded method; it differs from the other version in that it
/// returns a different value type.
GfVec3f TransformDir(const GfVec3f &vec) const
{
  return GfVec3f({{LIST("vec[%(i)s] * _mtx[%(i)s][0]", sep = " + ", num = 3)}},
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][1]", sep = " + ", num = 3)}},
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][2]", sep = " + ", num = 3)}});
}

/// Transforms the row vector \e vec by the matrix, returning the result.
/// This treats the vector as a 4-component vector whose fourth component
/// is 1 and ignores the fourth column of the matrix (i.e. assumes it is
/// (0, 0, 0, 1)).
GfVec3d TransformAffine(const GfVec3d &vec) const
{
  return GfVec3d({{LIST("vec[%(i)s] * _mtx[%(i)s][0]", sep = " + ", num = 3)}} + _mtx[3][0],
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][1]", sep = " + ", num = 3)}} + _mtx[3][1],
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][2]", sep = " + ", num = 3)}} + _mtx[3][2]);
}

/// Transforms the row vector \e vec by the matrix, returning the result.
/// This treats the vector as a 4-component vector whose fourth component
/// is 1 and ignores the fourth column of the matrix (i.e. assumes it is
/// (0, 0, 0, 1)).
GfVec3f TransformAffine(const GfVec3f &vec) const
{
  return GfVec3f({{LIST("vec[%(i)s] * _mtx[%(i)s][0]", sep = " + ", num = 3)}} + _mtx[3][0],
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][1]", sep = " + ", num = 3)}} + _mtx[3][1],
                 {{LIST("vec[%(i)s] * _mtx[%(i)s][2]", sep = " + ", num = 3)}} + _mtx[3][2]);
}
/// @}

private:
/// Returns the determinant of the 3x3 submatrix specified by the three
/// given row and column indices (0-3 for each).
GF_API
double _GetDeterminant3(size_t row1, size_t row2, size_t row3, size_t col1, size_t col2, size_t col3) const;

/// Diagonalizes the upper 3x3 matrix of a matrix known to be symmetric.
void _Jacobi3(GfVec3d *eigenvalues, GfVec3d eigenvectors[3]) const;

/// Set the 3x3 submatrix to the rotation given by a quaternion,
/// defined by the real component \p r and imaginary components \p i.
void _SetRotateFromQuat(
  {
    {
      SCL
    }
  } r,
  const GfVec3{{SCL[0]}} & i);

{
  % endblock customXformFunctions %
}
