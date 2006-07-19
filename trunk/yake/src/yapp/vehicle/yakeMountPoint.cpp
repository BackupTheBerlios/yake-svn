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
#include <yapp/vehicle/yakePCH.h>
#include <yapp/vehicle/yakeVehicle.h>
#include <yapp/vehicle/yakeNativeOde.h>

namespace yake {
namespace vehicle {

	//-----------------------------------------------------
	// Class: MountPoint
	//-----------------------------------------------------
	MountPoint::MountPoint() :
		mPosition(math::Vector3::kZero),
		mOrientation(math::Quaternion::kIdentity),
		mParent(0),
		mOverrideParent(0)
	{
	}
	MountPoint::~MountPoint()
	{
		MountablePtrList mountables = mMountables;
		ConstVectorIterator< MountablePtrList > it( mountables );
		while (it.hasMoreElements())
			detach( it.getNext() );
		YAKE_ASSERT( mMountables.empty() );

		ConstVectorIterator< MountPointList > itMP( mChildren );
		while (itMP.hasMoreElements())
		{
			std::pair<MountPoint*,bool> mp = itMP.getNext();
			if (mp.second)
				delete mp.first;
		}
		mChildren.clear();
	}
	void MountPoint::setParent(MountPoint* parent)
	{
		mParent = parent;
	}
	void MountPoint::setOverrideParentMovable(Movable* mov)
	{
		mOverrideParent = mov;
	}
	bool MountPoint::isSuitableFor( const Mountable* mountable ) const
	{
		return true; // default
	}
	void MountPoint::attach( Mountable* mountable )
	{
		if (!mountable)
			return;
		if (std::find( mMountables.begin(), mMountables.end(), mountable ) == mMountables.end())
		{
			mMountables.push_back( mountable );
			mountable->onAttached( this );
		}
	}
	void MountPoint::detach( Mountable* mountable )
	{
		YAKE_ASSERT( mountable ).warning("Please tell me how detach a null mountable :/ Skip it!");
		if (!mountable)
			return;
		MountablePtrList::iterator itFind = std::find( mMountables.begin(), mMountables.end(), mountable );
		if (itFind == mMountables.end())
			return;
		mMountables.erase( itFind );
		mountable->onDetached();
	}
	math::Vector3 MountPoint::getDirection() const
	{ 
		// direction in local coordinates
		//
		return mOrientation * math::Vector3::kUnitZ; 
	}
	void MountPoint::setDirection( const math::Vector3& dir)
	{
		const real tolerance = real(0.001);
		const math::Vector3 currDir = getDirection();
		const math::Vector3 newDir = dir.normalisedCopy();

		if ( newDir == currDir )
			return;

		if ( newDir.dotProduct( currDir ) + 1 <= tolerance ) // tolerance check. TODO tolerance globally? for math
		{
		    	real angle = 0.;
		    	math::Vector3 axis;
		    	mOrientation.ToAngleAxis( angle, axis );

			// adding another 180 grads
			angle += math::Math::PI;

			// setting new orientation
			mOrientation.FromAngleAxis( angle, axis );			
			
			return;
		}
		mOrientation = currDir.getRotationTo( newDir ) * mOrientation;
	}
	void MountPoint::setPosition(const math::Vector3& rPosition)
	{
		mPosition = rPosition;
	}
	math::Vector3 MountPoint::getPosition() const
	{
		return mPosition;
	}
	void MountPoint::setOrientation( const math::Quaternion& q )
	{
		mOrientation = q;
	}
	math::Quaternion MountPoint::getOrientation() const
	{
		return mOrientation;
	}
	math::Vector3 MountPoint::getDerivedPosition() const
	{
		if (mOverrideParent)
			return mOverrideParent->getOrientation() * mPosition + mOverrideParent->getPosition();
		if (mParent)
			return mParent->getDerivedOrientation() * mPosition + mParent->getDerivedPosition();
		return mPosition;
	}
	math::Quaternion MountPoint::getDerivedOrientation() const
	{
		if (mOverrideParent)
			return mOverrideParent->getOrientation() * mOrientation;
		if (mParent)
			return mParent->getDerivedOrientation() * mOrientation;
		return mOrientation;
	}
	void MountPoint::addChild(MountPoint* pt,bool transferOwnership /* = true */)
	{
		YAKE_ASSERT( pt );
		if (!pt)
			return;
		mChildren.push_back( std::make_pair(pt,transferOwnership) );
		pt->setParent(this);
	}

} // namespace vehicle
} // namespace yake

