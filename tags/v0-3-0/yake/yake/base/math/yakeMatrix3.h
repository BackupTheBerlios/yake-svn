/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_BASE_MATH_MATRIX3_H
#define YAKE_BASE_MATH_MATRIX3_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include "yakeMath.h"
#include "yakeVector3.h"

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.magic-software.com

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.


//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace math {
    /** A 3x3 matrix which can represent rotations around axes.
        @note
            <b>All the code is adapted from the Wild Magic 0.2 Matrix
            library (http://www.magic-software.com).</b>
        @par
            The coordinate system is assumed to be <b>right-handed</b>.
    */
    class YAKE_BASE_API Matrix3
    {
    public:
        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
		inline Matrix3 () {};
        inline Matrix3 (const real arr[3][3])
		{
			memcpy(m,arr,9*sizeof(real));
		}
        inline Matrix3 (const Matrix3& rkMatrix)
		{
			memcpy(m,rkMatrix.m,9*sizeof(real));
		}
        Matrix3 (real fEntry00, real fEntry01, real fEntry02,
                    real fEntry10, real fEntry11, real fEntry12,
                    real fEntry20, real fEntry21, real fEntry22)
		{
			m[0][0] = fEntry00;
			m[0][1] = fEntry01;
			m[0][2] = fEntry02;
			m[1][0] = fEntry10;
			m[1][1] = fEntry11;
			m[1][2] = fEntry12;
			m[2][0] = fEntry20;
			m[2][1] = fEntry21;
			m[2][2] = fEntry22;
		}

        // member access, allows use of construct mat[r][c]
        inline real* operator[] (int iRow) const
		{
			return (real*)m[iRow];
		}
        inline operator real* ()
		{
			return (real*)m[0];
		}
        Vector3 GetColumn (int iCol) const;
        void SetColumn(int iCol, const Vector3& vec);
        void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

        // assignment and comparison
        inline Matrix3& operator= (const Matrix3& rkMatrix)
		{
			memcpy(m,rkMatrix.m,9*sizeof(real));
			return *this;
		}
        bool operator== (const Matrix3& rkMatrix) const;
        inline bool operator!= (const Matrix3& rkMatrix) const
		{
			return !operator==(rkMatrix);
		}

        // arithmetic operations
        Matrix3 operator+ (const Matrix3& rkMatrix) const;
        Matrix3 operator- (const Matrix3& rkMatrix) const;
        Matrix3 operator* (const Matrix3& rkMatrix) const;
        Matrix3 operator- () const;

        // matrix * vector [3x3 * 3x1 = 3x1]
        Vector3 operator* (const Vector3& rkVector) const;

        // vector * matrix [1x3 * 3x3 = 1x3]
        friend Vector3 operator* (const Vector3& rkVector,
            const Matrix3& rkMatrix);

        // matrix * scalar
        Matrix3 operator* (real fScalar) const;

        // scalar * matrix
        friend Matrix3 operator* (real fScalar, const Matrix3& rkMatrix);

        // utilities
        Matrix3 Transpose () const;
        bool Inverse (Matrix3& rkInverse, real fTolerance = 1e-06) const;
        Matrix3 Inverse (real fTolerance = 1e-06) const;
        real Determinant () const;

        // singular value decomposition
        void SingularValueDecomposition (Matrix3& rkL, Vector3& rkS,
            Matrix3& rkR) const;
        void SingularValueComposition (const Matrix3& rkL,
            const Vector3& rkS, const Matrix3& rkR);

        // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
        void Orthonormalize ();

        // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDUDecomposition (Matrix3& rkQ, Vector3& rkD,
            Vector3& rkU) const;

        real SpectralNorm () const;

        // matrix must be orthonormal
        void ToAxisAngle (Vector3& rkAxis, real& rfRadians) const;
        void FromAxisAngle (const Vector3& rkAxis, real fRadians);

        // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
        // where yaw is rotation about the Up vector, pitch is rotation about the
        // Right axis, and roll is rotation about the Direction axis.
        bool ToEulerAnglesXYZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesXZY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesYXZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesYZX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesZXY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesZYX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        void FromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
        void FromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
        void FromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
        void FromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
        void FromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
        void FromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);

        // eigensolver, matrix must be symmetric
        void EigenSolveSymmetric (real afEigenvalue[3],
            Vector3 akEigenvector[3]) const;

        static void TensorProduct (const Vector3& rkU, const Vector3& rkV,
            Matrix3& rkProduct);

        static const real EPSILON;
        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;

    protected:
        // support for eigensolver
        void Tridiagonal (real afDiag[3], real afSubDiag[3]);
        bool QLAlgorithm (real afDiag[3], real afSubDiag[3]);

        // support for singular value decomposition
        static const real ms_fSvdEpsilon;
        static const int ms_iSvdMaxIterations;
        static void Bidiagonalize (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);
        static void GolubKahanStep (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);

        // support for spectral norm
        static real MaxCubicRoot (real afCoeff[3]);

        real m[3][3];

        // for faster access
        friend class Matrix4;
    };

} // math
} // yake

#endif // YAKE_BASE_MATH_MATRIX3_H
