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
			{
				parseMountPoint( *pN );
			} else if (name == "engine")
			{
				parseEngine( *pN );
			}
		}

		const String tplId = n.getAttributeValueAs<String>("name");

		mSigOnVehicleTpl( *this, tplId );
		YAKE_SAFE_DELETE( mpCurrVehTpl );
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
