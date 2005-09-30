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
#ifndef YAKE_APP_MODEL_VEHICLE_NATIVEENGINE_H
#define YAKE_APP_MODEL_VEHICLE_NATIVEENGINE_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeVehicleTpl.h>

namespace yake {
namespace model {
namespace vehicle {

	class YAPP_BASE_API NativeGearBox
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

		void setFromTemplate( const VehicleTemplate::GearTemplateList & gears );
	private:
		struct Gear
		{
			real		ratio_;
			GearMode	mode_;
		};
		typedef Vector< Gear > GearList;
		GearList	mGears;
	};

	class YAPP_BASE_API NativeCarEngine : public IEngine
	{
	public:
		NativeCarEngine();
		~NativeCarEngine();

		virtual void updateSimulation( real timeElapsed );

		void setFromTemplate( const VehicleTemplate::EngineTemplate & tpl );

		virtual void setParamMinRPM( const real rpm );
		virtual void setParamMaxRPM( const real rpm );
		virtual void setParamRedlineRPM( const real rpm );

		virtual void setThrottle( real throttle );
		virtual real getThrottle() const;

		virtual real getRPM() const;
		virtual uint8 getGear() const;
		virtual void shiftGear( uint8 gear );
		virtual void shiftGearUp();
		virtual void shiftGearDown();

		/** Returns the torque for the current engine data. [Nm] */
		virtual real getDriveTorque() const;

		//

		NativeGearBox& getGearBox();

		real getMinRPM() const;
		real getMaxRPM() const;

		/** Get the amount the RPM can rise [RPM/sec]. */
		real getRPMRise() const;
		/** Amount of RPM when decreasing when throttle is 0. [RPM/sec] */
		real getRPMDieOff() const;
		/** Minimum time needed to shift gears. */
		real getMinShiftingTime() const;
	private:
		real		mCurrentRPM;
		real		mMaxRPM;
		real		mMinRPM;
		real		mThrottle;
		real		mThrottleRPMRise;
		real		mRPMDieOff;
		real		mGearRatio;
		real		mInvGearRatio;
		real		mDifferentialRatio;
		NativeGearBox*	mpGearBox;
		uint8		mCurrentGear;
		real		mDriveTorque;
	private:
		void updateCurrentRPM( const real timeElapsed );
		void updateGearRatios();
		real getMaxTorque( const real rpm );
	};

}
}
}


#endif
