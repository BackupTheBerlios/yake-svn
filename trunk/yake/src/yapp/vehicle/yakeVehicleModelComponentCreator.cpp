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

namespace yake {
namespace vehicle {

	//-----------------------------------------------------
	// Class: VehicleModelComponentCreator
	//-----------------------------------------------------
	class VehicleModelComponentCreator : public model::ComponentCreator
	{
		YAKE_DECLARE_CONCRETE(VehicleModelComponentCreator,"yake/dotVehicle");

		VehicleModelComponentCreator();
		virtual void create(const model::ComponentCreationContext& ctx, const StringMap& params);

	private:
	};

	//-----------------------------------------------------
	// Class: VehicleModelComponentCreator
	//-----------------------------------------------------
	YAKE_REGISTER_CONCRETE(VehicleModelComponentCreator)

	VehicleModelComponentCreator::VehicleModelComponentCreator()
	{
	}
	void VehicleModelComponentCreator::create(const model::ComponentCreationContext& ctx, const StringMap& params)
	{
		// Verify validity of creation context
		physics::IWorld* pPWorld = ctx.getPhysicalWorld();
		YAKE_ASSERT( pPWorld );
		if (!pPWorld)
			return;

		// Extract parameters

		const String name = ctx.getName();

		StringMap::const_iterator itParam = params.find("file");
		const String fn = (itParam == params.end()) ? "" : itParam->second;

		itParam = params.find("physical");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'physical'.");
		if (itParam == params.end())
			return;
		const String physicalTag = itParam->second;

		itParam = params.find("template");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'template'.");
		if (itParam == params.end())
			return;
		const String tplname = itParam->second;

		// Retrieve vehicle system

		IVehicleSystem* vehSys = 0;
		{
			const bool success = ctx.getParamValueAs<vehicle::IVehicleSystem*>("vehicleSystem",vehSys);
			YAKE_ASSERT( success )(name).debug("Failed to retrieve IVehicleSystem interface from context parameter!");
				return;
			YAKE_ASSERT( vehSys )(name).debug("IVehicleSystem interface not set.");
			if (!vehSys)
				return;
		}

		// Load template if necessary
		if (!fn.empty())
		{
			const bool succ = vehSys->loadTemplates( fn );
			YAKE_ASSERT( succ )(name)(fn).debug("Failed to load template(s).");
			if (!succ)
				return;
		}

		// Retrieve 'Physical'

		model::Physical* pPhysical = dynamic_cast<model::Physical*>(ctx.getModel().getComponentByTag(physicalTag));
		YAKE_ASSERT( pPhysical )(physicalTag)(name).debug("Failed to retrieve Physical.");

		// Instantiate template vehicle

		IVehicle* pVehicle = vehSys->create(tplname, *pPWorld, *pPhysical );
		YAKE_ASSERT( pVehicle )(tplname).debug("Failed to instantiate vehicle.");
		if (!pVehicle)
			return;

		// Create VehicleModelComponent
		VehicleModelComponent* c = new VehicleModelComponent(pVehicle);
		YAKE_ASSERT( c );

		ctx.getModel().addComponent( c, name );

		return;
	}

} // namespace vehicle
} // namespace yake

