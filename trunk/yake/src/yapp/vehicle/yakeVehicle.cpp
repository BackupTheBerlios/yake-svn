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
	// Class: VehicleTemplate
	//-----------------------------------------------------
	//VehicleTemplate* VehicleTemplate::clone() const
	//{
	//}

	//-----------------------------------------------------
	// Class: Mountable
	//-----------------------------------------------------
	Mountable::Mountable() : mPt(0)
	{
	}
	Mountable::~Mountable()
	{
		YAKE_ASSERT( !mPt ).debug("Still attached... but d'tor called!");
		mPt = 0;
	}
	MountPoint* Mountable::getMountPoint() const
	{
		return mPt;
	}
	void Mountable::onAttached( MountPoint* mountpoint )
	{
		YAKE_ASSERT( !mPt ).debug("Detach before reattaching!");
		mPt = mountpoint;
	}
	void Mountable::onDetached()
	{
		mPt = 0;
	}

	//-----------------------------------------------------
	// Class: ILinearThruster
	//-----------------------------------------------------
	ILinearThruster::ILinearThruster() :
		mMinForce(0.),
		mMaxForce(1.)
	{
	}
	ILinearThruster::~ILinearThruster()
	{
	}
	void ILinearThruster::setForce( real force )
	{
		mForce = force;
	}
	void ILinearThruster::setMinimumForce( real force )
	{
		mMinForce = force;
	}
	real ILinearThruster::getMinimumForce() const
	{
		return mMinForce;
	}
	void ILinearThruster::setMaximumForce( real force )
	{
		mMaxForce = force;
	}
	real ILinearThruster::getMaximumForce() const
	{
		return mMaxForce;
	}
	real ILinearThruster::getForce() const
	{
		return mForce;
	}

	//-----------------------------------------------------
	// Class: IMountedThruster
	//-----------------------------------------------------
	IMountedThruster::IMountedThruster() : mThruster(0)
	{
	}
	IMountedThruster::~IMountedThruster()
	{
		mTargets.clear();
	}
	void IMountedThruster::setThruster(IThruster*t)
	{
		mThruster = t;
	}
	void IMountedThruster::addTarget( physics::IBody* body )
	{
		YAKE_ASSERT( body );
		if (body)
			mTargets.push_back( body );
	}
	void IMountedThruster::removeTarget( physics::IBody* body )
	{
		YAKE_ASSERT( body );
		if (body)
			mTargets.erase( find( mTargets, body ) );
	}
	void IMountedThruster::applyToTargets()
	{
		onApplyToTargets();
	}

} // namespace vehicle
} // namespace yake

