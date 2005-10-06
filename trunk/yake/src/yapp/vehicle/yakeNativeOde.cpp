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

	YAKE_IMPLEMENT_REGISTRY(IVehicleSystem);
	YAKE_REGISTER_CONCRETE(OdeVehicleSystem);

	//-----------------------------------------------------
	// Class: OdeVehicleSystem
	//-----------------------------------------------------
	OdeVehicleSystem::OdeVehicleSystem()
	{}
	IVehicle* OdeVehicleSystem::create(const VehicleTemplate& tpl, physics::IWorld& PWorld)
	{
		OdeVehicle* pV = new OdeVehicle();
		pV->_create( tpl, PWorld );
		return pV;
	}

	//-----------------------------------------------------
	// Class: OdeVehicle
	//-----------------------------------------------------
	OdeVehicle::OdeVehicle() : mpChassis(0)
	{
	}
	OdeVehicle::~OdeVehicle()
	{
		for (SteeringGroupList::const_iterator it = mSteeringGroups.begin(); it != mSteeringGroups.end(); ++it)
		{
			ConstDequeIterator< WheelList > itWheel( it->second );
			while (itWheel.hasMoreElements())
				delete itWheel.getNext();
		}
		mSteeringGroups.clear();

		ConstDequeIterator< EnginePtrList > itEngine( mEngines );
		while (itEngine.hasMoreElements())
			delete itEngine.getNext();
		mEngines.clear();

		ConstDequeIterator< MountPointList > itMP( mMountPoints );
		while (itMP.hasMoreElements())
			delete itMP.getNext();
		mMountPoints.clear();

		mpChassis->getCreator()->destroyActor( mpChassis );
		mpChassis = 0;
	}
	void OdeVehicle::updateSimulation(real timeElapsed)
	{
		ConstDequeIterator< EnginePtrList > itEngine( mEngines );
		while (itEngine.hasMoreElements())
		{
			IEngine* pEngine = itEngine.getNext();
			pEngine->updateSimulation(timeElapsed);
		}
		//
		mSigUpdateThrusterSimulation( timeElapsed );
		mSigApplyThrusterToTargets();
	}
	MountPoint* OdeVehicle::getMountPoint(size_t index) const
	{
		YAKE_ASSERT( index < mMountPoints.size() );
		if (index >= mMountPoints.size())
			return 0;
		return mMountPoints.at(index);
	}
	IEngine* OdeVehicle::getEngineInterface(size_t index) const
	{
		YAKE_ASSERT( index < mEngines.size() );
		if (index >= mEngines.size())
			return 0;
		return mEngines.at(index);
	}
	IWheel* OdeVehicle::getWheelInterface(size_t index) const
	{
		YAKE_ASSERT( index < mWheels.size() );
		if (index >= mWheels.size())
			return 0;
		return mWheels.at(index);
	}
	Vector3 OdeVehicle::getChassisPosition() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis->getPosition();
	}
	Quaternion OdeVehicle::getChassisOrientation() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis->getOrientation();
	}
	Movable* OdeVehicle::getChassisMovable() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis;
	}
	void OdeVehicle::_create(const VehicleTemplate& tpl, physics::IWorld& PWorld )
	{
		// chassis
		mpChassis = PWorld.createActor( physics::ACTOR_DYNAMIC );
		mpChassis->setPosition( tpl.mChassis.mPosition );
		ConstDequeIterator< VehicleTemplate::ShapeTplList > itShapeTpl( tpl.mChassis.mChassisShapes );
		while (itShapeTpl.hasMoreElements())
		{
			mpChassis->createShape( *itShapeTpl.getNext() );
		}
		mpChassis->getBody().setMass( tpl.mChassis.mMass );

		// mount points
		MountPointList allMPs;
		ConstDequeIterator< VehicleTemplate::MountPointTplList > itMP( tpl.mMountPoints );
		while (itMP.hasMoreElements())
		{
			const VehicleTemplate::MountPointTpl mptTpl = itMP.getNext();
			MountPoint* pMP = new OdeMountPoint();
			pMP->setPosition( mptTpl.mPosition );
			if (mptTpl.mUseDirection)
				pMP->setDirection( mptTpl.mDirection );
			else
				pMP->setOrientation( mptTpl.mOrientation );

			allMPs.push_back( pMP );

			if (mptTpl.mParentMountPointIdx == MPID_NO_PARENT)
			{
				mMountPoints.push_back( pMP );
				pMP->setOverrideParentMovable( mpChassis );
			}
			//mptTpl.
		}
		size_t idx = 0;
		itMP.reset( tpl.mMountPoints );
		while (itMP.hasMoreElements())
		{
			const VehicleTemplate::MountPointTpl mptTpl = itMP.getNext();
			if (mptTpl.mParentMountPointIdx != MPID_NO_PARENT)
			{
				allMPs.at(mptTpl.mParentMountPointIdx)->addChild( allMPs.at(idx), true );
			}
			++idx;
		}
		allMPs.clear();

		// engines
		ConstDequeIterator< VehicleTemplate::EngineTplList > itEngineTpl( tpl.mEngines );
		while (itEngineTpl.hasMoreElements())
		{
			//@todo refactor to avoid casting... ?

			const VehicleTemplate::EngineTpl* engineTpl = itEngineTpl.getNext();

			//@todo replace dynamic_casts with static_casts in release mode ?

			const VehicleTemplate::CarEngineTpl* carEngineTpl =
					dynamic_cast<const VehicleTemplate::CarEngineTpl*>( engineTpl );
			if (carEngineTpl)
			{
				OdeCarEngine* pEngine = new OdeCarEngine();
				mEngines.push_back(pEngine);
				pEngine->setParamMaxRPM( carEngineTpl->rpmMax_ );
				pEngine->setParamMinRPM( carEngineTpl->rpmMin_ );
				pEngine->setParamRedlineRPM( carEngineTpl->rpmRedline_ );
			}
			else
			{
				const VehicleTemplate::ThrusterTpl* thrusterTpl =
						dynamic_cast<const VehicleTemplate::ThrusterTpl*>( engineTpl );
				if (thrusterTpl)
				{
					// thruster itself
					OdeThruster* pEngine = new OdeThruster();
					mEngines.push_back(pEngine);
					pEngine->setMinimumForce( thrusterTpl->minForce );
					pEngine->setMaximumForce( thrusterTpl->maxForce );
					pEngine->setThrottle(0.);
					subscribeToUpdateThrusterSimulation(
						boost::bind( &OdeThruster::updateSimulation, pEngine, _1 ) );

					// the mountable thruster wrapper
					if (thrusterTpl->mountPtIdx != MPID_NO_PARENT)
					{
						OdeMountedThruster* pMounted = new OdeMountedThruster();
						pMounted->setThruster( pEngine );

						pMounted->addTarget( mpChassis->getBodyPtr() );

						subscribeToApplyThrusterToTargets(
							boost::bind( &OdeMountedThruster::applyToTargets, pMounted ) );

						mMountPoints[thrusterTpl->mountPtIdx]->attach( pMounted );
					}
				}
			}
		}
	
		// steering groups
		uint32 numStGrps = tpl.mSteeringGroups > 0 ? tpl.mSteeringGroups : 1;
		for (uint32 i=0; i<numStGrps; ++i)
			mSteeringGroups.insert( std::make_pair(i,WheelList()) );

		// wheels
		ConstDequeIterator< VehicleTemplate::WheelTplList > itWheelTpl( tpl.mWheels );
		while (itWheelTpl.hasMoreElements())
		{
			const VehicleTemplate::WheelTpl wheelTpl = itWheelTpl.getNext();
			YAKE_ASSERT( wheelTpl.mSteeringGroup < mSteeringGroups.size() );
			if (wheelTpl.mSteeringGroup >= mSteeringGroups.size())
			{
				mWheels.push_back( 0 ); // => need this so that the indices stay valid!
				continue;
			}

			OdeWheel* pW = new OdeWheel(mpChassis,wheelTpl,PWorld);
			mSteeringGroups[ wheelTpl.mSteeringGroup ].push_back( pW );
			mWheels.push_back( pW );
		}
	}

	//-----------------------------------------------------
	// Class: OdeWheel
	//-----------------------------------------------------
	OdeWheel::OdeWheel(	physics::IActorPtr chassisObj,
						const VehicleTemplate::WheelTpl& tpl,
						physics::IWorld& PWorld ) :
		mpJoint(0),
		mpWheel(0)
	{
		YAKE_ASSERT( chassisObj );
		mpWheel = PWorld.createActor( physics::ACTOR_DYNAMIC );
		mpWheel->createShape( physics::IShape::SphereDesc( tpl.mRadius ) );
		real mass = tpl.mMassRelativeToChassis ?
							(tpl.mMass * chassisObj->getBody().getMass()) : tpl.mMass;
		mpWheel->getBody().setMass( mass );

		mpJoint = PWorld.createJoint( physics::IJoint::DescFixed( chassisObj, mpWheel ) );
	}
	OdeWheel::~OdeWheel()
	{
		YAKE_ASSERT( mpWheel );
		YAKE_ASSERT( mpJoint );
		mpWheel->getCreator()->destroyJoint( mpJoint );
		mpJoint = 0;
		mpWheel->getCreator()->destroyActor( mpWheel );
		mpWheel = 0;
	}
	Vector3 OdeWheel::getPosition() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getPosition();
	}
	Quaternion OdeWheel::getOrientation() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getOrientation();
	}

	//-----------------------------------------------------
	// Class: OdeThruster
	//-----------------------------------------------------
	OdeThruster::OdeThruster() : mThrottle(0)
	{
	}
	void OdeThruster::setThrottle( real throttle )
	{
		mThrottle = (throttle < 0.) ? 0. : ((throttle > 1.) ? 1. : throttle);
	}
	real OdeThruster::getThrottle() const
	{
		return mThrottle;
	}
	void OdeThruster::updateSimulation( real timeElapsed )
	{
		setForce( getMinimumForce() + mThrottle * ( getMaximumForce() - getMinimumForce() ) );
	}

	//-----------------------------------------------------
	// Class: OdeMountedThruster
	//-----------------------------------------------------
	OdeMountedThruster::OdeMountedThruster()
	{
	}
	void OdeMountedThruster::onApplyToTargets()
	{
		YAKE_ASSERT(mThruster);
		if (!mThruster)
			return;
		const MountPoint* mp = getMountPoint();
		const Vector3 pos = mp ? mp->getDerivedPosition() : Vector3::kZero;
		const Quaternion rot = mp ? mp->getDerivedOrientation() : Quaternion::kIdentity;
		const Vector3 f = mThruster->getForce() * ( rot * -Vector3::kUnitZ );
		
		ConstDequeIterator< BodyPtrList > itBody( mTargets );
		while (itBody.hasMoreElements())
		{
			physics::IBody* pBody = itBody.getNext();
			pBody->addForceAtPos( f, pos );
		}
	}
} // namespace vehicle
} // namespace yake
