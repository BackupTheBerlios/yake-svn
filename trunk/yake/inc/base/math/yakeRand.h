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
#ifndef YAKE_BASE_MATH_RAND_H
#define YAKE_BASE_MATH_RAND_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "../yakePrerequisites.h"
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace math
{

		class MTRand;
		/** Random number generator interface. 
			\see RandomNumberGeneratorMT
		*/
		template < typename IntType, typename RealType >
		class YAKE_BASE_API RandomNumberGenerator
		{
		public:
			virtual ~RandomNumberGenerator() {}

			virtual void setSeed( IntType seed ) = 0;
			virtual IntType getSeed() const = 0;

			virtual RealType operator()() = 0;
			virtual IntType randInt() = 0;
			virtual RealType randReal() = 0;
		};

		/** A random number generator.
			In fact, it's a thin wrapper for a MersenneTwister implementation (
			see yakeRand.cpp and yakeMersenneTwister.h for more information ).
			\see RandomNumberGenerator
		*/
		class YAKE_BASE_API RandomNumberGeneratorMT : public RandomNumberGenerator< uint32, real >
		{
		public:
			RandomNumberGeneratorMT();
			virtual ~RandomNumberGeneratorMT();

			virtual void setSeed( uint32 seed );
			virtual uint32 getSeed() const;

			virtual real operator()();
			virtual uint32 randInt();
			virtual real randReal();

		protected:
			MTRand		* mRand;
			uint32		mSeed;
		};

} // math
} // base
} // yake

#endif // YAKE_BASE_MATH_RAND_H