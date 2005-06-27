/*
   ------------------------------------------------------------------------------------
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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_BASE_MATH_MATH_H
#define YAKE_BASE_MATH_MATH_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include "yakeRand.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace math {

/** Math routines.
	Please note: The math code is taken from Ogre (www.ogre3d.org).
*/
class YAKE_BASE_API Math
{
public:
	/** The angular units used by the API.
	@remarks
		By default, YAKE uses degrees in all it's external APIs.
	*/       
	enum AngleUnit
	{
		AU_DEGREE,
		AU_RADIAN
	};

protected:
	// angle units used by the api
	static AngleUnit msAngleUnit;

	/// Size of the trig tables as determined by constructor.
	static int mTrigTableSize;

	/// Radian -> index factor value ( mTrigTableSize / 2 * PI )
	static real mTrigTableFactor;
	static real* mSinTable;
	static real* mTanTable;

	/** Private function to build trig tables.
	*/
	void buildTrigTables();

	static real SinTable (real fValue);
	static real TanTable (real fValue);
public:
	/** Default constructor.
	@param
		trigTableSize Optional parameter to set the size of the
		tables used to implement Sin, Cos, Tan
	*/
	Math(unsigned int trigTableSize = 4096);

	/** Default destructor.
	*/
	~Math();

	static inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
	static inline int ICeil (real fValue) { return int(ceil(fValue)); }
	static inline int IFloor (real fValue) { return int(floor(fValue)); }
	static int ISign (int iValue);

	static inline real Abs (real fValue) { return real(fabs(fValue)); }
	static real ACos (real fValue);
	static real ASin (real fValue);
	static inline real ATan (real fValue) { return real(atan(fValue)); }
	static inline real ATan2 (real fY, real fX) { return real(atan2(fY,fX)); }
	static inline real Ceil (real fValue) { return real(ceil(fValue)); }

	/** Cosine function.
		@param
			fValue Angle in radians
		@param
			useTables If true, uses lookup tables rather than
			calculation - faster but less accurate.
	*/
	static inline real Cos (real fValue, bool useTables = false) 
	{	return (!useTables) ? real(cos(fValue)) : SinTable(fValue + HALF_PI);	}

	static inline real Exp (real fValue) 
	{ return real(exp(fValue)); }

	static inline real Floor (real fValue) 
	{ return real(floor(fValue)); }

	static inline real Log (real fValue) 
	{ return real(log(fValue)); }

	static inline real Pow (real fBase, real fExponent) 
	{ return real(pow(fBase,fExponent)); }

	static real Sign (real fValue);

	/** Sine function.
	@param
		fValue Angle in radians
	@param
		useTables If true, uses lookup tables rather than
	calculation - faster but less accurate.
	*/
	static inline real Sin (real fValue, bool useTables = false) 
	{	return (!useTables) ? real(sin(fValue)) : SinTable(fValue);	}

	static inline real Sqr (real fValue) { return fValue*fValue; }

	static inline real Sqrt (real fValue) { return real(sqrt(fValue)); }

	/** Inverse square root i.e. 1 / Sqrt(x), good for vector
		normalisation.
	*/
	static real InvSqrt(real fValue);

	static real UnitRandom ();  // in [0,1]

	static real RangeRandom (real fLow, real fHigh);  // in [fLow,fHigh]

	static real SymmetricRandom ();  // in [-1,1]

	/** Tangent function.
		@param
			fValue Angle in radians
		@param
			useTables If true, uses lookup tables rather than
			calculation - faster but less accurate.
	*/
	static inline real Tan (real fValue, bool useTables = false) 
	{	return (!useTables) ? real(tan(fValue)) : TanTable(fValue);	}

	static inline real DegreesToRadians(real degrees) { return degrees * fDeg2Rad; }
	static inline real RadiansToDegrees(real radians) { return radians * fRad2Deg; }

	/** Sets the native angle units (radians or degrees) expected by and returned by the Ogre API
	@remarks
		By default, YAKE's main API uses degrees (this Math class uses radians because that is the underlying
		unit used by the library routines. This may be changed by the user of the engine so that every instance
		of degrees actually accepts radians instead.
	@par
		You can set this directly after creating a new Root, and also before/after resource creation,
		depending on whether you want the change to affect resource files.
	@par
		Warning: don't set this during the middle of an app run - some classes store degrees internally
		as degrees, and perform the conversion for internal usage. Changing the AngleUnits between set
		and get will result in screwed up values. This affects some file loading too - notably particle
		system angle attributes. These values must also be changed in the particle files to use radians.
	*/
	static void setAngleUnit(AngleUnit unit);
	/** Get the unit being used for angles. */
	static AngleUnit getAngleUnit(void);

	/** Convert from the units the engine is currently using to radians. */
	static real AngleUnitsToRadians(real units);
	/** Convert from radians to the units the engine is currently using . */
	static real RadiansToAngleUnits(real radians);

	/** Checks wether a given point is inside a triangle, in a
		2-dimensional (Cartesian) space.
		@remarks
			The vertices of the triangle must be given in either
			trigonometrical (anticlockwise) or inverse trigonometrical
			(clockwise) order.
		@param
			px The X-coordinate of the point.
		@param
			py The Y-coordinate of the point.
		@param
			ax The X-coordinate of the triangle's first vertex.
		@param
			ay The Y-coordinate of the triangle's first vertex.
		@param
			bx The X-coordinate of the triangle's second vertex.
		@param
			by The Y-coordinate of the triangle's second vertex.
		@param
			cx The X-coordinate of the triangle's third vertex.
		@param
			cy The Y-coordinate of the triangle's third vertex.
		@returns
			If the point resides in the triangle, <b>true</b> is
			returned.
		@par
			If the point is outside the triangle, <b>false</b> is
			returned.
	*/
	static bool pointInTri2D( real px, real pz, real ax, real az, real bx, real bz, real cx, real cz );
	/** Compare 2 reals, using tolerance for inaccuracies.
	*/
	static bool RealEqual(real a, real b,real tolerance = std::numeric_limits<real>::epsilon());

	static const real POS_INFINITY;
	static const real NEG_INFINITY;
	static const real PI;
	static const real TWO_PI;
	static const real HALF_PI;
	static const real fDeg2Rad;
	static const real fRad2Deg;
	static const real EPSILON;
};


} // math
} // yake

#endif // YAKE_BASE_MATH_MATH_H
