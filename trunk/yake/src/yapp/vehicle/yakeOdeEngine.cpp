/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
	class GearBox
	{
	public:
		enum GearMode {
			GM_REVERSE,
			GM_NEUTRAL,
			GM_FORWARD
		};
		void setSpeed( uint8 numGears );
		uint8 getSpeed() const;

		void setGearRatio( uint8 gear, real ratio );
		void setGearMode( uint8 gear, const GearMode mode );
		real getGearRatio( uint8 gear );

		void setFromTemplate( const VehicleTemplate::GearTplList & gears );
	private:
		struct Gear
		{
			real		ratio_;
			GearMode	mode_;
		};
		typedef Vector< Gear > GearList;
		GearList	mGears;
	};
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
	// Class: OdeCarEngine
	//-----------------------------------------------------
	OdeCarEngine::OdeCarEngine() :
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
	OdeCarEngine::~OdeCarEngine()
	{
		YAKE_SAFE_DELETE( mpGearBox );
	}
	void OdeCarEngine::updateSimulation( real timeElapsed )
	{
		updateCurrentRPM( timeElapsed );
		updateGearRatios();

		//engine torque = throttle position * max torque
		//drive torque = engine_torque * gear_ratio * differential_ratio * transmission_efficiency
		//drive torque gets delivered to the wheels
		real engineTorque = mThrottle * getMaxTorque( mCurrentRPM );
		mDriveTorque = engineTorque * mGearRatio * mDifferentialRatio * 0.75/*just a guess*/;
	}
	void OdeCarEngine::setThrottle( real throttle )
	{
		mThrottle = throttle;
		if (mThrottle > 1)
			mThrottle = 1;
		else if (mThrottle < 0)
			mThrottle = 0;
	}
	real OdeCarEngine::getThrottle() const
	{
		return mThrottle;
	}
	real OdeCarEngine::getRPM() const
	{
		return mCurrentRPM;
	}
	uint8 OdeCarEngine::getGear() const
	{
		return mCurrentGear;
	}
	void OdeCarEngine::shiftGear( uint8 gear )
	{
		if (gear < mpGearBox->getSpeed())
			mCurrentGear = gear;
	}
	void OdeCarEngine::shiftGearUp()
	{
		if (mCurrentGear+1 < mpGearBox->getSpeed())
			mCurrentGear++;
	}
	void OdeCarEngine::shiftGearDown()
	{
		if (mCurrentGear > 0)
			mCurrentGear--;
	}
	real OdeCarEngine::getDriveTorque() const
	{
		return mDriveTorque;
	}
	real OdeCarEngine::getMinRPM() const
	{
		return mMinRPM;
	}
	real OdeCarEngine::getMaxRPM() const
	{
		return mMaxRPM;
	}
	real OdeCarEngine::getRPMRise() const
	{
		return mThrottleRPMRise;
	}
	real OdeCarEngine::getRPMDieOff() const
	{
		return mRPMDieOff;
	}
	real OdeCarEngine::getMinShiftingTime() const
	{
		return 0.;
	}
	real OdeCarEngine::getMaxTorque( const real rpm )
	{
		//TODO: look up in curve
		return 500; // in Nm
	}
	void OdeCarEngine::updateGearRatios()
	{
		mGearRatio = mpGearBox->getGearRatio( mCurrentGear );
		mInvGearRatio = (mGearRatio != 0.) ? (1.0 / mGearRatio) : 0.;
	}
	void OdeCarEngine::updateCurrentRPM(real timeElapsed)
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
	void OdeCarEngine::setParamMinRPM( const real rpm )
	{
		mMinRPM = rpm;
	}
	void OdeCarEngine::setParamMaxRPM( const real rpm )
	{
		mMaxRPM = rpm;
	}
	void OdeCarEngine::setParamRedlineRPM( const real rpm )
	{
	}
} // namespace vehicle
} // namespace yake
