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
#ifndef YAKE_BASE__MATH_INTERPOLATOR_H
#define YAKE_BASE__MATH_INTERPOLATOR_H

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace math
{

		template < typename T >
		real clamp( T val, T valMin, T valMax )
		{
			if (val < valMin)
				return valMin;
			else if (val > valMax)
				return valMax;
			return val;
		}

		template < typename T >
		class Interpolator
		{
		protected:
			Interpolator() {}

			T & mVal;
		public:
			Interpolator( T & rval ) : mVal( rval )
			{
			}
			virtual ~Interpolator()
			{
			}

			virtual void update( real time ) = 0;
		};

		/** These implementations are partly based on an article I found somewhere on the web,
			but I just can't remember which one it was.
			Then again, the formulas are generally known ;)
		*/
		template < typename T >
		class LinearInterpolator : public Interpolator< T >
		{
		protected:
			LinearInterpolator();

			real	mStart,mEnd;
			real	mElapsed;
			real	mTotal;
		public:
			LinearInterpolator( T & rval, real start, real end, real totalTime ) : Interpolator< T >( rval ), mStart(start),mEnd(end),mElapsed(0.),mTotal(totalTime)
			{
			}

			virtual void update( real time )
			{
				mElapsed += time;
				real pos = clamp< real >( mElapsed / mTotal, 0., 1. );
				mVal = mStart * ( 1 - pos ) + mEnd * pos;
			}
		};

		template < typename T >
		class QuadraticInterpolator : public Interpolator< T >
		{
		protected:
			QuadraticInterpolator();

			real	mStart,mMid,mEnd;
			real	mElapsed;
			real	mTotal;
		public:
			QuadraticInterpolator( T & rval, real start, real mid, real end, real totalTime ) : Interpolator< T >( rval ), mStart(start),mMid(mid),mEnd(end),mElapsed(0.),mTotal(totalTime)
			{
			}

			virtual void update( real time )
			{
				mElapsed += time;
				real b = clamp< real >( mElapsed / mTotal, 0., 1. );
				real a = 1 - b;
				mVal = mStart * a * a + midVal * 2 * a * b + mEnd * b * b;
			}
		};

		template < typename T >
		class CubicInterpolator : public Interpolator< T >
		{
		protected:
			CubicInterpolator();

			real	mStart,mMid1,mMid2,mEnd;
			real	mElapsed;
			real	mTotal;
		public:
			CubicInterpolator( T & rval, real start, real mid1, real mid2, real end, real totalTime ) : Interpolator< T >( rval ), mStart(start),mMid(mid),mEnd(end),mElapsed(0.),mTotal(totalTime)
			{
			}

			virtual void update( real time )
			{
				mElapsed += time;
				real b = clamp< real >( mElapsed / mTotal, 0., 1. );
				real a = 1 - b;
				mVal = mStart * a * a * a + midVal1 * 3 * a * a * b + midVal2 * 3 * a * b * b + mEnd * b * b * b;
			}
		};


} // math
} // base
} // yake

#endif // YAKE_BASE__MATH_INTERPOLATOR_H
