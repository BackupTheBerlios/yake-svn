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
#ifndef YAKE_VEHICLE_DOTVEHICLE_H
#define YAKE_VEHICLE_DOTVEHICLE_H

#include "yakePrerequisites.h"
#include "yakeTemplates.h"

namespace yake {
	namespace data {
		namespace dom {
			class INode;
		}
	}
namespace vehicle {

	class YAKE_VEH_API DotVehicleParser
	{
	public:
		DotVehicleParser();
		bool parse( const String& fn );
		bool parse( const data::dom::INode& rootNode );

		VehicleTemplate* detachCurrentVehicleTpl();

		YAKE_MEMBERSIGNAL(public,void(DotVehicleParser&,const String&),OnVehicleTpl);
	private:
		void parseVehicleTpl( const data::dom::INode& n );
		 void parseChassis( const data::dom::INode& n );
		  void parseBody( const data::dom::INode& n );
		  void parseShapeSet( const data::dom::INode& n );
		   void parseShape( const data::dom::INode& n );
		    void parseShapeBox( const data::dom::INode& n, const String& matId );
		    void parseShapeSphere( const data::dom::INode& n, const String& matId );
		   void parseMountPoint( const data::dom::INode& n, VehicleTemplate::MountPointTpl* parentMtPt = 0 );
		 void parseEngine( const data::dom::INode& n );
		  void parseRPM( vehicle::VehicleTemplate::CarEngineTpl&, const data::dom::INode& n );
		  void parseGearBox( vehicle::VehicleTemplate::CarEngineTpl&, const data::dom::INode& n );
		 //void parseAxle( const data::dom::INode& n );
		 void parseSteeringGroup( const data::dom::INode& n );
		 void parseWheel( const data::dom::INode& n );
		void parseEngineTpl( const data::dom::INode& n );

		void parsePosition( const data::dom::INode& n, Vector3& ret );
		void parseOrientation( const data::dom::INode& n, Quaternion& ret );
		void parseDirection( const data::dom::INode& n, Vector3& ret );

		VehicleTemplate*	mpCurrVehTpl;
	};

} // namespace vehicle
} // namespace yake


#endif
