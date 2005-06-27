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
#ifndef YAKE_APP_MODEL_VEHICLE_NATIVE_PHYSICALCOMPONENT_H
#define YAKE_APP_MODEL_VEHICLE_NATIVE_PHYSICALCOMPONENT_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeVehiclePhysicalComponent.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	class NativeWheel;
	/** WIP version of the "yake.native" implementation of a IPhysicalVehicleComponent.
		@Remarks Work-in-progress version.
	*/
	class YAPP_BASE_API NativePhysicalVehicleComponent : public IPhysicalVehicleComponent
	{
	public:
		YAKE_DECLARE_CLASS( NativePhysicalVehicleComponent );
		YAKE_DECLARE_CONCRETE( NativePhysicalVehicleComponent, "yake.native" );

		NativePhysicalVehicleComponent(::yake::physics::IWorld* pWorld);
		virtual ~NativePhysicalVehicleComponent();

		virtual bool createFromTemplate( const VehicleTemplate & tpl );
		/*
		virtual bool createChassis(const String & rName);
		virtual bool createEngine(const String & rName, const String & rTemplate) ;
		virtual bool createAxle(const String & rName, const String & rEngineName = "") ;
		virtual bool createSteerGroup(const String & rName) ;
		virtual bool createWheel(const String & rName, const String & rAxleName, const String & rSteerGroup = "") ;
		virtual bool createBrake(const String & rName) ;
		*/

		virtual IEngine* getEngine(const size_t index) ;

		//virtual void attachComplexModel(::yake::app::model::complex::Model* pModel);

		/*
		virtual bool attachAxleToEngine(const String & axleName, const String & engineName) ;
		virtual bool attachWheelToAxle(const String & wheelName, const String & axleName) ;
		virtual bool attachWheelToBrake(const String & wheelName, const String & brakeName, const real influence) ;
		virtual bool attachWheelToSteerGroup(const String & wheelName, const String & steerGroup) ;
		*/
		virtual void update(const real timeElapsed);

		virtual void setWheelMass(const size_t index, const real mass);
		virtual void setWheelRadius(const size_t index, const real r);
		virtual void setWheelPosition(const size_t index, const Vector3 & position);
		virtual void setWheelSuspension(const size_t index, const real stiffness) ;
		virtual void setWheelFriction(const size_t index, const real f) ;
		virtual void setWheelLateralFriction(const size_t index, const real f) ;

		virtual void setSteer(const size_t index, const real steerValue) ;
		virtual void setBrake(const size_t index, const real brakeValue) ;
		virtual void setEngineThrottle(const size_t index, const real throttle) ;

		virtual Movable* getChassisMovable();
		virtual Movable* getWheelMovable(const size_t index);

		virtual void subscribeToOnWheelCollision( const WheelCollisionSignal::slot_type & slot )
		{ mWheelCollisionSig.connect( slot ); }

		void translate( const Vector3 & d );
	private:
		typedef ::yake::base::templates::Vector< NativeEngine* > EngineList;
		EngineList		mEngines;

		typedef ::yake::base::templates::Vector< NativeWheel* > WheelList;
		WheelList		mWheels;

		struct Axle
		{
			WheelList		wheels;
			NativeEngine*	engine;
			Axle() : engine(0)
			{
			}
		};
		typedef ::yake::base::templates::Vector< Axle* > AxleList;
		AxleList		mAxles;

		struct SteerGroup
		{
			real		steer;
			WheelList	wheels;
			SteerGroup() : steer(0)
			{
			}
		};

		typedef ::yake::base::templates::Vector< SteerGroup* > SteerGroupList;
		SteerGroupList						mSteerGroups;

		physics::IWorld*					mpPWorld;

		WheelCollisionSignal				mWheelCollisionSig;

		physics::IActor*			mpChassis;
		Physical*							mpPhysical;
	private:
		SteerGroup* _getSteerGroup(const size_t index) const;
		Axle* _getAxle(const size_t index) const;
		NativeEngine* _getEngine(const size_t index) const;
		NativeWheel* _getWheel(const size_t index) const;
		void _reset();
	};

}
}
}
}


#endif