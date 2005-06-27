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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>

#include <stdlib.h> // for RAND_MAX

#define FORCEINLINE inline
#include <yake/base/math/yakeAsmMath.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace math {

	const real Math::POS_INFINITY = std::numeric_limits<real>::infinity();
	const real Math::NEG_INFINITY = -std::numeric_limits<real>::infinity();
	const real Math::PI = real( 4.0 * atan( 1.0 ) );
	const real Math::TWO_PI = real( 2.0 * PI );
	const real Math::HALF_PI = real( 0.5 * PI );
	const real Math::fDeg2Rad = PI / real(180.0);
	const real Math::fRad2Deg = real(180.0) / PI;
	const real Math::EPSILON = (real) 0.0000001;

	int Math::mTrigTableSize;
	Math::AngleUnit Math::msAngleUnit;

	real  Math::mTrigTableFactor;
	real *Math::mSinTable = 0;
	real *Math::mTanTable = 0;

    //-----------------------------------------------------------------------
    Math::Math( unsigned int trigTableSize )
    {
        msAngleUnit = AU_DEGREE;

        mTrigTableSize = trigTableSize;
        mTrigTableFactor = mTrigTableSize / Math::TWO_PI;

        mSinTable = new real[mTrigTableSize];
        mTanTable = new real[mTrigTableSize];

        buildTrigTables();

        // Init random number generator
        srand( (unsigned)time(0) );
    }

    //-----------------------------------------------------------------------
    Math::~Math()
    {
        delete [] mSinTable;
        delete [] mTanTable;
    }

    //-----------------------------------------------------------------------
    void Math::buildTrigTables(void)
    {
        // Build trig lookup tables
        // Could get away with building only PI sized Sin table but simpler this 
        // way. Who cares, it'll ony use an extra 8k of memory anyway and I like 
        // simplicity.
        real angle;
        for (int i = 0; i < mTrigTableSize; ++i)
        {
            angle = Math::TWO_PI * i / mTrigTableSize;
            mSinTable[i] = sin(angle);
            mTanTable[i] = tan(angle);
        }
    }
	//-----------------------------------------------------------------------	
	real Math::SinTable (real fValue)
    {
        // Convert range to index values, wrap if required
        int idx;
        if (fValue >= 0)
        {
            idx = int(fValue * mTrigTableFactor) % mTrigTableSize;
        }
        else
        {
            idx = mTrigTableSize - (int(-fValue * mTrigTableFactor) % mTrigTableSize) - 1;
        }

        return mSinTable[idx];
    }
	//-----------------------------------------------------------------------
	real Math::TanTable (real fValue)
    {
        // Convert range to index values, wrap if required
		int idx = int(fValue *= mTrigTableFactor) % mTrigTableSize;
		return mTanTable[idx];
    }
    //-----------------------------------------------------------------------
    int Math::ISign (int iValue)
    {
        return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
    }
    //-----------------------------------------------------------------------
    real Math::ACos (real fValue)
    {
        if ( -1.0 < fValue )
        {
            if ( fValue < 1.0 )
                return real(acos(fValue));
            else
                return 0.0;
        }
        else
        {
            return PI;
        }
    }
    //-----------------------------------------------------------------------
    real Math::ASin (real fValue)
    {
        if ( -1.0 < fValue )
        {
            if ( fValue < 1.0 )
                return real(asin(fValue));
            else
                return -HALF_PI;
        }
        else
        {
            return HALF_PI;
        }
    }
    //-----------------------------------------------------------------------
    real Math::Sign (real fValue)
    {
        if ( fValue > 0.0 )
            return 1.0;

        if ( fValue < 0.0 )
            return -1.0;

        return 0.0;
    }
	//-----------------------------------------------------------------------
	real Math::InvSqrt(real fValue)
	{
		return real(asm_rsq(fValue));
	}
    //-----------------------------------------------------------------------
    real Math::UnitRandom ()
    {
        return asm_rand() / asm_rand_max();
    }
    
    //-----------------------------------------------------------------------
    real Math::RangeRandom (real fLow, real fHigh)
    {
        return (fHigh-fLow)*UnitRandom() + fLow;
    }

    //-----------------------------------------------------------------------
    real Math::SymmetricRandom ()
    {
		return 2.0f * UnitRandom() - 1.0f;
    }

   //-----------------------------------------------------------------------
    void Math::setAngleUnit(Math::AngleUnit unit)
   {
       msAngleUnit = unit;
   }
   //-----------------------------------------------------------------------
   Math::AngleUnit Math::getAngleUnit(void)
   {
       return msAngleUnit;
   }
    //-----------------------------------------------------------------------
    real Math::AngleUnitsToRadians(real angleunits)
    {
       if (msAngleUnit == AU_DEGREE)
           return angleunits * fDeg2Rad;
       else
           return angleunits;
    }

    //-----------------------------------------------------------------------
    real Math::RadiansToAngleUnits(real radians)
    {
       if (msAngleUnit == AU_DEGREE)
           return radians * fRad2Deg;
       else
           return radians;
    }

    //-----------------------------------------------------------------------
    bool Math::pointInTri2D( real px, real py, real ax, real ay, real bx, real by, real cx, real cy )
    {
        real v1x, v2x, v1y, v2y;
        bool bClockwise;

        v1x = bx - ax;
        v1y = by - ay;

        v2x = px - bx;
        v2y = py - by;

        // For the sake of readability
        #define Clockwise ( v1x * v2y - v1y * v2x >= 0.0 )

        bClockwise = Clockwise;

        v1x = cx - bx;
        v1y = cy - by;

        v2x = px - cx;
        v2y = py - cy;

        if( Clockwise != bClockwise )
            return false;

        v1x = ax - cx;
        v1y = ay - cy;

        v2x = px - ax;
        v2y = py - ay;

        if( Clockwise != bClockwise )
            return false;

        return true;

        // Clean up the #defines
        #undef Clockwise
    }

    //-----------------------------------------------------------------------
    bool Math::RealEqual( real a, real b, real tolerance )
    {
        if ((b < (a + tolerance)) && (b > (a - tolerance)))
            return true;
        else
            return false;
    }

} // math
} // yake
