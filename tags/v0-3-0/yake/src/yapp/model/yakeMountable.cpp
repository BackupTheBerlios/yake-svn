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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeEngine.h>
#include <yapp/model/yakeMountable.h>

namespace yake {
namespace model {

	//-----------------------------------------------------
	// Class: MountPoint
	//-----------------------------------------------------
	MountPoint::~MountPoint()
	{
		ConstVectorIterator< MountablePtrList > it( mMountables );
		while (it.hasMoreElements())
			detach( it.getNext() );
		mMountables.clear();
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
	Vector3 MountPoint::getDirection() const
	{ 
		return mDir; 
	}
	void MountPoint::setDirection( const Vector3& dir)
	{
		mDir = dir.normalisedCopy();
	}

	//-----------------------------------------------------
	// Class: Mountable
	//-----------------------------------------------------
	Mountable::Mountable() : mPt(0)
	{}
	Mountable::~Mountable()
	{
		if (mPt)
			mPt->detach( this );
	}
	void Mountable::onAttached( MountPoint* mountpoint )
	{
		YAKE_ASSERT( mPt == 0 ).debug("Mountable already bound to a mount point! Rebinding, anyway.");
		mPt = mountpoint;
	}
	void Mountable::onDetached()
	{
		mPt = 0;
	}
	MountPoint* Mountable::getMountPoint() const
	{
		return mPt;
	}

	//-----------------------------------------------------
	// Class: Thruster
	//-----------------------------------------------------
	Thruster::Thruster() :
		mMinForce(real(0)),
		mMaxForce(real(1)),
		mForce(real(0))
	{
	}
	void Thruster::setMinimumForce( real force )
	{
		mMinForce = force;
	}
	real Thruster::getMinimumForce() const
	{
		return mMinForce;
	}
	void Thruster::setMaximumForce( real force )
	{
		mMaxForce = force;
	}
	real Thruster::getMaximumForce() const
	{
		return mMaxForce;
	}
	void Thruster::setForce( real force )
	{
		mForce = force;
		if (mForce < mMinForce)
			mForce = mMinForce;
		else if (mForce > mMaxForce)
			mForce = mMaxForce;
	}
	real Thruster::getForce() const
	{
		return mForce;
	}
}
}
