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
#ifndef YAKE_APP_MODEL_VEHICLE_INPUTCOMPONENT_H
#define YAKE_APP_MODEL_VEHICLE_INPUTCOMPONENT_H

#include <yapp/base/yappPrerequisites.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	enum VehicleInputComponentType {
		VICT_ENGINE,
		VICT_THRUSTER,
		VICT_CAR_ENGINE,
		VICT_CAR
	};

	enum VehicleGearMode {
		VGM_FORWARD,
		VGM_NEUTRAL,
		VGM_REVERSE
	};

	class YAPP_BASE_API IEngineInputComponent : public InputComponent
	{
	public:
		virtual ~IEngineInputComponent() {}

		/** Set throttle for a given engine. Value is in the range [0,1]. */
		virtual void throttleAnalog( const size_t idxEngine, const real throttle ) = 0;
	};
	class YAPP_BASE_API ICarEngineInputComponent : public IEngineInputComponent
	{
	public:
		/** Shift up for a given engine. */
		virtual void shiftGearUp( const size_t idxEngine ) = 0;
		/** Shift down for a given engine. */
		virtual void shiftGearDown( const size_t idxEngine ) = 0;
		/** Get current gear's mode (reverse, neutral, forward) for a given engine. */
		virtual VehicleGearMode getCurrentGearMode( const size_t idxEngine ) const = 0;
		/** Get current gear for a given engine. */
		virtual uint8 getCurrentGear( const size_t idxEngine ) const = 0;
	};
	class YAPP_BASE_API IThrusterInputComponent : public IEngineInputComponent
	{
	public:
	};

	class YAPP_BASE_API ICarInputComponent : public InputComponent
	{
	public:
		/** Set steering value. Value is in the range [-1,1]. */
		virtual void steerAnalog( const size_t idxSteerGroup, const real targetSteer ) = 0;

		/** Set braking value for a given brake. Value is in the range [0,1]. */
		virtual void brakeAnalog( const size_t idxBrake, const real ratio ) = 0;

		//virtual void handbrake( const bool activate ) = 0;
	};

}
}
}
}


#endif