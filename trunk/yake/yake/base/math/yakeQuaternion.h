/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
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
*/
#ifndef YAKE_BASE_MATH_QUATERNION_H
#define YAKE_BASE_MATH_QUATERNION_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include "yakeMath.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace math
{

	class Matrix3;
	class Vector3;

	class YAKE_BASE_API Quaternion
	{
	public:
        inline Quaternion (
            real fW = 1.0,
            real fX = 0.0, real fY = 0.0, real fZ = 0.0)
		{
			w = fW;
			x = fX;
			y = fY;
			z = fZ;
		}
        inline Quaternion (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
		}

        void FromRotationMatrix (const Matrix3& kRot);
        void ToRotationMatrix (Matrix3& kRot) const;

        void FromAngleAxis (const real& rfAngle, const Vector3& rkAxis);
        void ToAngleAxis (real& rfAngle, Vector3& rkAxis) const;
        void FromAxes (const Vector3* akAxis);
        void FromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        //void ToAxes (Vector3* akAxis) const;
        void ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

        inline Quaternion& operator= (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
        Quaternion operator+ (const Quaternion& rkQ) const;
        Quaternion operator- (const Quaternion& rkQ) const;
        Quaternion operator* (const Quaternion& rkQ) const;
        Quaternion operator* (real fScalar) const;
        friend Quaternion operator* (real fScalar,
            const Quaternion& rkQ);
        Quaternion operator- () const;
        bool operator== (const Quaternion& rhs) const;
	bool operator!= (const Quaternion& rhs) const;


        // functions of a quaternion
        real Dot (const Quaternion& rkQ) const;  // dot product
        real Norm () const;  // squared-length
        Quaternion Inverse () const;  // apply to non-zero quaternion
        Quaternion UnitInverse () const;  // apply to unit-length quaternion
        //Quaternion Exp () const;
        //Quaternion Log () const;

        // rotation of a vector by a quaternion
        Vector3 operator* (const Vector3& rkVector) const;

		/*
        // spherical linear interpolation
        static Quaternion Slerp (real fT, const Quaternion& rkP,
            const Quaternion& rkQ, bool shortestPath = false);

        static Quaternion SlerpExtraSpins (real fT,
            const Quaternion& rkP, const Quaternion& rkQ,
            int iExtraSpins);

        // setup for spherical quadratic interpolation
        static void Intermediate (const Quaternion& rkQ0,
            const Quaternion& rkQ1, const Quaternion& rkQ2,
            Quaternion& rka, Quaternion& rkB);

        // spherical quadratic interpolation
        static Quaternion Squad (real fT, const Quaternion& rkP,
            const Quaternion& rkA, const Quaternion& rkB,
            const Quaternion& rkQ, bool shortestPath = false);
		*/

        // cutoff for sine near zero
        static const real ms_fEpsilon;

        // special values
        static const Quaternion kZero;
        static const Quaternion kIdentity;

        real w, x, y, z;
	};

} // math
} // base
} // yake

#endif // YAKE_BASE_MATH_QUATERNION_H


