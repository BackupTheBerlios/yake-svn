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
#ifndef YAKE_BASE_MATH_VECTOR3_H
#define YAKE_BASE_MATH_VECTOR3_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include "yakeMath.h"
#include "yakeQuaternion.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace math {

	/**	A 3d vector.
		Based on Ogre's code ( www.ogre3d.org ).
	*/
    class YAKE_BASE_API Vector3
    {
    public:
        real x, y, z;        

    public:
        inline Vector3( real fX = 0, real fY = 0, real fZ = 0) 
            : x( fX ), y( fY ), z( fZ )
        {
        }

        inline Vector3( real afCoordinate[3] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] ),
              z( afCoordinate[2] )
        {
        }

        inline Vector3( const real* const r )
            : x( r[0] ), y( r[1] ), z( r[2] )
        {
        }

        inline Vector3( const Vector3& rkVector )
            : x( rkVector.x ), y( rkVector.y ), z( rkVector.z )
        {
        }

        inline real operator [] ( unsigned i ) const
        {
            assert( i < 3 );

            return *(&x+i);
        }

		inline real& operator [] ( unsigned i )
        {
            assert( i < 3 );

            return *(&x+i);
        }

        inline Vector3& operator = ( const Vector3& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;
            z = rkVector.z;            

            return *this;
        }

        inline bool operator == ( const Vector3& rkVector ) const
        {
            return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
        }

        inline bool operator != ( const Vector3& rkVector ) const
        {
            return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
        }

        // arithmetic operations
        inline Vector3 operator + ( const Vector3& rkVector ) const
        {
            Vector3 kSum;

            kSum.x = x + rkVector.x;
            kSum.y = y + rkVector.y;
            kSum.z = z + rkVector.z;

            return kSum;
        }

        inline Vector3 operator - ( const Vector3& rkVector ) const
        {
            Vector3 kDiff;

            kDiff.x = x - rkVector.x;
            kDiff.y = y - rkVector.y;
            kDiff.z = z - rkVector.z;

            return kDiff;
        }

        inline Vector3 operator * ( real fScalar ) const
        {
            Vector3 kProd;

            kProd.x = fScalar*x;
            kProd.y = fScalar*y;
            kProd.z = fScalar*z;

            return kProd;
        }

        inline Vector3 operator * ( const Vector3& rhs) const
        {
            Vector3 kProd;

            kProd.x = rhs.x * x;
            kProd.y = rhs.y * y;
            kProd.z = rhs.z * z;

            return kProd;
        }

        inline Vector3 operator / ( real fScalar ) const
        {
            assert( fScalar != 0.0 );

            Vector3 kDiv;

            real fInv = (real) 1.0 / fScalar;
            kDiv.x = (real) x * fInv;
            kDiv.y = (real) y * fInv;
            kDiv.z = (real) z * fInv;

            return kDiv;
        }

        inline Vector3 operator - () const
        {
            Vector3 kNeg;

            kNeg.x = -x;
            kNeg.y = -y;
            kNeg.z = -z;

            return kNeg;
        }

        inline friend Vector3 operator * ( real fScalar, const Vector3& rkVector )
        {
            Vector3 kProd;

            kProd.x = fScalar * rkVector.x;
            kProd.y = fScalar * rkVector.y;
            kProd.z = fScalar * rkVector.z;

            return kProd;
        }

        // arithmetic updates
        inline Vector3& operator += ( const Vector3& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;
            z += rkVector.z;

            return *this;
        }

        inline Vector3& operator -= ( const Vector3& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;
            z -= rkVector.z;

            return *this;
        }

        inline Vector3& operator *= ( real fScalar )
        {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            return *this;
        }

        inline Vector3& operator /= ( real fScalar )
        {
            assert( fScalar != 0.0 );

            real fInv = (real) 1.0 / fScalar;

            x *= fInv;
            y *= fInv;
            z *= fInv;

            return *this;
        }

        /** Returns the length (magnitude) of the vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                length (e.g. for just comparing lengths) use squaredLength()
                instead.
        */
        inline real length () const
        {
            return Math::Sqrt( x * x + y * y + z * z );
        }

        /** Returns the square of the length(magnitude) of the vector.
            @remarks
                This  method is for efficiency - calculating the actual
                length of a vector requires a square root, which is expensive
                in terms of the operations required. This method returns the
                square of the length of the vector, i.e. the same as the
                length but before the square root is taken. Use this if you
                want to find the longest / shortest vector without incurring
                the square root.
        */
        inline real squaredLength () const
        {
            return x * x + y * y + z * z;
        }

        /** Calculates the dot (scalar) product of this vector with another.
            @remarks
                The dot product can be used to calculate the angle between 2
                vectors. If both are unit vectors, the dot product is the
                cosine of the angle; otherwise the dot product must be
                divided by the product of the lengths of both vectors to get
                the cosine of the angle. This result can further be used to
                calculate the distance of a point from a plane.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @returns
                A float representing the dot product value.
        */
        inline real dotProduct(const Vector3& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        /** Normalises the vector.
            @remarks
                This method normalises the vector such that it's
                length / magnitude is 1. The result is called a unit vector.
            @note
                This function will not crash for zero-sized vectors, but there
                will be no changes made to their components.
            @returns The previous length of the vector.
        */
        inline real normalise()
        {
            real fLength = Math::Sqrt( x * x + y * y + z * z );

            // Will also work for zero-sized vectors, but will change nothing
            if ( fLength > 1e-06 )
            {
                real fInvLength = (real)1.0 / fLength;
                x *= fInvLength;
                y *= fInvLength;
                z *= fInvLength;
            }

            return fLength;
        }

        /** Calculates the cross-product of 2 vectors, i.e. the vector that
            lies perpendicular to them both.
            @remarks
                The cross-product is normally used to calculate the normal
                vector of a plane, by calculating the cross-product of 2
                non-equivalent vectors which lie on the plane (e.g. 2 edges
                of a triangle).
            @param
                vec Vector which, together with this one, will be used to
                calculate the cross-product.
            @returns
                A vector which is the result of the cross-product. This
                vector will <b>NOT</b> be normalised, to maximise efficiency
                - call Vector3::normalise on the result if you wish this to
                be done. As for which side the resultant vector will be on, the
                returned vector will be on the side from which the arc from 'this'
                to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z) 
                = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
            @par
                For a clearer explanation, look a the left and the bottom edges
                of your monitor's screen. Assume that the first vector is the
                left edge and the second vector is the bottom edge, both of
                them starting from the lower-left corner of the screen. The
                resulting vector is going to be perpendicular to both of them
                and will go <i>inside</i> the screen, towards the cathode tube
                (assuming you're using a CRT monitor, of course).
        */
        inline Vector3 crossProduct( const Vector3& rkVector ) const
        {
            Vector3 kCross;

            kCross.x = y * rkVector.z - z * rkVector.y;
            kCross.y = z * rkVector.x - x * rkVector.z;
            kCross.z = x * rkVector.y - y * rkVector.x;

            return kCross;
        }

        /** As normalise, except that this vector is unaffected and the
            normalised vector is returned as a copy. */
        inline Vector3 normalisedCopy(void) const
        {
            Vector3 ret = *this;
            ret.normalise();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal . 
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        inline Vector3 reflect(const Vector3& normal)
        {
            return Vector3( ( 2 * this->dotProduct(normal) * normal ) - *this );
        }

        /** Gets the shortest arc quaternion to rotate this vector to the destination
            vector. 
        @remarks
            Don't call this if you think the dest vector can be close to the inverse
            of this vector, since then ANY axis of rotation is ok. 
        */
        Quaternion getRotationTo(const Vector3& dest) const
        {
            // Based on Stan Melax's article in Game Programming Gems
            Quaternion q;
            // Copy, since cannot modify local
            Vector3 v0 = *this;
            Vector3 v1 = dest;
            v0.normalise();
            v1.normalise();

            Vector3 c = v0.crossProduct(v1);

            // NB if the crossProduct approaches zero, we get unstable because ANY axis will do
            // when v0 == -v1
            real d = v0.dotProduct(v1);
            // If dot == 1, vectors are the same
            if (d >= 1.0f)
            {
                return Quaternion::kIdentity;
            }
            real s = Math::Sqrt( (1+d)*2 );
            real invs = 1 / s;


            q.x = c.x * invs;
            q.y = c.y * invs;
            q.z = c.z * invs;
            q.w = s * real(0.5);
            return q;
        }

		/** Guess what... taken from OGRE again... */
		inline Vector3 perpendicular(void) const
		{
			static const real fSquareZero = real( 1e-06 * 1e-06 );
			Vector3 perp = this->crossProduct( Vector3::kUnitX );

			// Check length
			if( perp.squaredLength() < fSquareZero )
			{
				/* This vector is the Y axis multiplied by a scalar, so we have 
				to use another axis.
				*/
				perp = this->crossProduct( Vector3::kUnitY );
			}

			return perp;
		}

		// special vectors
        static const Vector3 kZero;
        static const Vector3 kUnitX;
        static const Vector3 kUnitY;
        static const Vector3 kUnitZ;
        static const Vector3 kUnitScale;

    };
	inline ::std::ostream& operator << ( ::std::ostream& lhs, const Vector3 & rhs )
	{
		lhs << rhs.x << " " << rhs.y << " " << rhs.z;
		return lhs;
	}

} // math
} // yake

#endif // YAKE_BASE_MATH_VECTOR3_H


