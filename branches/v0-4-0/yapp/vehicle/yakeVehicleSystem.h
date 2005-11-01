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
#ifndef YAKE_VEHICLE_SYSTEM_H
#define YAKE_VEHICLE_SYSTEM_H

#include "yakePrerequisites.h"

namespace yake {
namespace vehicle {

	class IVehicle;
	class YAKE_VEH_API IVehicleSystem
	{
		YAKE_DECLARE_REGISTRY_0(IVehicleSystem,String);
	public:
		virtual ~IVehicleSystem() {}
		virtual IVehicle* create(const VehicleTemplate&, physics::IWorld& PWorld) = 0;
		virtual bool loadTemplates(const String& fn) = 0;
		//virtual VehicleTemplate* cloneTemplate(const String& tpl) = 0;
		virtual VehicleTemplate* getTemplate(const String& tpl) const = 0;
		virtual IVehicle* create(const String& tpl, physics::IWorld& PWorld) = 0;
	};

} // namespace vehicle
} // namespace yake


#endif
