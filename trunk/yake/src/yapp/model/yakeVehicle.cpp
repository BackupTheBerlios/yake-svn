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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>

namespace yapp {
namespace model {
namespace complex {

namespace vehicle {

	Vector3 Wheel::getLateralSlipNormal() const
	{
		YAKE_ASSERT( mJoint );
		return mJoint->getAxis2();
	}
	void Wheel::setJoint( SharedPtr<physics::IJoint> & jt )
	{
		mJoint = jt;
	}
	void Wheel::setComplex( physics::IComplexObject* pCO )
	{
		YAKE_ASSERT( pCO );
		mCO = pCO;
		mCO->setLateralSlipEnabled( true );
		mCO->setSlipCoefficients( 0.1, 0. );
		mCO->setSlipNormalSource( this );
	}
	void Wheel::setSuspension( const real spring, const real damping )
	{
		YAKE_ASSERT( mJoint );
		mJoint->setSpring( spring );
		mJoint->setDamping( damping );
	}
	real Wheel::getSuspensionSpring() const
	{
		YAKE_ASSERT( mJoint );
		return mJoint->getSpring();
	}
	real Wheel::getSuspensionDamping() const
	{
		YAKE_ASSERT( mJoint );
		return mJoint->getDamping();
	}
	void Wheel::applySteer( real steer )
	{
		real epsilon = 0.015;
		//if (steer < 0) epsilon *= -1;
		if (steer > -0.04 && steer < 0.04)
		{
			steer = 0;
			epsilon = 0;
		}
		mJoint->setLowStop( steer - epsilon );
		mJoint->setHighStop( steer + epsilon );
	}
	void Wheel::applyTq( const Vector3 & torque )
	{
		mCO->getBody()->addTorque( mCO->getBody()->getOrientation() * torque );
	}
	void Wheel::applyBrakeTq( const Vector3 & torque )
	{
		Vector3 linVel = mCO->getBody()->getLinearVelocity();
		Vector3 dir = mCO->getBody()->getOrientation() * Vector3::kUnitZ;
		if (dir.dotProduct(linVel) > 0)
			mCO->getBody()->addRelTorque( torque );
		else
			mCO->getBody()->addRelTorque( -torque );
	}
	void Wheel::apply( real velocity, real fmax )
	{
		if (mJoint->getType() == physics::IJoint::JT_HINGE)
		{
			mJoint->setMotor1Velocity( velocity );
			mJoint->setMotor1MaximumForce( fmax );
		}
		else
		{
			mJoint->setMotor2Velocity( velocity );
			mJoint->setMotor2MaximumForce( fmax );
		}
	}

	void Axle::attachAffectedWheel( Wheel* pWheel )
	{
		YAKE_ASSERT( pWheel );
		mWheels.push_back( pWheel );
	}
	void Axle::applyTorque(real torque)
	{
		VectorIterator< WheelList > it( mWheels.begin(), mWheels.end() );
		while (it.hasMoreElements())
		{
			Wheel* pWheel = it.getNext();
			YAKE_ASSERT( pWheel );
			//pWheel->applyTq( torque / 2 ); // FIXME: just two drive wheels

			real force = (torque/2.) / 0.34; // 0.34 meter = wheel diameter, 2 drive wheels
			if (force < 0)
				force *= -1;
			//FIXME:
			real magic2 = 0.2;
			pWheel->apply(	(torque < 0) ? 100 : -100,
							0.001 * force * magic2); // 0.001 mass factor (tonnes to kg)
		}
	}


	void GearBox::setSpeed(uint8 numGears)
	{
		mGears.resize( numGears );
	}
	uint8 GearBox::getSpeed() const
	{
		size_t size = mGears.size();
		YAKE_ASSERT( size <= 255 );
		return static_cast<uint8>(size);
	}
	void GearBox::setGearRatio(uint8 gear, real ratio)
	{
		YAKE_ASSERT( gear < getSpeed() );
		Gear& g = mGears[gear];
		g.ratio_ = ratio;
	}
	void GearBox::setGearMode( uint8 gear, const GearMode mode )
	{
		YAKE_ASSERT( gear < getSpeed() );
		Gear& g = mGears[gear];
		g.mode_ = mode;
	}
	real GearBox::getGearRatio( uint8 gear )
	{
		YAKE_ASSERT( gear < getSpeed() );
		Gear& g = mGears[gear];
		return g.ratio_;
	}



	void Engine::shiftGear( uint8 gear )
	{
		if (gear < mpGearBox->getSpeed())
			mCurrentGear = gear;
	}
	void Engine::shiftGearUp()
	{
		if (mCurrentGear+1 < mpGearBox->getSpeed())
			mCurrentGear++;
	}
	void Engine::shiftGearDown()
	{
		if (mCurrentGear > 0)
			mCurrentGear--;
	}
	real Engine::getRPM() const
	{
		return mCurrentRPM;
	}
	void Engine::setThrottle( real throttle )
	{
		mThrottle = throttle;
		if (mThrottle > 1)
			mThrottle = 1;
		else if (mThrottle < 0)
			mThrottle = 0;
	}
	real Engine::getThrottle() const
	{
		return mThrottle;
	}
	uint8 Engine::getGear() const
	{
		return mCurrentGear;
	}
	void Engine::init()
	{
		// Corvette C5 (http://home.planet.nl/~monstrous/c5specs.html)
		mDifferentialRatio = 3.42;
		YAKE_ASSERT( mpGearBox == 0);
		mpGearBox = new GearBox();
		YAKE_ASSERT( mpGearBox );
		mpGearBox->setSpeed( 6 );
		mpGearBox->setGearRatio( 0, -2.90 ); mpGearBox->setGearMode( 0, GearBox::GM_REVERSE );
		mpGearBox->setGearRatio( 1,  2.66 ); mpGearBox->setGearMode( 1, GearBox::GM_NORMAL );
		mpGearBox->setGearRatio( 2,  1.78 ); mpGearBox->setGearMode( 2, GearBox::GM_NORMAL );
		mpGearBox->setGearRatio( 3,  1.30 ); mpGearBox->setGearMode( 3, GearBox::GM_NORMAL );
		mpGearBox->setGearRatio( 4,  0.74 ); mpGearBox->setGearMode( 4, GearBox::GM_NORMAL );
		mpGearBox->setGearRatio( 5,  0.50 ); mpGearBox->setGearMode( 5, GearBox::GM_NORMAL );
	}
	Engine::Engine() :
		mDifferentialRatio( 3.42 ),
		mThrottle(0),
		mCurrentGear(0),
		mDriveTorque(0),
		mpGearBox(0),
		mMinRPM(0),
		mMaxRPM(3500),
		mThrottleRPMRise(2500),
		mRPMDieOff(3500)
	{
		init();
	}
	Engine::~Engine()
	{
		YAKE_SAFE_DELETE( mpGearBox );
	}
	void Engine::update( const real timeElapsed )
	{
		updateCurrentRPM( timeElapsed );
		updateGearRatios();

		//engine torque = throttle position * max torque
		//drive torque = engine_torque * gear_ratio * differential_ratio * transmission_efficiency
		//drive torque gets delivered to the wheels
		real engineTorque = mThrottle * getMaxTorque( mCurrentRPM );
		mDriveTorque = engineTorque * mGearRatio * mDifferentialRatio * 0.75/*just a guess*/;

		applyToAxles();
	}
	real Engine::getDriveTorque() const
	{
		return mDriveTorque;
	}
	real Engine::getMaxTorque( const real rpm )
	{
		//TODO: look up in curve
		return 500; // in Nm
	}
	void Engine::updateGearRatios()
	{
		mGearRatio = mpGearBox->getGearRatio( mCurrentGear );
		mInvGearRatio = (mGearRatio != 0.) ? (1.0 / mGearRatio) : 0.;
	}
	void Engine::updateCurrentRPM(real timeElapsed)
	{
		if (mThrottle > 0.)
		{
			mCurrentRPM += mThrottle * (mThrottleRPMRise * timeElapsed);
			if (mCurrentRPM > mMaxRPM)
				mCurrentRPM = mMaxRPM;
		}
		else
		{
			mCurrentRPM -= mRPMDieOff * timeElapsed;
			if (mCurrentRPM < mMinRPM)
				mCurrentRPM = mMinRPM;
		}
	}
	void Engine::attachAxle( Axle* pAxle )
	{
		YAKE_ASSERT( pAxle ).error("valid pointer expected!");
		mAxles.push_back( pAxle );
	}
	void Engine::applyToAxles()
	{
		VectorIterator< AxleList > it( mAxles.begin(), mAxles.end() );
		while (it.hasMoreElements())
		{
			Axle* pAxle = it.getNext();
			YAKE_ASSERT( pAxle );
			pAxle->applyTorque( mDriveTorque );
		}
	}

	//-----------------------------------------------------
	void LandVehicle::setSteer( real steer )
	{
		mSteerTarget = steer;
	}
	//-----------------------------------------------------
	real LandVehicle::getSteer() const
	{
		return mCurrentSteer;
	}
	//-----------------------------------------------------
	LandVehicle::~LandVehicle()
	{
		VectorIterator<WheelMap> it(mWheels.begin(), mWheels.end());
		while (it.hasMoreElements())
		{
			delete(it.getNext().second);
		}
		mWheels.clear();
	}
	//-----------------------------------------------------
	void LandVehicle::setEngine(AutoPtr<Engine> & engine)
	{
		mEngine = engine;
	}
	//-----------------------------------------------------
	Engine* LandVehicle::getEngine() const
	{
		return mEngine.get();
	}
	//-----------------------------------------------------
	void LandVehicle::addAxle(Axle* pAxle)
	{
		YAKE_ASSERT( pAxle );
		mAxles.push_back( pAxle );
	}
	//-----------------------------------------------------
	void LandVehicle::addWheel(const String & id, Wheel* pWheel)
	{
		YAKE_ASSERT( id != "" );
		if (id == "")
			return;
		YAKE_ASSERT( pWheel );
		mWheels.insert( std::make_pair(id,pWheel) );
	}
	//-----------------------------------------------------
	Wheel* LandVehicle::getWheel( const String & id )
	{
		YAKE_ASSERT( id != "" );
		WheelMap::const_iterator itFind = mWheels.find( id );
		if (itFind == mWheels.end())
			return 0;
		return itFind->second;
	}
	//-----------------------------------------------------
	void LandVehicle::setWheelAffectedByBrake(uint8 brake, Wheel* pWheel, real ratio)
	{
		YAKE_ASSERT( pWheel );
		BrakeEntry* pBrake = 0;
		BrakeList::const_iterator itFindBrake = mBrakes.find( brake );
		if (itFindBrake == mBrakes.end())
		{
			pBrake = new BrakeEntry;
			mBrakes.insert( std::make_pair( brake, SharedPtr<BrakeEntry>(pBrake) ) );
		}
		else
			pBrake = itFindBrake->second.get();
		pBrake->wheels.push_back( std::make_pair(pWheel,ratio) );
	}
	//-----------------------------------------------------
	void LandVehicle::brake( uint8 brake, real amount )
	{
		BrakeList::const_iterator itFindBrake = mBrakes.find( brake );
		YAKE_ASSERT(itFindBrake != mBrakes.end());
		itFindBrake->second->amount = amount;
	}


}

}
}
}