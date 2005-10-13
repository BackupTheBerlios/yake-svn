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
#include <yapp/vehicle/yakeDotVehicle.h>

namespace yake {
namespace vehicle {

	using namespace data::dom;

	//-----------------------------------------------------
	// Class: DotVehicleParser
	//-----------------------------------------------------
	DotVehicleParser::DotVehicleParser() : mpCurrVehTpl(0)
	{
	}
	bool DotVehicleParser::parse( const String& fn )
	{
		data::dom::xml::XmlSerializer xmlSer;
		xmlSer.parse( fn, false );
		if (!xmlSer.getDocumentNode().get())
			return false;

		return parse( *xmlSer.getDocumentNode() );
	}
	bool DotVehicleParser::parse( const data::dom::INode& rootNode )
	{
		NodeList nodes = rootNode.getNodes();
		ConstVectorIterator< NodeList > itN( nodes );
		while (itN.hasMoreElements())
		{
			INode* pN = itN.getNext().get();
			const String name = pN->getName();
			if (name == "vehicleTemplate")
			{
				parseVehicleTpl( *pN );
			}
		}
		YAKE_SAFE_DELETE( mpCurrVehTpl );
		return true;
	}
	VehicleTemplate* DotVehicleParser::detachCurrentVehicleTpl()
	{
		VehicleTemplate* ret = mpCurrVehTpl;
		mpCurrVehTpl = 0;
		return ret;
	}
	void DotVehicleParser::parseVehicleTpl( const data::dom::INode& n )
	{
		YAKE_SAFE_DELETE( mpCurrVehTpl );
		mpCurrVehTpl = new VehicleTemplate();

		NodeList nodes = n.getNodes();
		ConstVectorIterator< NodeList > itN( nodes );
		while (itN.hasMoreElements())
		{
			INode* pN = itN.getNext().get();
			const String name = pN->getName();
			if (name == "mountPoint")
				parseMountPoint( *pN );
			else if (name == "engine")
				parseEngine( *pN );
			else if (name == "chassis")
				parseChassis( *pN );
			else if (name == "steeringGroup")
				parseSteeringGroup( *pN );
			else if (name == "wheel")
				parseWheel( *pN );
		}

		const String tplId = n.getAttributeValueAs<String>("name");

		mSigOnVehicleTpl( *this, tplId );
		YAKE_SAFE_DELETE( mpCurrVehTpl );
	}
	void DotVehicleParser::parseChassis( const data::dom::INode& n )
	{
		SharedPtr<data::dom::INode> pN = n.getNodeByName("body");
		if (pN.get())
		{
			mpCurrVehTpl->mChassis.mMass = StringUtil::parseReal( pN->getAttributeValueAs<String>("totalMass") );
		}
		pN = n.getNodeByName("shapeSet");
		if (pN.get())
		{
			NodeList shapeNodes = pN->getNodes();
			ConstVectorIterator< NodeList > itShapeN( shapeNodes );
			while (itShapeN.hasMoreElements())
			{
				const INode* pShapeN = itShapeN.getNext().get();
				const String name = pShapeN->getName();
				if (name == "box")
					parseShapeBox( *pShapeN );
				else if (name == "sphere")
					parseShapeSphere( *pShapeN );
				else
				{
					YAKE_ASSERT( 0 && "unhandled shape type" )( name );
				}
			}
		}
	}
	void DotVehicleParser::parseShapeBox( const data::dom::INode& n )
	{
		Vector3 dim;
		dim.x = StringUtil::parseReal( n.getAttributeValueAs<String>("x") );
		dim.y = StringUtil::parseReal( n.getAttributeValueAs<String>("y") );
		dim.z = StringUtil::parseReal( n.getAttributeValueAs<String>("z") );
		Vector3 pos;
		if (n.getNodeByName("position"))
			parsePosition( *n.getNodeByName("position"), pos );
		mpCurrVehTpl->mChassis.mChassisShapes.push_back(
			new physics::IShape::BoxDesc( dim, 0, pos ) );
	}
	void DotVehicleParser::parseShapeSphere( const data::dom::INode& n )
	{
		real radius = StringUtil::parseReal( n.getAttributeValueAs<String>("radius") );
		Vector3 pos;
		if (n.getNodeByName("position"))
			parsePosition( *n.getNodeByName("position"), pos );
		mpCurrVehTpl->mChassis.mChassisShapes.push_back(
			new physics::IShape::SphereDesc( radius, 0, pos ) );
	}
	void DotVehicleParser::parseEngine( const data::dom::INode& n )
	{
		String id = n.getAttributeValueAs<String>("name");
		YAKE_ASSERT( !id.empty() );

		String type = n.getAttributeValueAs<String>("type");
		YAKE_ASSERT( !type.empty() );

		if (type == "MountedThruster")
		{
			String mtPt = n.getAttributeValueAs<String>("mountPoint");
			YAKE_ASSERT( !mtPt.empty() );

			SharedPtr<data::dom::INode> pForcesN = n.getNodeByName("forces");
			YAKE_ASSERT( pForcesN.get() );

			mpCurrVehTpl->mEngines[ id ] = new VehicleTemplate::ThrusterTpl(
				StringUtil::parseReal( pForcesN->getAttributeValueAs<String>("min") ),
				StringUtil::parseReal( pForcesN->getAttributeValueAs<String>("max") ),
				mtPt );
		}
		else
		{
			YAKE_ASSERT( 1==0 && "Unhandled engine type!" );
		}
	}
	void DotVehicleParser::parseMountPoint( const data::dom::INode& n, VehicleTemplate::MountPointTpl* parentMtPt /*= 0*/ )
	{
		YAKE_ASSERT( mpCurrVehTpl );

		const String thisId = n.getAttributeValueAs<String>("name");
		YAKE_ASSERT( !thisId.empty() );

		Vector3 position;
		SharedPtr<INode> pNode = n.getNodeByName("position");
		if (pNode.get())
			parsePosition( *pNode, position );

		Quaternion orientation;
		pNode = n.getNodeByName("orientation");
		if (pNode.get())
			parseOrientation( *pNode, orientation );

		Vector3 direction;
		pNode = n.getNodeByName("direction");
		if (pNode.get())
			parseDirection( *pNode, direction );

		VehicleTemplate::MountPointTpl* thisMtPt = 0;

		if (parentMtPt)
		{
			parentMtPt->mChildren[ thisId ] = VehicleTemplate::MountPointTpl(position);
			thisMtPt = &(parentMtPt->mChildren[ thisId ]);
		}
		else 
		{
			mpCurrVehTpl->mMountPoints[ thisId ] = VehicleTemplate::MountPointTpl(position);
			thisMtPt = &(mpCurrVehTpl->mMountPoints[ thisId ]);
		}

		thisMtPt->mDirection = direction.normalisedCopy();
		thisMtPt->mOrientation = orientation;
		thisMtPt->mUseDirection = direction.length() > 0.001;

		// child mount points

		NodeList nodes = n.getNodes();
		ConstVectorIterator< NodeList > itN( nodes );
		while (itN.hasMoreElements())
		{
			INode* pN = itN.getNext().get();
			const String name = pN->getName();
			if (name == "mountPoint")
			{
				parseMountPoint( *pN, thisMtPt );
			}
		}
	}
	void DotVehicleParser::parseSteeringGroup( const data::dom::INode& n )
	{
		const uint32 idx = uint32( StringUtil::toSizeT( n.getAttributeValueAs<String>("index") ) );
		mpCurrVehTpl->mSteeringGroups = std::max( idx, mpCurrVehTpl->mSteeringGroups );
		if (mpCurrVehTpl->mSteeringGroups == 0)
			mpCurrVehTpl->mSteeringGroups = 1;
	}
	void DotVehicleParser::parseWheel( const data::dom::INode& n )
	{
		const String name = n.getAttributeValueAs<String>("name");
		YAKE_ASSERT( !name.empty() );
		if (name.empty())
			return;
		YAKE_ASSERT( mpCurrVehTpl->mWheels.find( name ) == mpCurrVehTpl->mWheels.end() )( name ).debug("duplicate wheel name. overriding values.");

		Vector3 position;
		SharedPtr<data::dom::INode> pN = n.getNodeByName("position");
		if (pN.get())
			parsePosition( *pN, position );

		Quaternion rotation( Quaternion::kIdentity );
		pN = n.getNodeByName("orientation");
		if (pN.get())
			parseOrientation( *pN, rotation );

		const real radius = StringUtil::parseReal( n.getAttributeValueAs<String>("radius") );
		uint32 sg = SG_NO_STEERING_GROUP;
		if (!n.getAttributeValueAs<String>("steeringGroup").empty())
			sg = uint32( StringUtil::toSizeT( n.getAttributeValueAs<String>("steeringGroup") ) );
		const real mass = StringUtil::parseReal( n.getAttributeValueAs<String>("mass") );
		const bool relToChassis = StringUtil::parseBool( n.getAttributeValueAs<String>("massIsRelativeToChassis") );
		const real suspSpring = StringUtil::parseReal( n.getAttributeValueAs<String>("suspensionSpring") );
		const real suspDamping = StringUtil::parseReal( n.getAttributeValueAs<String>("suspensionDamping") );

		mpCurrVehTpl->mWheels[ name ] = VehicleTemplate::WheelTpl(
											position,
											rotation,
											radius,
											mass,
											relToChassis,
											sg,
											suspSpring,
											suspDamping );


	}
	void DotVehicleParser::parsePosition( const data::dom::INode& n, Vector3& ret )
	{
		ret.x = StringUtil::parseReal( n.getAttributeValueAs<String>("x") );
		ret.y = StringUtil::parseReal( n.getAttributeValueAs<String>("y") );
		ret.z = StringUtil::parseReal( n.getAttributeValueAs<String>("z") );
	}
	void DotVehicleParser::parseDirection( const data::dom::INode& n, Vector3& ret )
	{
		ret.x = StringUtil::parseReal( n.getAttributeValueAs<String>("x") );
		ret.y = StringUtil::parseReal( n.getAttributeValueAs<String>("y") );
		ret.z = StringUtil::parseReal( n.getAttributeValueAs<String>("z") );
	}
	void DotVehicleParser::parseOrientation( const data::dom::INode& n, Quaternion& ret )
	{
		if ( n.getAttributeValueAs<String>("qx") != "" )
		{
			ret.x = StringUtil::parseReal( n.getAttributeValueAs<String>("qx") );
			ret.y = StringUtil::parseReal( n.getAttributeValueAs<String>("qy") );
			ret.z = StringUtil::parseReal( n.getAttributeValueAs<String>("qz") );
			ret.w = StringUtil::parseReal( n.getAttributeValueAs<String>("qw") );
		}
		else if ( n.getAttributeValueAs<String>("axisX") != "" )
		{
			Vector3 axis;
			axis.x = StringUtil::parseReal( n.getAttributeValueAs<String>("axisX") );
			axis.y = StringUtil::parseReal( n.getAttributeValueAs<String>("axisY") );
			axis.z = StringUtil::parseReal( n.getAttributeValueAs<String>("axisZ") );
			ret.FromAxes( &axis );
		}
		else if ( n.getAttributeValueAs<String>("angleX") != "" )
		{
			Vector3 axis;
			axis.x = StringUtil::parseReal( n.getAttributeValueAs<String>("angleX") );
			axis.y = StringUtil::parseReal( n.getAttributeValueAs<String>("angleY") );
			axis.z = StringUtil::parseReal( n.getAttributeValueAs<String>("angleZ") );
			real angle = StringUtil::parseReal( n.getAttributeValueAs<String>("angle") );
			ret.FromAngleAxis( angle, axis );
		}
	}

} // namespace vehicle
} // namespace yake
