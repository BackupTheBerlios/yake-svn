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
#include <yake/physics/yakePCH.h>
#include <yake/physics/yakePhysics.h>

namespace yake {
namespace physics {

	YAKE_IMPLEMENT_REGISTRY( IPhysicsSystem );

	BodyGroup::BodyGroup()
	{
	}
	BodyGroup::BodyGroup( const BodyGroup & rkBodyGroup ) :
		mBodies( rkBodyGroup.mBodies )
	{
	}
	BodyGroup::BodyGroup( const BodyVector & rkBodies ) :
		mBodies( rkBodies )
	{
	}
	BodyGroup::~BodyGroup()
	{
	}
	BodyGroup& BodyGroup::operator += (SharedPtr<IBody> & rBody)
	{
		mBodies.push_back( rBody );
		return *this;
	}
	BodyGroup& BodyGroup::operator -= (SharedPtr<IBody> & rBody)
	{
		BodyVector::iterator itFind = std::find(mBodies.begin(), mBodies.end(), rBody );
		if (itFind != mBodies.end())
			mBodies.erase( itFind );
		return *this;
	}
	BodyGroup::iterator BodyGroup::begin()
	{
		return mBodies.begin();
	}
	BodyGroup::const_iterator BodyGroup::begin() const
	{
		return mBodies.begin();
	}
	BodyGroup::iterator BodyGroup::end()
	{
		return mBodies.end();
	}
	BodyGroup::const_iterator BodyGroup::end() const
	{
		return mBodies.end();
	}
}
}
