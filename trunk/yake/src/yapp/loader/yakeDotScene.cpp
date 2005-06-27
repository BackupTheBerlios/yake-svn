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
#include <yapp/loader/yakeDotScene.h>

namespace yake {
namespace data {
namespace serializer {
namespace dotscene {


	//------------------------------------------------------
	SceneNodeList DotSceneSerializer::getRootLevelSceneNodes() const
	{
		return mRootNodes;
	}
	//------------------------------------------------------
	void DotSceneSerializer::reset()
	{
		mDocNode.reset();
	}

	//------------------------------------------------------
	bool DotSceneSerializer::load(
							const SharedPtr<dom::INode> & docNode,
							graphics::IWorld* pGWorld)
	{
		std::cout << "load()" << std::endl;
		YAKE_ASSERT( docNode.get() );
		if (!docNode.get())
			return false;
		YAKE_ASSERT( pGWorld );
		if (!pGWorld)
			return false;
		mDocNode = docNode;
		mGWorld = pGWorld;
		//
		std::cout << "parsing scene..." << std::endl;
		readScene( mDocNode, 0 );

		//
		return true;
	}

	//------------------------------------------------------
	void DotSceneSerializer::readScene( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		const String name = pNode->getAttributeValueAs<String>( "name" );
		std::cout << "readScene() [" << name << "]" << std::endl;
		YAKE_ASSERT( pNode );
		SharedPtr<dom::INode> pNodes = pNode->getNodeByName("nodes");
		std::cout << "scene: found nodes = " << (pNodes.get() ? "yes" : "no") << std::endl;
		if (pNodes.get())
			readNodes( pNodes, pParentSN );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readNodes( const SharedPtr<dom::INode> & pNodes, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readNodes()" << std::endl;
		YAKE_ASSERT( pNodes );
		const dom::NodeList & nodes = pNodes->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readNode( (*it), pParentSN );
		}
	}

	//------------------------------------------------------
	void DotSceneSerializer::readNode( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		String nodeName = pNode->getAttributeValueAs<String>("name");
		std::cout << "readNode() [name=" << nodeName << "]" << std::endl;
		YAKE_ASSERT( pNode );
		if (nodeName.empty())
			nodeName = uniqueName::create(nodeName);
		graphics::ISceneNode* pChildSN = mGWorld->createSceneNode(nodeName);
		YAKE_ASSERT( pChildSN );
		mNodeNames.insert( std::make_pair( pChildSN, nodeName ) );
		if (pParentSN)
			pParentSN->addChildNode( pChildSN );
		else
			mRootNodes.push_back( pChildSN );
		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String tag = ::yake::StringUtil::toLowerCase((*it)->getValueAs<String>("name"));

			const SharedPtr<dom::INode> & pChild = (*it);
			if (tag == "entity")
				readEntity( pChild, pChildSN );
			else if (tag == "position")
			{
				Vector3 position;
				readPosition( pChild, position );
				pChildSN->setPosition( position );
			}
			else if (tag == "rotation")
			{
				Quaternion rotation;
				readRotation( pChild, rotation );
				pChildSN->setOrientation( rotation );
			}
			else if (tag == "scale")
			{
				Vector3 scale;
				readScale( pChild, scale );
				pChildSN->setScale( scale );
			}
			else if (tag == "node" )
				readNode( pChild, pChildSN );
			else if (tag == "light" )
				readLight( pChild, pChildSN );
		}
	}

	//------------------------------------------------------
	void DotSceneSerializer::readEntity( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readEntity() [name=" << (pNode->getAttributeValueAs<String>("name")) << "]" << std::endl;
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pParentSN );
		String name = (pNode->getAttributeValueAs<String>("name"));
		String meshName = (pNode->getAttributeValueAs<String>("meshFile"));
		String castsShadow = ::yake::StringUtil::toLowerCase(pNode->getAttributeValueAs<String>("castsShadow"));
		graphics::IEntity* pEnt = mGWorld->createEntity( meshName );
		YAKE_ASSERT( pEnt );
		pEnt->setCastsShadow( (castsShadow == "yes") );
		pParentSN->attachEntity( pEnt );
	}
	
	//------------------------------------------------------
	void DotSceneSerializer::readVector( const SharedPtr<dom::INode>& pNode, Vector3& rVec )
	{
		YAKE_ASSERT( pNode );
		rVec.x = atof( pNode->getAttributeValueAs<String>("x").c_str() );
		rVec.y = atof( pNode->getAttributeValueAs<String>("y").c_str() );
		rVec.z = atof( pNode->getAttributeValueAs<String>("z").c_str() );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position )
	{
		readVector( pNode, position );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readScale( const SharedPtr<dom::INode>& pNode, Vector3& rScale )
	{
		readVector( pNode, rScale );
	}
	
	//------------------------------------------------------
	void DotSceneSerializer::readRotation( const SharedPtr<dom::INode> & pNode, Quaternion & rotation )
	{
		YAKE_ASSERT( pNode );
		if (pNode->getAttributeValueAs<String>("qx") != "")
		{
			rotation.x = atof( pNode->getAttributeValueAs<String>("qx").c_str() );
			rotation.y = atof( pNode->getAttributeValueAs<String>("qy").c_str() );
			rotation.z = atof( pNode->getAttributeValueAs<String>("qz").c_str() );
			rotation.w = atof( pNode->getAttributeValueAs<String>("qw").c_str() );
		}
		if (pNode->getAttributeValueAs<String>("axisX") != "")
		{
			Vector3 axis;
			axis.x = atof( pNode->getAttributeValueAs<String>("axisX").c_str() );
			axis.y = atof( pNode->getAttributeValueAs<String>("axisY").c_str() );
			axis.z = atof( pNode->getAttributeValueAs<String>("axisZ").c_str() );
			rotation.FromAxes( &axis );
		}
		if (pNode->getAttributeValueAs<String>("angleX") != "")
		{
			Vector3 axis;
			axis.x = atof( pNode->getAttributeValueAs<String>("angleX").c_str() );
			axis.y = atof( pNode->getAttributeValueAs<String>("angleY").c_str() );
			axis.z = atof( pNode->getAttributeValueAs<String>("angleZ").c_str() );
			real angle = atof( pNode->getAttributeValueAs<String>("angle").c_str() );;
			rotation.FromAngleAxis( angle, axis );
		}
	}
	//------------------------------------------------------
	void DotSceneSerializer::readColour( const SharedPtr<dom::INode>& pNode, Color& colour )
	{
		YAKE_ASSERT( pNode );
		
		/// TODO we shouldn't use atof directly... Maybe some wrapper?
		colour.r = atof( pNode->getAttributeValueAs<String>("r").c_str() );
		colour.g = atof( pNode->getAttributeValueAs<String>("g").c_str() );
		colour.b = atof( pNode->getAttributeValueAs<String>("b").c_str() );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readLightRange( const SharedPtr<dom::INode>& pNode, graphics::ILight* pLight )
	{
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pLight );
		
		real innerAngle = atof( pNode->getAttributeValueAs<String>( "inner" ).c_str() );
		real outerAngle = atof( pNode->getAttributeValueAs<String>( "outer" ).c_str() );
		real falloff = atof( pNode->getAttributeValueAs<String>( "falloff" ).c_str() );
	
		pLight->setSpotlightRange( innerAngle, outerAngle, falloff );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readLightAttenuation( const SharedPtr<dom::INode>& pNode, graphics::ILight* pLight )
	{
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pLight );
		
		real range = atof( pNode->getAttributeValueAs<String>( "range" ).c_str() );
		real constant = atof( pNode->getAttributeValueAs<String>( "constant" ).c_str() );
		real linear = atof( pNode->getAttributeValueAs<String>( "linear" ).c_str() );
		real quadratic = atof( pNode->getAttributeValueAs<String>( "quadratic" ).c_str() );
		
		pLight->setAttenuation( range, constant, linear, quadratic );
	}
	
	//------------------------------------------------------
	void DotSceneSerializer::readLight( const SharedPtr<dom::INode>& pNode, graphics::ISceneNode* pParentSN )
	{
		String name = pNode->getAttributeValueAs<String>( "name" );

		std::cout << "readLight() [name=" << name << "]" << std::endl;
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pParentSN );
		
		String lightType = pNode->getAttributeValueAs<String>( "type" );
				
		graphics::ILight* pLight = mGWorld->createLight();
		YAKE_ASSERT( pLight );
		
		graphics::ILight::LightType lightTypeID = graphics::ILight::LT_POINT;
		
		if ( lightType == "spot" )
			lightTypeID = graphics::ILight::LT_SPOT;
		else if ( lightType == "directional" )
			lightTypeID = graphics::ILight::LT_DIRECTIONAL;
		
		pLight->setType( lightTypeID );
				
		pParentSN->attachLight( pLight );

		const dom::NodeList& nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = (*it)->getValueAs<String>( "name" );
			
			childNodeName = ::yake::StringUtil::toLowerCase( childNodeName );
			std::cout << "node child: " <<  childNodeName << std::endl;

			const SharedPtr<dom::INode>& pChild = *it;
			
			if (childNodeName == "normal")
			{
				Vector3 normal;
				readPosition( pChild, normal );
				pLight->setDirection( normal );
			}
			else if (childNodeName == "colourDiffuse")
			{
				Color colour_diffuse;
				readColour( pChild, colour_diffuse );
				pLight->setDiffuseColour( colour_diffuse );
			}
			else if (childNodeName == "colourSpecular")
			{
				Color colour_specular;
				readColour( pChild, colour_specular );
				pLight->setSpecularColour( colour_specular );
			}
			else if (childNodeName == "lightRange")
			{
				readLightRange( pChild, pLight );
			}
			else if (childNodeName == "lightAttenuation")
			{
				readLightAttenuation( pChild, pLight );
			}

		}
	}


} // dotscene
} // serializer
} // data
} // yake
