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
#include <inc/physics/yakePCH.h>
#include <inc/physics/yakePhysicsSystem.h>
#include <inc/physics/yakePhysical.h>

namespace yake {
namespace physics {

	Physical::Physical() : mBaseMovable(0)
	{
	}

	Physical::~Physical()
	{
		for (PhysicalList::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
			delete (*it);
		for (ComplexList::const_iterator it = mComplex.begin(); it != mComplex.end(); ++it)
			delete (*it);
	}

	void Physical::setPosition( const Vector3 & position )
	{
		if (mBaseMovable)
		{
			mBaseMovable->setPosition( position );
			for (PhysicalList::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->setPosition( position );
		}
	}

	void Physical::setOrientation( const Quaternion & orientation )
	{
		if (mBaseMovable)
		{
			mBaseMovable->setOrientation( orientation );
			for (PhysicalList::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->setOrientation( orientation );
		}
	}

	Vector3 Physical::getPosition() const
	{
		if (mBaseMovable)
			return mBaseMovable->getPosition();
		return Vector3::kZero;
	}

	Quaternion Physical::getOrientation() const
	{
		if (mBaseMovable)
			return mBaseMovable->getOrientation();
		return Quaternion::kIdentity;
	}

	void Physical::setBaseMovable( Movable & pBaseObj )
	{
		mBaseMovable = &pBaseObj;
	}

	void Physical::removeBaseMovable()
	{
		mBaseMovable = 0;
	}

	void Physical::addChild( Physical* pPhysical )
	{
		mChildren.push_back( pPhysical );
	}

	void Physical::addComplex(physics::IComplexObject* pComplex )
	{
		mComplex.push_back( pComplex );
	}

	void Physical::associate(const base::String & name, physics::IComplexObject* pComplex)
	{
		mComplexNames[ name ] = pComplex;
	}

	physics::IComplexObject* Physical::getComplex( const base::String & name )
	{
		return mComplexNames[name];
	}


}
}