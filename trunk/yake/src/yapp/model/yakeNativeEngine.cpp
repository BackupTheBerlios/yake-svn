/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#include <yapp/model/yakeVehicleSystem.h>
#include <yapp/model/yakeEngine.h>
#include <yapp/model/yakeNativeEngine.h>
#include <yapp/model/yakeVehiclePhysicalComponent.h>

namespace yake {
namespace model {
namespace vehicle {


	void NativeGearBox::setSpeed(uint8 numGears)
	{
		mGears.resize( numGears );
	}
	uint8 NativeGearBox::getSpeed() const
	{
		size_t size = mGears.size();
		YAKE_ASSERT( size <= 255 );
		return static_cast<uint8>(size);
	}
	void NativeGearBox::setGearRatio(uint8 gear, real ratio)
	{
		YAKE_ASSERT( gear < getSpeed() );
		Gear& g = mGears[gear];
		g.ratio_ = ratio;
	}
	void NativeGearBox::setGearMode( uint8 gear, const GearMode mode )
	{
		YAKE_ASSERT( gear < getSpeed() );
		Gear& g = mGears[gear];
		g.mode_ = mode;
	}
	real NativeGearBox::getGearRatio( uint8 gear )
	{
		YAKE_ASSERT( gear < getSpeed() );
		Gear& g = mGears[gear];
		return g.ratio_;
	}
	void NativeGearBox::setFromTemplate( const VehicleTemplate::GearTemplateList & gears )
	{
		mGears.clear();
		ConstVectorIterator< VehicleTemplate::GearTemplateList > it( gears.begin(), gears.end() );
		while (it.hasMoreElements())
		{
			const VehicleTemplate::GearTemplate & tpl = it.getNext();
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


	NativeCarEngine::NativeCarEngine() :
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
		mpGearBox = new NativeGearBox();
		YAKE_ASSERT( mpGearBox );
		mpGearBox->setSpeed( 6 );
		mpGearBox->setGearRatio( 0, real(-2.90 ) ); mpGearBox->setGearMode( 0, NativeGearBox::GM_REVERSE );
		mpGearBox->setGearRatio( 1, real( 2.66 ) ); mpGearBox->setGearMode( 1, NativeGearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 2, real( 1.78 ) ); mpGearBox->setGearMode( 2, NativeGearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 3, real( 1.30 ) ); mpGearBox->setGearMode( 3, NativeGearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 4, real( 0.74 ) ); mpGearBox->setGearMode( 4, NativeGearBox::GM_FORWARD );
		mpGearBox->setGearRatio( 5, real( 0.50 ) ); mpGearBox->setGearMode( 5, NativeGearBox::GM_FORWARD );
	}
	NativeCarEngine::~NativeCarEngine()
	{
		YAKE_SAFE_DELETE( mpGearBox );
	}
	void NativeCarEngine::setFromTemplate( const VehicleTemplate::EngineTemplate & tpl )
	{
		const VehicleTemplate::CarEngineTemplate& engineTpl =
			static_cast<const VehicleTemplate::CarEngineTemplate&>(tpl);
		mMinRPM				= engineTpl.rpmMin_;
		mMaxRPM				= engineTpl.rpmMax_;
		mThrottleRPMRise	= engineTpl.rpmRise_;
		mRPMDieOff			= engineTpl.rpmDieOff_;
		mDifferentialRatio	= engineTpl.differentialRatio;
		YAKE_ASSERT( mpGearBox );
		mpGearBox->setFromTemplate( engineTpl.gears_ );
	}
	void NativeCarEngine::updateSimulation( real timeElapsed )
	{
		updateCurrentRPM( timeElapsed );
		updateGearRatios();

		//engine torque = throttle position * max torque
		//drive torque = engine_torque * gear_ratio * differential_ratio * transmission_efficiency
		//drive torque gets delivered to the wheels
		real engineTorque = mThrottle * getMaxTorque( mCurrentRPM );
		mDriveTorque = engineTorque * mGearRatio * mDifferentialRatio * 0.75/*just a guess*/;
	}
	void NativeCarEngine::setThrottle( real throttle )
	{
		mThrottle = throttle;
		if (mThrottle > 1)
			mThrottle = 1;
		else if (mThrottle < 0)
			mThrottle = 0;
	}
	real NativeCarEngine::getThrottle() const
	{
		return mThrottle;
	}
	real NativeCarEngine::getRPM() const
	{
		return mCurrentRPM;
	}
	uint8 NativeCarEngine::getGear() const
	{
		return mCurrentGear;
	}
	void NativeCarEngine::shiftGear( uint8 gear )
	{
		if (gear < mpGearBox->getSpeed())
			mCurrentGear = gear;
	}
	void NativeCarEngine::shiftGearUp()
	{
		if (mCurrentGear+1 < mpGearBox->getSpeed())
			mCurrentGear++;
	}
	void NativeCarEngine::shiftGearDown()
	{
		if (mCurrentGear > 0)
			mCurrentGear--;
	}
	real NativeCarEngine::getDriveTorque() const
	{
		return mDriveTorque;
	}
	real NativeCarEngine::getMinRPM() const
	{
		return mMinRPM;
	}
	real NativeCarEngine::getMaxRPM() const
	{
		return mMaxRPM;
	}
	real NativeCarEngine::getRPMRise() const
	{
		return mThrottleRPMRise;
	}
	real NativeCarEngine::getRPMDieOff() const
	{
		return mRPMDieOff;
	}
	real NativeCarEngine::getMinShiftingTime() const
	{
		return 0.;
	}
	real NativeCarEngine::getMaxTorque( const real rpm )
	{
		//TODO: look up in curve
		return 500; // in Nm
	}
	void NativeCarEngine::updateGearRatios()
	{
		mGearRatio = mpGearBox->getGearRatio( mCurrentGear );
		mInvGearRatio = (mGearRatio != 0.) ? (1.0 / mGearRatio) : 0.;
	}
	void NativeCarEngine::updateCurrentRPM(real timeElapsed)
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
	void NativeCarEngine::setParamMinRPM( const real rpm )
	{
		mMinRPM = rpm;
	}
	void NativeCarEngine::setParamMaxRPM( const real rpm )
	{
		mMaxRPM = rpm;
	}
	void NativeCarEngine::setParamRedlineRPM( const real rpm )
	{
	}

} // ns vehicle
} // ns model
} // ns yake

