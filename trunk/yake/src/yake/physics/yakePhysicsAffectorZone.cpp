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
#include <yake/physics/yakePCH.h>
#include <yake/physics/yakePhysics.h>
#include <yake/physics/yakeAffectorZone.h>

namespace yake {
namespace physics {

	AffectorZone::AffectorZone() : mEnabled(true), mNeedsUpdate(true)
	{
	}
	void AffectorZone::update( const real timeElapsed )
	{
		YAKE_ASSERT( mEnabled );
		if (!mNeedsUpdate)
			return;

		_determineAffectedBodies();
		
		VectorIterator<AffectorList> itAff( mAffectors.begin(), mAffectors.end() );
		while (itAff.hasMoreElements())
		{
			itAff.getNext()->applyTo( mAffectedBodies, timeElapsed );
		}
	}

	void AffectorZone::setEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	bool AffectorZone::isEnabled() const
	{
		return mEnabled;
	}

	void AffectorZone::addAffector(SharedPtr<IBodyAffector> &pAffector)
	{
		YAKE_ASSERT(pAffector);
		mAffectors.push_back(pAffector);
	}

	void AffectorZoneManager::addAffectorZone(SharedPtr<AffectorZone> &pSpace)
	{
		mZones.push_back(pSpace);
	}

	AffectorZoneManager::AffectorZoneManager(SharedPtr<physics::IWorld>& pWorld) : mWorld(pWorld)
	{
		mStepSigConn = mWorld->subscribeToPreStepInternal( boost::bind(&AffectorZoneManager::update, this, _1) );
	}

	AffectorZoneManager::~AffectorZoneManager()
	{
		mStepSigConn.disconnect();
		mZones.clear();
	}

	void AffectorZoneManager::update(const real timeElapsed)
	{
		VectorIterator<ZoneList> itZone(mZones.begin(), mZones.end());
		SharedPtr<AffectorZone> pZone;
		while (itZone.hasMoreElements())
		{
			pZone = itZone.getNext();
			if (pZone->isEnabled())
				pZone->update(timeElapsed);
		}
	}

}
}
