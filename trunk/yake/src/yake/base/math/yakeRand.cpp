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
#include <yake/base/yakePCH.h>
#include <yake/base/math/yakeRand.h>
#include <yake/base/math/yakeMersenneTwister.h>

namespace yake {
namespace math {

	//---------------------------------------------------
	RandomNumberGeneratorMT::RandomNumberGeneratorMT()
	{
		mRand = new MTRand();
		setSeed( 54738 );
	}

	//---------------------------------------------------
	RandomNumberGeneratorMT::~RandomNumberGeneratorMT()
	{
		YAKE_SAFE_DELETE( mRand );
	}

	//---------------------------------------------------
	real RandomNumberGeneratorMT::operator()()
	{
		return (real) mRand->rand();
	}

	//---------------------------------------------------
	uint32 RandomNumberGeneratorMT::randInt()
	{
		return mRand->randInt();
	}

	//---------------------------------------------------
	real RandomNumberGeneratorMT::randReal()
	{
		return (real) mRand->rand();
	}

	//---------------------------------------------------
	void RandomNumberGeneratorMT::setSeed( uint32 seed )
	{
		mSeed = seed;
		mRand->seed( mSeed );
	}

	//---------------------------------------------------
	uint32 RandomNumberGeneratorMT::getSeed() const
	{
		return mSeed;
	}
}
}
