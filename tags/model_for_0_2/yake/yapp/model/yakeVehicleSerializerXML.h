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
#ifndef YAKE_DATA_VEHICLE_H
#define YAKE_DATA_VEHICLE_H

#include <yake/data/yakeData.h>
#include <yapp/model/yakeVehicleTpl.h>

#pragma warning(disable: 4244)

namespace yake {
namespace data {
namespace serializer {
namespace vehicle {

	/** VehicleReader base class providing default implementation for DOM parsing.
	*/
	class VehicleSerializer
	{
	public:
		VehicleSerializer();
		~VehicleSerializer() {}

		String getName() const
		{ return "yake.data.vehicle"; }

		virtual Version getVersion() const = 0;

		/** Build vehicle from DOM.
			\param
			\return 
		*/
		virtual bool load(	const SharedPtr<dom::INode> & docNode,
							app::model::vehicle::VehicleTemplate & tpl);

		/** Prepare for next run of load/store.
		*/
		virtual void reset();
	protected:
		/// Default implementations for common functions.

		virtual void readVehicle( const SharedPtr<dom::INode> & pNode );
		virtual void readChassis( const SharedPtr<dom::INode> & pNodes );
		virtual void readGeoms( const SharedPtr<dom::INode> & pNodes, app::model::vehicle::VehicleTemplate::ChassisTemplate & chassisTpl );
			virtual void readGeom( const SharedPtr<dom::INode> & pNode, app::model::vehicle::VehicleTemplate::ChassisTemplate & chassisTpl );
				virtual void readGeomDimensions( const SharedPtr<dom::INode> & pNode, Vector3 & dimensions );
		virtual void readEngines( const SharedPtr<dom::INode> & pNodes );
			virtual void readEngine( const SharedPtr<dom::INode> & pNode );
				virtual void readRPM( const SharedPtr<dom::INode> & pNode, app::model::vehicle::VehicleTemplate::EngineTemplate& engineTpl );
				virtual void readGearbox( const SharedPtr<dom::INode> & pNode, app::model::vehicle::VehicleTemplate::EngineTemplate& engineTpl );
		virtual void readAxles( const SharedPtr<dom::INode> & pNodes );
			virtual void readAxle( const SharedPtr<dom::INode> & pNode );
		virtual void readWheels( const SharedPtr<dom::INode> & pNodes );
			virtual void readWheel( const SharedPtr<dom::INode> & pNode );
				virtual void readSuspension( const SharedPtr<dom::INode> & pNode, real & spring, real & damping );
		virtual void readOrientation( const SharedPtr<dom::INode> & pNode, Quaternion & orientation );
		virtual void readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position );
	protected:
		SharedPtr<dom::INode>							mDocNode;
		app::model::vehicle::VehicleTemplate*			mpTpl;
	};
	class VehicleSerializerV1 : public VehicleSerializer
	{
	public:
		virtual Version getVersion() const
		{ return Version(0,1,0); }
	};

} // dotscene
} // serializer
} // data
} // yake

#endif
