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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeVehicleTpl.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	VehicleTemplate::VehicleTemplate() : mNumSteerGroups(1)
	{
	}
	VehicleTemplate::~VehicleTemplate()
	{
	}
	size_t VehicleTemplate::getNumEngines() const
	{
		return mEngines.size();
	}
	const VehicleTemplate::EngineTemplate& VehicleTemplate::getEngine(const size_t index) const
	{
		YAKE_ASSERT( index < mEngines.size() );
		return mEngines[ index ];
	}
	void VehicleTemplate::addEngine( const EngineTemplate & tpl)
	{
		mEngines.push_back( tpl );
	}
	size_t VehicleTemplate::getNumAxles() const
	{
		return mAxles.size();
	}
	const VehicleTemplate::AxleTemplate& VehicleTemplate::getAxle(const size_t index) const
	{
		YAKE_ASSERT( index < mAxles.size() );
		return mAxles[ index ];
	}
	void VehicleTemplate::addAxle( const AxleTemplate & tpl)
	{
		mAxles.push_back( tpl );
	}
	size_t VehicleTemplate::getNumSteeringGroups() const
	{
		return mNumSteerGroups;
	}
	void VehicleTemplate::addSteeringGroup()
	{
		mNumSteerGroups++;
	}
	size_t VehicleTemplate::getNumWheels() const
	{
		return mWheels.size();
	}
	const VehicleTemplate::WheelTemplate& VehicleTemplate::getWheel(const size_t index) const
	{
		YAKE_ASSERT( index < mWheels.size() );
		return mWheels[ index ];
	}
	void VehicleTemplate::addWheel( const WheelTemplate & tpl)
	{
		mWheels.push_back( tpl );
	}

}
}
}
}