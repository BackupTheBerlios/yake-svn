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
#ifndef YAKE_APP_MODEL_VEHICLE_SYSTEM_H
#define YAKE_APP_MODEL_VEHICLE_SYSTEM_H

#include <yapp/base/yappPrerequisites.h>

namespace yake {
namespace model {
namespace vehicle {

	class Vehicle;
	class IPhysicalVehicleComponent;

	class YAPP_BASE_API IVehicleSystem
	{
	public:
		YAKE_DECLARE_CLASS( IVehicleSystem );
		YAKE_DECLARE_REGISTRY_0( IVehicleSystem, ::yake::String );

		virtual Vehicle* createVehicle( const ::yake::String & rTemplate,
										physics::IWorld* pPWorld, 
										graphics::IWorld* pGWorld ) = 0;
	};

	class YAPP_BASE_API NativeVehicleSystem : public IVehicleSystem
	{
	public:
		YAKE_DECLARE_CLASS( NativeVehicleSystem );
		YAKE_DECLARE_CONCRETE( NativeVehicleSystem, "yake.native" );

		virtual Vehicle* createVehicle( const ::yake::String & rTemplate, 
										physics::IWorld* pWorld, 
										graphics::IWorld* pGWorld )
		{ return 0; }
	private:
		bool _loadModelFromDotVehicle(::yake::model::complex::Model* pModel,
									const ::yake::String & rFN, 
									physics::IWorld* pPWorld,
									graphics::IWorld* pGWorld,
									IPhysicalVehicleComponent* pPC
									);
	};


}
}
}

#endif
