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
#ifndef YAKE_APP_MODEL_VEHICLE_PHYSICALCOMPONENT_H
#define YAKE_APP_MODEL_VEHICLE_PHYSICALCOMPONENT_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeActor.h>
#include <yapp/model/yakeModel.h>
#include <yapp/model/yakeEngine.h>
#include <yapp/model/yakeVehicleTpl.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	class YAPP_BASE_API IPhysicalVehicleComponent : public PhysicalComponent
	{
	public:
		YAKE_DECLARE_CLASS( IPhysicalVehicleComponent );
		YAKE_DECLARE_REGISTRY_1( IPhysicalVehicleComponent, ::yake::base::String, ::yake::physics::IWorld* );
		//VehiclePhysicalCaps getCaps() const = 0;

		virtual bool createFromTemplate( const VehicleTemplate & tpl ) = 0;

		/*
		virtual bool createChassis(const size_t index) = 0;
		virtual bool createEngine(const size_t index, const String & rTemplate) = 0;
		virtual bool createAxle(const size_t index, const String & rEngineName = "") = 0;
		virtual bool createSteerGroup(const size_t index) = 0;
		virtual bool createWheel(const size_t index, const String & rAxleName, const String & rSteerGroup = "") = 0;
		virtual bool createBrake(const size_t index) = 0;
		*/

		virtual IEngine* getEngine(const size_t index) = 0;

		virtual void setWheelMass(const size_t index, const real mass) = 0;
		virtual void setWheelRadius(const size_t index, const real r) = 0;
		virtual void setWheelPosition(const size_t index, const Vector3 & position) = 0;
		virtual void setWheelSuspension(const size_t index, const real stiffness) = 0;
		virtual void setWheelFriction(const size_t index, const real f) = 0;
		virtual void setWheelLateralFriction(const size_t index, const real f) = 0;

		virtual void setSteer(const size_t index, const real steerValue) = 0;
		virtual void setBrake(const size_t index, const real brakeValue) = 0;
		virtual void setEngineThrottle(const size_t index, const real throttle) = 0;

		virtual Movable* getChassisMovable() = 0;
		virtual Movable* getWheelMovable(const size_t index) = 0;

		virtual void update(const real timeElapsed) = 0;

		/** As not every implementation necessarily supports all of this information
			these flags are specified or'd in a WheelCollisionInfo::flags_ member.
			@Remarks WIP. will be revised.
		*/
		enum WheelCollisionFlag
		{
			WCF_NAME = 0x00000001,
			WCF_OTHEROBJ = 0x00000002,
			WCF_OTHEROBJ_NAME = 0x00000004,
			WCF_OTHEROBJ_MATERIAL_NAME = 0x00000008
		};
		struct WheelCollisionInfo
		{
			uint32						flags_;
			String						wheelName_;
			physics::IActor*	otherObj_;
			String						otherObjName_;
			String						otherObjMaterialName_;
		};
		typedef Signal1<void(const WheelCollisionInfo&)> WheelCollisionSignal;
		virtual void subscribeToOnWheelCollision( const WheelCollisionSignal::slot_type & slot ) = 0;
	};


}
}
}
}


#endif