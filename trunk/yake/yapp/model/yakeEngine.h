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
#ifndef YAKE_APP_MODEL_VEHICLE_ENGINE_H
#define YAKE_APP_MODEL_VEHICLE_ENGINE_H

#include <yapp/base/yappPrerequisites.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	/** Interface to a vehicle engine.
		@remarks This is work-in-progress and may be revised at any time.
	*/
	class YAPP_BASE_API IEngine
	{
	public:
		virtual ~IEngine() 
		{}

		virtual void updateSimulation( real timeElapsed ) = 0;

		virtual void setThrottle( real throttle ) = 0;
		virtual real getThrottle() const = 0;
	};

	/** Interface to a vehicle engine.
		@remarks This is work-in-progress and may be revised at any time.
	*/
	class YAPP_BASE_API ICarEngine : public IEngine
	{
	public:
		virtual ~ICarEngine() 
		{}

		//virtual void setParameter( const String & name, const String & value );
		virtual void setParamMinRPM( const real rpm ) = 0;
		virtual void setParamMaxRPM( const real rpm ) = 0;
		virtual void setParamRedlineRPM( const real rpm ) = 0;

		virtual real getRPM() const = 0;
		virtual uint8 getGear() const = 0;
		virtual void shiftGear( uint8 gear ) = 0;
		virtual void shiftGearUp() = 0;
		virtual void shiftGearDown() = 0;

		/** Returns the torque for the current engine data. [Nm] */
		virtual real getDriveTorque() const = 0;
	};


}
}
}
}


#endif