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

#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yake/data/yakeData.h>
#include <yapp/model/yakeVehicleSerializerXML.h>

#pragma warning(disable: 4244)

#ifdef YAKE_DEBUG
#	define COUTLN( X ) \
		std::cout << X << std::endl;
#	define COUTLN_V3( X ) \
		std::cout << #X << " (" << X.x << ", " << X.y << ", " << X.z << ")" << std::endl;
#else
#	define COUTLN( X )
#	define COUTLN_V3( X )
#endif

namespace yake {
namespace data {
namespace serializer {
namespace vehicle {

	//------------------------------------------------------
	VehicleSerializer::VehicleSerializer() : mpTpl(0)
	{
	}
	//------------------------------------------------------
	void VehicleSerializer::reset()
	{
		mDocNode.reset();
		mpTpl = 0;
	}

	//------------------------------------------------------
	bool VehicleSerializer::load(	const SharedPtr<dom::INode> & docNode,
									app::model::vehicle::VehicleTemplate& tpl)
	{
		COUTLN( "load()" );
		YAKE_ASSERT( docNode.get() );
		if (!docNode.get())
			return false;
		mDocNode = docNode;
		mpTpl = &tpl;
		//
		COUTLN( "parsing vehicle..." );
		readVehicle( mDocNode );

		//
		return true;
	}

	//------------------------------------------------------
	void VehicleSerializer::readVehicle( const SharedPtr<dom::INode> & pNode )
	{
		const String name = varGet<String>(pNode->getValue("name"));
		COUTLN( "readVehicle() [" << name << "]" );
		YAKE_ASSERT( pNode );

		SharedPtr<dom::INode> pNodes = pNode->getNodeByName("chassis");
		COUTLN( "scene: found chassis = " << (pNodes.get() ? "yes" : "no") );
		if (pNodes.get())
			readChassis( pNodes );

		pNodes = pNode->getNodeByName("engines");
		COUTLN( "scene: found engines = " << (pNodes.get() ? "yes" : "no") );
		if (pNodes.get())
			readEngines( pNodes );

		pNodes = pNode->getNodeByName("axles");
		COUTLN( "scene: found axles = " << (pNodes.get() ? "yes" : "no") );
		if (pNodes.get())
			readAxles( pNodes );

		pNodes = pNode->getNodeByName("wheels");
		COUTLN( "scene: found wheels = " << (pNodes.get() ? "yes" : "no") );
		if (pNodes.get())
			readWheels( pNodes );
	}
	//------------------------------------------------------
	void VehicleSerializer::readEngines( const SharedPtr<dom::INode> & pNodes )
	{
		COUTLN( "readEngines()" );
		YAKE_ASSERT( pNodes );
		const dom::NodeList & nodes = pNodes->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readEngine( (*it) );
		}
	}
	//------------------------------------------------------
	void VehicleSerializer::readEngine( const SharedPtr<dom::INode> & pNode )
	{
		String id = varGet<String>(pNode->getAttributeValue("id"));
		String tag = varGet<String>(pNode->getAttributeValue("tag"));
		COUTLN( "readEngine() [id=" << id << " tag=" << tag << "]" );
		YAKE_ASSERT( pNode );

		// create template
		app::model::vehicle::VehicleTemplate::EngineTemplate engineTpl;

		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = varGet<String>((*it)->getValue("name"));
			childNodeName = ::yake::base::StringUtil::toLowerCase(childNodeName);
			COUTLN( "   (child: " <<  childNodeName << ")" );

			const SharedPtr<dom::INode> & pChild = (*it);
			if (childNodeName == "rpm")
			{
				readRPM( pChild, engineTpl );
			}
		}

		YAKE_ASSERT( mpTpl );
		mpTpl->addEngine( engineTpl );
	}
	//------------------------------------------------------
	void VehicleSerializer::readRPM( const SharedPtr<dom::INode> & pNode, app::model::vehicle::VehicleTemplate::EngineTemplate& engineTpl )
	{
		COUTLN( "  readRPM()" );
		YAKE_ASSERT( pNode );

		engineTpl.rpmMin_ = StringUtil::toReal( pNode->getAttributeValueAs<String>("min") );
		engineTpl.rpmMax_ = StringUtil::toReal( pNode->getAttributeValueAs<String>("max") );
		engineTpl.rpmRedline_ = StringUtil::toReal( pNode->getAttributeValueAs<String>("redline") );
	}
	//------------------------------------------------------
	void VehicleSerializer::readGearbox( const SharedPtr<dom::INode> & pNode, app::model::vehicle::VehicleTemplate::EngineTemplate& engineTpl )
	{
		using namespace app::model::vehicle;
		COUTLN( "  readGearbox()" );
		YAKE_ASSERT( pNode );
		const dom::NodeList & nodes = pNode->getNodes();
		templates::ConstVectorIterator< dom::NodeList > it( nodes.begin(), nodes.end() );
		while (it.hasMoreElements())
		{
			const SharedPtr<dom::INode> & pGearNode = it.getNext();
			VehicleTemplate::GearTemplate gear;
			String mode = pGearNode->getAttributeValueAs<String>("mode");
			if (mode == "reverse")
				gear.mode_ = VehicleTemplate::GM_REVERSE;
			else if (mode == "neutral")
				gear.mode_ = VehicleTemplate::GM_NEUTRAL;
			else
				gear.mode_ = VehicleTemplate::GM_FORWARD;
			if (gear.mode_ != VehicleTemplate::GM_NEUTRAL)
				gear.ratio_ = StringUtil::toReal( pNode->getAttributeValueAs<String>("ratio") );
			else
				gear.ratio_ = 0.;

			engineTpl.gears_.push_back( gear );
		}
	}
	//------------------------------------------------------
	void VehicleSerializer::readAxles( const SharedPtr<dom::INode> & pNodes )
	{
		COUTLN( "readAxles()" );
		YAKE_ASSERT( pNodes );
		const dom::NodeList & nodes = pNodes->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readAxle( (*it) );
		}
	}
	//------------------------------------------------------
	void VehicleSerializer::readAxle( const SharedPtr<dom::INode> & pNode )
	{
		String id = varGet<String>(pNode->getAttributeValue("id"));
		String tag = varGet<String>(pNode->getAttributeValue("tag"));
		String engineId = varGet<String>(pNode->getAttributeValue("engine"));
		COUTLN( "readAxle() [id=" << id << " tag=" << tag << " engine=" << engineId << "]" );
		YAKE_ASSERT( pNode );

		app::model::vehicle::VehicleTemplate::AxleTemplate axleTpl;
		axleTpl.efficiency_ = 0.75;
		axleTpl.idxEngine_ = StringUtil::toInt32(engineId);

		YAKE_ASSERT( mpTpl );
		mpTpl->addAxle( axleTpl );
	}
	//------------------------------------------------------
	void VehicleSerializer::readWheels( const SharedPtr<dom::INode> & pNodes )
	{
		COUTLN( "readWheels()" );
		YAKE_ASSERT( pNodes );
		const dom::NodeList & nodes = pNodes->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readWheel( (*it) );
		}
	}
	//------------------------------------------------------
	void VehicleSerializer::readWheel( const SharedPtr<dom::INode> & pNode )
	{
		String id = varGet<String>(pNode->getAttributeValue("id"));
		String tag = varGet<String>(pNode->getAttributeValue("tag"));
		String axleId = varGet<String>(pNode->getAttributeValue("axle"));
		COUTLN( "readWheel() [id=" << id << " tag=" << tag << " axle=" << axleId << "]" );
		YAKE_ASSERT( pNode );

		app::model::vehicle::VehicleTemplate::WheelTemplate wheelTpl;

		// physical representation of the wheel
		wheelTpl.radius_ = StringUtil::toReal( varGet<String>(pNode->getAttributeValue("radius")) );
		wheelTpl.mass_ = StringUtil::toReal( varGet<String>(pNode->getAttributeValue("mass")) );
		Vector3 position(0,0,0);
		SharedPtr<dom::INode> pPosNode = pNode->getNodeByName("position");
		if (pPosNode.get())
		{
			readPosition( pPosNode, position );
		}

		String gfxName;
		SharedPtr<dom::INode> pGfxNode = pNode->getNodeByName("gfx");
		if (pGfxNode.get())
		{
			gfxName = varGet<String>(pGfxNode->getAttributeValue("name"));
			COUTLN( "  gfx = " << gfxName );
		}

		wheelTpl.position_ = position;
		wheelTpl.bMassIsRelativeToChassisMass_ = 
					(varGet<String>(pGfxNode->getAttributeValue("name")) == "relative");
		wheelTpl.gfxDescriptor_ = gfxName;
		wheelTpl.gfxDescriptorType_ = "scene";
		wheelTpl.idxAxle_ = StringUtil::toSizeT( axleId );
		//wheelTpl.idxSteeringGroup_ = StringUtil::toInt32( steerId );
		wheelTpl.idxSteeringGroup_ = -1;

		YAKE_ASSERT( mpTpl );
		mpTpl->addWheel( wheelTpl );
	}
	//------------------------------------------------------
	void VehicleSerializer::readChassis( const SharedPtr<dom::INode> & pNode )
	{
		COUTLN( "readChassis()" );
		YAKE_ASSERT( pNode );

		app::model::vehicle::VehicleTemplate::ChassisTemplate chassisTpl;
		chassisTpl.mass_ = 1.5;

		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = varGet<String>((*it)->getValue("name"));
			childNodeName = ::yake::base::StringUtil::toLowerCase(childNodeName);
			COUTLN( "   (child: " <<  childNodeName << ")" );

			const SharedPtr<dom::INode> & pChild = (*it);
			if (childNodeName == "physics")
			{
				chassisTpl.mass_ = StringUtil::toReal( varGet<String>( pChild->getAttributeValue("mass") ) );
			}
		}

		String gfxName;
		SharedPtr<dom::INode> pGfxNode = pNode->getNodeByName("gfx");
		if (pGfxNode.get())
		{
			gfxName = varGet<String>(pGfxNode->getAttributeValue("name"));
		}

		chassisTpl.gfxDescriptor_ = gfxName;
		//chassisTpl.position_ = 

		YAKE_ASSERT( mpTpl );
		mpTpl->chassisTpl_ = chassisTpl;
	}
/*
	//------------------------------------------------------
	void VehicleSerializer::readEntity( const SharedPtr<dom::INode> & pNode )
	{
		COUTLN( "readEntity() [name=" << varGet<String>(pNode->getAttributeValue("name")) << "]" );
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pParentSN );
		String name = varGet<String>(pNode->getAttributeValue("name"));
		String meshName = varGet<String>(pNode->getAttributeValue("meshfile"));
		graphics::IEntity* pEnt = mGWorld->createEntity( meshName );
		YAKE_ASSERT( pEnt );
		pEnt->setCastsShadow( false );
		pParentSN->attachEntity( pEnt );
	}
*/
	//------------------------------------------------------
	void VehicleSerializer::readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position )
	{
		YAKE_ASSERT( pNode );
		position.x = atof( pNode->getAttributeValueAs<String>("x").c_str() );
		position.y = atof( pNode->getAttributeValueAs<String>("y").c_str() );
		position.z = atof( pNode->getAttributeValueAs<String>("z").c_str() );
	}

	//------------------------------------------------------
	void VehicleSerializer::readOrientation( const SharedPtr<dom::INode> & pNode, Quaternion & orientation )
	{
		YAKE_ASSERT( pNode );
		if (pNode->getAttributeValueAs<String>("qx") != "")
		{
			orientation.x = atof( pNode->getAttributeValueAs<String>("qx").c_str() );
			orientation.y = atof( pNode->getAttributeValueAs<String>("qy").c_str() );
			orientation.z = atof( pNode->getAttributeValueAs<String>("qz").c_str() );
			orientation.w = atof( pNode->getAttributeValueAs<String>("qw").c_str() );
		}
		if (pNode->getAttributeValueAs<String>("axisx") != "")
		{
			Vector3 axis;
			axis.x = atof( pNode->getAttributeValueAs<String>("axisx").c_str() );
			axis.y = atof( pNode->getAttributeValueAs<String>("axisy").c_str() );
			axis.z = atof( pNode->getAttributeValueAs<String>("axisz").c_str() );
			orientation.FromAxes( &axis );
		}
		if (pNode->getAttributeValueAs<String>("anglex") != "")
		{
			Vector3 axis;
			axis.x = atof( pNode->getAttributeValueAs<String>("anglex").c_str() );
			axis.y = atof( pNode->getAttributeValueAs<String>("anglex").c_str() );
			axis.z = atof( pNode->getAttributeValueAs<String>("anglex").c_str() );
			real angle = atof( pNode->getAttributeValueAs<String>("angle").c_str() );;
			orientation.FromAngleAxis( angle, axis );
		}
	}

} // dotscene
} // serializer
} // data
} // yake

