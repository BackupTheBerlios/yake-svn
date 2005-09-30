/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
									model::vehicle::VehicleTemplate& tpl)
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
		
		const String name = pNode->getName();
		COUTLN( "readVehicle() [" << name << "]" );
		YAKE_ASSERT( pNode );

		SharedPtr<dom::INode> pNodes = pNode->getNodeByName( "chassis" );
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
		String id = pNode->getAttributeValueAs<String>( "id" );
		String tag = pNode->getAttributeValueAs<String>( "tag" );
		COUTLN( "readEngine() [id=" << id << " tag=" << tag << "]" );
		YAKE_ASSERT( pNode );

		// create template
		model::vehicle::VehicleTemplate::CarEngineTemplate engineTpl;

		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = (*it)->getName();
			childNodeName = ::yake::StringUtil::toLowerCase(childNodeName);
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
	void VehicleSerializer::readRPM( const SharedPtr<dom::INode> & pNode, model::vehicle::VehicleTemplate::CarEngineTemplate& engineTpl )
	{
		COUTLN( "  readRPM()" );
		YAKE_ASSERT( pNode );

		engineTpl.rpmMin_ = StringUtil::toReal( pNode->getAttributeValueAs<String>( "min" ) );
		engineTpl.rpmMax_ = StringUtil::toReal( pNode->getAttributeValueAs<String>( "max" ) );
		engineTpl.rpmRedline_ = StringUtil::toReal( pNode->getAttributeValueAs<String>( "redline" ) );
	}
	//------------------------------------------------------
	void VehicleSerializer::readGearbox( const SharedPtr<dom::INode> & pNode, model::vehicle::VehicleTemplate::CarEngineTemplate& engineTpl )
	{
		using namespace model::vehicle;
		COUTLN( "  readGearbox()" );
		YAKE_ASSERT( pNode );
		const dom::NodeList & nodes = pNode->getNodes();
		ConstVectorIterator< dom::NodeList > it( nodes.begin(), nodes.end() );
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
		String id = pNode->getAttributeValueAs<String>( "id" );
		String tag = pNode->getAttributeValueAs<String>( "tag" );
		String engineId = pNode->getAttributeValueAs<String>( "engine" );
		COUTLN( "readAxle() [id=" << id << " tag=" << tag << " engine=" << engineId << "]" );
		YAKE_ASSERT( pNode );

		model::vehicle::VehicleTemplate::AxleTemplate axleTpl;
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
		String id = pNode->getAttributeValueAs<String>( "id" );
		String tag = pNode->getAttributeValueAs<String>( "tag" );
		String axleId = pNode->getAttributeValueAs<String>( "axle" );
		COUTLN( "readWheel() [id=" << id << " tag=" << tag << " axle=" << axleId << "]" );
		YAKE_ASSERT( pNode );

		model::vehicle::VehicleTemplate::WheelTemplate wheelTpl;

		// physical representation of the wheel
		wheelTpl.radius_ = StringUtil::toReal( pNode->getAttributeValueAs<String>( "radius" ) );
		wheelTpl.mass_ = StringUtil::toReal( pNode->getAttributeValueAs<String>( "mass" ) );
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
			gfxName = pGfxNode->getAttributeValueAs<String>( "name" );
			COUTLN( "  gfx = " << gfxName );
		}

		wheelTpl.position_ = position;
		wheelTpl.bMassIsRelativeToChassisMass_ = 
					( pGfxNode->getAttributeValueAs<String>( "name" ) == "relative");
		wheelTpl.gfxDescriptor_ = gfxName;
		wheelTpl.gfxDescriptorType_ = "scene";
		wheelTpl.idxAxle_ = StringUtil::toInt32( axleId );
		//wheelTpl.idxSteeringGroup_ = StringUtil::toInt32( steerId );
		wheelTpl.idxSteeringGroup_ = -1;

		SharedPtr<dom::INode> pSuspNode = pNode->getNodeByName("suspension");
		if (pSuspNode.get())
		{
			readSuspension( pSuspNode, wheelTpl.suspensionSpring_, wheelTpl.suspensionDamping_ );
		}
		else
		{
			wheelTpl.suspensionSpring_ = 30;
			wheelTpl.suspensionDamping_ = 3;
		}

		YAKE_ASSERT( mpTpl );
		mpTpl->addWheel( wheelTpl );
	}
	//------------------------------------------------------
	void VehicleSerializer::readSuspension( const SharedPtr<dom::INode> & pNode, real & spring, real & damping )
	{
		COUTLN( "readSuspension()" );
		YAKE_ASSERT( pNode );
		spring = atof( pNode->getAttributeValueAs<String>("spring").c_str() );
		damping = atof( pNode->getAttributeValueAs<String>("damping").c_str() );
	}
	//------------------------------------------------------
	void VehicleSerializer::readChassis( const SharedPtr<dom::INode> & pNode )
	{
		COUTLN( "readChassis()" );
		YAKE_ASSERT( pNode );

		model::vehicle::VehicleTemplate::ChassisTemplate chassisTpl;
		chassisTpl.mass_ = 1.5;

		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = (*it)->getName();
			childNodeName = ::yake::StringUtil::toLowerCase(childNodeName);
			COUTLN( "   (child: " <<  childNodeName << ")" );

			const SharedPtr<dom::INode> & pChild = (*it);
			if (childNodeName == "physics")
			{
				chassisTpl.mass_ = StringUtil::toReal( pChild->getAttributeValueAs<String>( "mass" ) );
			}
		}

		String gfxName;
		SharedPtr<dom::INode> pGfxNode = pNode->getNodeByName("gfx");
		if (pGfxNode.get())
		{
			gfxName = pGfxNode->getAttributeValueAs<String>( "name" );
		}

		chassisTpl.gfxDescriptor_ = gfxName;

		chassisTpl.position_ = Vector3(0,0,0);
		SharedPtr<dom::INode> pPosNode = pNode->getNodeByName("position");
		if (pPosNode.get())
			readPosition( pPosNode, chassisTpl.position_ );

		SharedPtr<dom::INode> pDimNode = pNode->getNodeByName("dimensions");
		if (pDimNode.get())
			readGeomDimensions( pDimNode, chassisTpl.dimensions_ );

		SharedPtr<dom::INode> pGeomsNode = pNode->getNodeByName("geoms");
		if (pGeomsNode.get())
			readGeoms( pGeomsNode, chassisTpl );

		YAKE_ASSERT( mpTpl );
		mpTpl->chassisTpl_ = chassisTpl;

		COUTLN(	" pos=" << chassisTpl.position_.x << ", " << chassisTpl.position_.y << ", " << chassisTpl.position_.z <<
				" dim=" << chassisTpl.dimensions_.x << ", " << chassisTpl.dimensions_.y << ", " << chassisTpl.dimensions_.z );
	}
	//------------------------------------------------------
	void VehicleSerializer::readGeoms( const SharedPtr<dom::INode> & pNodes, model::vehicle::VehicleTemplate::ChassisTemplate & chassisTpl )
	{
		YAKE_ASSERT( pNodes );
		COUTLN("readGeoms() (children=" << pNodes->getNodes().size() << ")");
		const dom::NodeList & nodes = pNodes->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readGeom( (*it), chassisTpl );
			const model::vehicle::VehicleTemplate::ShapeTemplate & geomTpl = chassisTpl.collisionShapes_.back();
			COUTLN(	"  geom type=" << geomTpl.type_ <<
					" pos=" << geomTpl.position_.x << ", " << geomTpl.position_.y << ", " << geomTpl.position_.z );
//					" dim=" << geomTpl.dimensions_.x << ", " << geomTpl.dimensions_.y << ", " << geomTpl.dimensions_.z );
		}
	}
	//------------------------------------------------------
	void VehicleSerializer::readGeom( const SharedPtr<dom::INode> & pNode, model::vehicle::VehicleTemplate::ChassisTemplate & chassisTpl )
	{
		YAKE_ASSERT( pNode );
		COUTLN("readGeom() == " << pNode->getAttributeValueAs<String>("type"));
		model::vehicle::VehicleTemplate::ShapeTemplate geomTpl;

		// dimensions
		SharedPtr<dom::INode> pTmpNode = pNode->getNodeByName("dimensions");
		//if (pTmpNode.get())
		//{
		//	readGeomDimensions( pTmpNode, geomTpl.dimensions_ );
		//	String mode = StringUtil::toLowerCase( pTmpNode->getAttributeValueAs<String>("mode") );
		//	if (mode == "relative")
		//	{
		//		geomTpl.dimensions_.x *= chassisTpl.dimensions_.x;
		//		geomTpl.dimensions_.y *= chassisTpl.dimensions_.y;
		//		geomTpl.dimensions_.z *= chassisTpl.dimensions_.z;
		//	}
		//}
		//else
		//	geomTpl.dimensions_ = Vector3(0.1,0.1,0.1);

		geomTpl.position_ = Vector3(0,0,0);
		pTmpNode = pNode->getNodeByName("position");
		if (pTmpNode.get())
			readPosition( pTmpNode, geomTpl.position_ );

		String geomType = StringUtil::toLowerCase( pNode->getAttributeValueAs<String>("type") );
		if (geomType == "sphere")
		{
			geomTpl.type_ = physics::ST_SPHERE;
			real radius = atof( pNode->getAttributeValueAs<String>("radius").c_str() );
			geomTpl.pDesc_.reset( new physics::IShape::SphereDesc(radius) );
		}
		else if (geomType == "box")
		{
			geomTpl.type_ = physics::ST_BOX;
			Vector3 dimensions(1,1,1);
			dimensions.x = atof( pNode->getAttributeValueAs<String>("boxx").c_str() );
			dimensions.y = atof( pNode->getAttributeValueAs<String>("boxy").c_str() );
			dimensions.z = atof( pNode->getAttributeValueAs<String>("boxz").c_str() );
			geomTpl.pDesc_.reset( new physics::IShape::BoxDesc(dimensions) );
		}

		COUTLN("  -> type= " << geomType );

		chassisTpl.collisionShapes_.push_back( geomTpl );
	}
	//------------------------------------------------------
	void VehicleSerializer::readGeomDimensions( const SharedPtr<dom::INode> & pNode, Vector3 & dimensions )
	{
		COUTLN("  readGeomDimensions()");
		YAKE_ASSERT( pNode );
		dimensions.x = atof( pNode->getAttributeValueAs<String>("x").c_str() );
		dimensions.y = atof( pNode->getAttributeValueAs<String>("y").c_str() );
		dimensions.z = atof( pNode->getAttributeValueAs<String>("z").c_str() );
	}
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
