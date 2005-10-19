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
	// Class: GearBox
	//-----------------------------------------------------
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
	void GearBox::setFromTemplate( const VehicleTemplate::GearTplList & gears )
	{
		mGears.clear();
		ConstVectorIterator< VehicleTemplate::GearTplList > it( gears.begin(), gears.end() );
		while (it.hasMoreElements())
		{
			const VehicleTemplate::GearTpl & tpl = it.getNext();
			Gear g;
			switch( tpl.mode_ )
			{
			case VehicleTemplate::GM_FORWARD:
				g.mode_ = GM_FORWARD;
				break;
			case VehicleTemplate::GM_REVERSE:
				g.mode_ = GM_REVERSE;
				break;
			case VehicleTemplate::GM_NEUTRAL:
				g.mode_ = GM_NEUTRAL;
				break;
			};
			g.ratio_ = tpl.ratio_;
			mGears.push_back( g );
		}
	}

	//-----------------------------------------------------
	// Class: GenericCarEngine
	//-----------------------------------------------------
	GenericCarEngine::GenericCarEngine() :
		mDifferentialRatio( real(3.42) ),
		mThrottle(0),
		mCurrentGear(0),
		mDriveTorque(0),
		mpGearBox(0),
		mMinRPM(0),
		mMaxRPM(3500),
		mThrottleRPMRise(2500),
		mRPMDieOff(3500)
	{
		mpGearBox = new GearBox();
		YAKE_ASSERT( mpGearBox );
		mpGearBox->setSpeed( 6 );
		mpGearBox->setGearRatio( 0, real(-2.90 ) ); mpGearBox->setGearMode( 0, GearBox::GM_REVERSE );
		mpGearBox->setGearRatio( 1, real( 2.66 ) ); mpGearBox->setGearMode( 1, GearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 2, real( 1.78 ) ); mpGearBox->setGearMode( 2, GearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 3, real( 1.30 ) ); mpGearBox->setGearMode( 3, GearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 4, real( 0.74 ) ); mpGearBox->setGearMode( 4, GearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 5, real( 0.50 ) ); mpGearBox->setGearMode( 5, GearBox::GM_FORWARD );
	}
	GenericCarEngine::~GenericCarEngine()
	{
		YAKE_SAFE_DELETE( mpGearBox );
	}
	void GenericCarEngine::updateSimulation( real timeElapsed )
	{
		updateCurrentRPM( timeElapsed );
		updateGearRatios();

		//engine torque = throttle position * max torque
		//drive torque = engine_torque * gear_ratio * differential_ratio * transmission_efficiency
		//drive torque gets delivered to the wheels
		real engineTorque = mThrottle * getMaxTorque( mCurrentRPM );
		mDriveTorque = engineTorque * mGearRatio * mDifferentialRatio * 0.75/*just a guess*/;
	}
	GearBox& GenericCarEngine::getGearBox()
	{
		YAKE_ASSERT( mpGearBox );
		return *mpGearBox;
	}
	void GenericCarEngine::setThrottle( real throttle )
	{
		mThrottle = throttle;
		if (mThrottle > 1)
			mThrottle = 1;
		else if (mThrottle < 0)
			mThrottle = 0;
	}
	real GenericCarEngine::getThrottle() const
	{
		return mThrottle;
	}
	real GenericCarEngine::getRPM() const
	{
		return mCurrentRPM;
	}
	uint8 GenericCarEngine::getGear() const
	{
		return mCurrentGear;
	}
	void GenericCarEngine::shiftGear( uint8 gear )
	{
		if (gear < mpGearBox->getSpeed())
			mCurrentGear = gear;
	}
	void GenericCarEngine::shiftGearUp()
	{
		if (mCurrentGear+1 < mpGearBox->getSpeed())
			mCurrentGear++;
	}
	void GenericCarEngine::shiftGearDown()
	{
		if (mCurrentGear > 0)
			mCurrentGear--;
	}
	real GenericCarEngine::getDriveTorque() const
	{
		return mDriveTorque;
	}
	real GenericCarEngine::getMinRPM() const
	{
		return mMinRPM;
	}
	real GenericCarEngine::getMaxRPM() const
	{
		return mMaxRPM;
	}
	real GenericCarEngine::getRPMRise() const
	{
		return mThrottleRPMRise;
	}
	real GenericCarEngine::getRPMDieOff() const
	{
		return mRPMDieOff;
	}
	real GenericCarEngine::getMinShiftingTime() const
	{
		return 0.;
	}
	real GenericCarEngine::getMaxTorque( const real rpm )
	{
		//TODO: look up in curve
		return 500; // in Nm
	}
	void GenericCarEngine::updateGearRatios()
	{
		mGearRatio = mpGearBox->getGearRatio( mCurrentGear );
		mInvGearRatio = (mGearRatio != 0.) ? (1.0 / mGearRatio) : 0.;
	}
	void GenericCarEngine::updateCurrentRPM(real timeElapsed)
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
	void GenericCarEngine::setParamMinRPM( const real rpm )
	{
		mMinRPM = rpm;
	}
	void GenericCarEngine::setParamMaxRPM( const real rpm )
	{
		mMaxRPM = rpm;
	}
	void GenericCarEngine::setParamRedlineRPM( const real rpm )
	{
	}
} // namespace vehicle
} // namespace yake
