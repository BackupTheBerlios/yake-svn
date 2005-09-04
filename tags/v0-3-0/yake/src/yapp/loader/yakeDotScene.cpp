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
#include <yapp/loader/yakeDotScene.h>

namespace yake {
namespace data {
namespace parser {
namespace dotscene {

	String DotSceneParser::ROOT_NODE_NAME = "root_node";
	
	//------------------------------------------------------
	void DotSceneParser::reset()
	{
		//FIXME state is more that just doc node
		mDocNode.reset();
	}

	//------------------------------------------------------
	bool DotSceneParser::load( const SharedPtr<dom::INode>& docNode )
	{
		YAKE_DECLARE_FUNCTION( load );
		
		YAKE_LOG( "load()" );
		
		YAKE_ASSERT( docNode.get() );
		
		if (!docNode.get())
			return false;
		
		mDocNode = docNode;
		
		YAKE_LOG( "parsing scene..." );
		
		readScene( mDocNode );

		return true;
	}

	//------------------------------------------------------
	void DotSceneParser::readScene( const SharedPtr<dom::INode>& pNode )
	{
		YAKE_DECLARE_FUNCTION( readScene );
		
		//const String name = pNode->getAttributeValueAs<String>( "name" );
		//YAKE_LOG( "readScene() [" + name + "]" );
		
		YAKE_ASSERT( pNode );
		
		SharedPtr<dom::INode> pNodes = pNode->getNodeByName("nodes");
		YAKE_LOG( "scene: found nodes = " + String( pNodes.get() ? "yes" : "no" ) );
		
		if ( pNodes.get() )
			readNodes( pNodes );
	}

	//------------------------------------------------------
	void DotSceneParser::readNodes( const SharedPtr<dom::INode>& pNodes )
	{
		YAKE_DECLARE_FUNCTION( readNodes );
		
		YAKE_LOG( "readNodes()" );
		YAKE_ASSERT( pNodes );
		
		const dom::NodeList& nodes = pNodes->getNodes();
		for( dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it )
		{
			readNode( (*it), ROOT_NODE_NAME );
		}
	}

	//------------------------------------------------------
	void DotSceneParser::readNode( const SharedPtr<dom::INode>& pNode, String parentNodeName )
	{
		YAKE_DECLARE_FUNCTION( readNode );
		
		String nodeName = pNode->getAttributeValueAs<String>( "name" );
		
		YAKE_LOG( "readNode() [name=" + nodeName + "]" );
		YAKE_ASSERT( pNode );
		
		if ( nodeName.empty() )
			nodeName = uniqueName::create( nodeName );
		
		NodeDesc currentSceneNode;
		
		currentSceneNode.name = nodeName;
		currentSceneNode.parentNodeName = parentNodeName;
		
		// Implementing top-bottom parsing. So we have to read full info about the top-most node 
		// and then descend to lower levels "root---->children". 
		// That's why XML nodes for scene nodes should be saved at first. XML nodes for all entities, cameras,
		// lights, etc. of the current scene node should also be saved and be parsed after the current node.
		// Saved XML nodes for child scene nodes are parsed in the last turn.
		dom::NodeList childNodes;
		dom::NodeList attachedObjects; // for entities, cameras, lights, etc.
		
		const dom::NodeList& nodes = pNode->getNodes();
		for ( dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it )
		{
			String tag = StringUtil::toLowerCase((*it)->getName());

			const SharedPtr<dom::INode> & pChild = (*it);
			if (tag == "position")
			{
				readPosition( pChild, currentSceneNode.transform.position );
			}
			else if (tag == "rotation")
			{
				readRotation( pChild, currentSceneNode.transform.rotation );
			}
			else if (tag == "scale")
			{
				readScale( pChild, currentSceneNode.transform.scale );
			}
			else if (tag == "node" )
				childNodes.push_back( *it );
			else if (	tag == "light" ||
						tag == "entity" ||
						tag == "camera" )
				attachedObjects.push_back( *it );
		}
		
		// Now firing signal as new node was parsed
		mSigNode( currentSceneNode );
		// And placing it to the map
		mSNDescriptions[ nodeName ] = currentSceneNode;
		
		// Now it's time to read attached objects
		for ( dom::NodeList::const_iterator it = attachedObjects.begin(); it != attachedObjects.end(); ++it )
		{
			String tag = StringUtil::toLowerCase((*it)->getName());

			const SharedPtr<dom::INode>& pChild = (*it);
			if ( tag == "entity" )
			{
				readEntity( pChild, nodeName );
			}
			else if ( tag == "camera" )
			{
				readCamera( pChild, nodeName );
			}
			else if ( tag == "light" )
			{
				readLight( pChild, nodeName );
			}
		}
		
		// Next, reading child scene nodes
		// descending the node tree...
		for ( dom::NodeList::const_iterator it = childNodes.begin(); it != childNodes.end(); ++it )
		{
				readNode( *it, nodeName );
		}
	}

	//------------------------------------------------------
	void DotSceneParser::readEntity( const SharedPtr<dom::INode>& pNode, String parentNodeName )
	{
		YAKE_DECLARE_FUNCTION( readEntity );
		
		YAKE_ASSERT( pNode );
		
		EntityDesc desc;
		
		String name = (pNode->getAttributeValueAs<String>("name"));
		
		desc.name = name;
		desc.parentNodeName = parentNodeName;
		
		YAKE_LOG( "readEntity() [name = " + name + "]" );
		
		desc.meshFile = pNode->getAttributeValueAs<String>( "meshFile" );
		
		String castsShadows = StringUtil::toLowerCase(pNode->getAttributeValueAs<String>("castsShadow"));
		desc.castsShadows = castsShadows == "yes";

		// Entity description ready. Fire!
		mSigEntity( desc );
		
		// Storing...
		mEntityDescriptions[ name ] = desc;
	}
	
	//------------------------------------------------------
	void DotSceneParser::readVector( const SharedPtr<dom::INode>& pNode, Vector3& rVec )
	{
		YAKE_ASSERT( pNode );
		rVec.x = StringUtil::parseReal( pNode->getAttributeValueAs<String>("x") );
		rVec.y = StringUtil::parseReal( pNode->getAttributeValueAs<String>("y") );
		rVec.z = StringUtil::parseReal( pNode->getAttributeValueAs<String>("z") );
	}

	//------------------------------------------------------
	void DotSceneParser::readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position )
	{
		readVector( pNode, position );
	}

	//------------------------------------------------------
	void DotSceneParser::readScale( const SharedPtr<dom::INode>& pNode, Vector3& rScale )
	{
		readVector( pNode, rScale );
	}
	
	//------------------------------------------------------
	void DotSceneParser::readRotation( const SharedPtr<dom::INode>& pNode, Quaternion & rotation )
	{
		YAKE_ASSERT( pNode );
		if (pNode->getAttributeValueAs<String>("qx") != "")
		{
			rotation.x = StringUtil::parseReal( pNode->getAttributeValueAs<String>("qx") );
			rotation.y = StringUtil::parseReal( pNode->getAttributeValueAs<String>("qy") );
			rotation.z = StringUtil::parseReal( pNode->getAttributeValueAs<String>("qz") );
			rotation.w = StringUtil::parseReal( pNode->getAttributeValueAs<String>("qw") );
		}
		if (pNode->getAttributeValueAs<String>("axisX") != "")
		{
			Vector3 axis;
			axis.x = StringUtil::parseReal( pNode->getAttributeValueAs<String>("axisX") );
			axis.y = StringUtil::parseReal( pNode->getAttributeValueAs<String>("axisY") );
			axis.z = StringUtil::parseReal( pNode->getAttributeValueAs<String>("axisZ") );
			rotation.FromAxes( &axis );
		}
		if (pNode->getAttributeValueAs<String>("angleX") != "")
		{
			Vector3 axis;
			axis.x = StringUtil::parseReal( pNode->getAttributeValueAs<String>("angleX") );
			axis.y = StringUtil::parseReal( pNode->getAttributeValueAs<String>("angleY") );
			axis.z = StringUtil::parseReal( pNode->getAttributeValueAs<String>("angleZ") );
			real angle = StringUtil::parseReal( pNode->getAttributeValueAs<String>("angle") );
			rotation.FromAngleAxis( angle, axis );
		}
	}
	//------------------------------------------------------
	void DotSceneParser::readColour( const SharedPtr<dom::INode>& pNode, Color& colour )
	{
		YAKE_DECLARE_FUNCTION( readColour );
		
		YAKE_ASSERT( pNode );
		
		String r = pNode->getAttributeValueAs<String>("r");
		String g = pNode->getAttributeValueAs<String>("g");
		String b = pNode->getAttributeValueAs<String>("b");
		
		YAKE_LOG( "readColour: r=" + r + ", g=" + g + ", b=" + b );
		
		colour.r = StringUtil::parseReal( r );
		colour.g = StringUtil::parseReal( g );
		colour.b = StringUtil::parseReal( b );
	}

	//------------------------------------------------------
	void DotSceneParser::readLightRange( const SharedPtr<dom::INode>& pNode, LightDesc& desc )
	{
		YAKE_ASSERT( pNode );
		
		desc.range.inner = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "inner" ) );
		desc.range.outer = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "outer" ) );
		desc.range.falloff = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "falloff" ) );
	}

	//------------------------------------------------------
	void DotSceneParser::readLightAttenuation( const SharedPtr<dom::INode>& pNode, LightDesc& desc )
	{
		YAKE_ASSERT( pNode );
		
		desc.attenuation.range = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "range" ) );
		desc.attenuation.constant = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "constant" ) );
		desc.attenuation.linear = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "linear" ) );
		desc.attenuation.quadratic = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "quadratic" ) );
	}
	
	//------------------------------------------------------
	void DotSceneParser::readLight( const SharedPtr<dom::INode>& pNode, String parentNodeName )
	{
		YAKE_DECLARE_FUNCTION( readLight );
		
		String name = pNode->getAttributeValueAs<String>( "name" );

		LightDesc desc;
		
		desc.name = name;
		desc.parentNodeName = parentNodeName;
		
		YAKE_LOG( "readLight() [name=" + name + "]" );
		YAKE_ASSERT( pNode );
		
		String lightType = pNode->getAttributeValueAs<String>( "type" );
				
		desc.type = graphics::ILight::LT_POINT;
		
		if ( lightType == "spot" )
			desc.type = graphics::ILight::LT_SPOT;
		else if ( lightType == "directional" )
			desc.type = graphics::ILight::LT_DIRECTIONAL;
		
		const dom::NodeList& nodes = pNode->getNodes();
		for ( dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it )
		{
			String childNodeName = (*it)->getName();
			
			YAKE_LOG( "node child: " + StringUtil::toLowerCase( childNodeName ) );

			const SharedPtr<dom::INode>& pChild = *it;
			
			if ( childNodeName == "normal" )
			{
				readVector( pChild, desc.normal );
			}
			else if ( childNodeName == "colourDiffuse" )
			{
				readColour( pChild, desc.diffuseColor );
			}
			else if ( childNodeName == "colourSpecular" )
			{
				readColour( pChild, desc.specularColor );
			}
			else if ( childNodeName == "lightRange" )
			{
				readLightRange( pChild, desc );
			}
			else if ( childNodeName == "lightAttenuation" )
			{
				readLightAttenuation( pChild, desc );
			}
		}
		
		// Light description ready. Fire!
		mSigLight( desc );
		
		// Store
		mLightDescriptions[ name ] = desc;
	}
	
	//------------------------------------------------------
	void DotSceneParser::readCameraClipping( const SharedPtr<dom::INode>& pNode, CameraDesc& desc )
	{
		YAKE_ASSERT( pNode );
		
		desc.clipping.near = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "near" ) );
		desc.clipping.far = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "far" ) );
	}
	
	//------------------------------------------------------
	void DotSceneParser::readCamera( const SharedPtr<dom::INode>& pNode, String parentNodeName )
	{
		YAKE_DECLARE_FUNCTION( readCamera );
		
		String name = pNode->getAttributeValueAs<String>( "name" );

		CameraDesc desc;
		
		desc.name = name;
		desc.parentNodeName = parentNodeName;
		
		YAKE_LOG( "readCamera() [name=" + name + "]" );
		YAKE_ASSERT( pNode );
		
		String projectionType = pNode->getAttributeValueAs<String>( "projectionType" );
				
		desc.projectionType = graphics::ICamera::PT_PERSPECTIVE;
		
		if ( projectionType == "orthographic" )
			desc.projectionType = graphics::ICamera::PT_ORTHOGRAPHIC;
		
		desc.fov = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "fov" ) );
		
		const dom::NodeList& nodes = pNode->getNodes();
		for ( dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it )
		{
			String childNodeName = (*it)->getName();
			
			YAKE_LOG( "node child: " + StringUtil::toLowerCase( childNodeName ) );

			const SharedPtr<dom::INode>& pChild = *it;
			
			if ( childNodeName == "normal" )
			{
				readVector( pChild, desc.normal );
			}
			else if ( childNodeName == "clipping" )
			{
				readCameraClipping( pChild, desc );
			}
			else if ( childNodeName == "trackTarget" )
			{
				desc.trackTargetName = pChild->getAttributeValueAs<String>( "nodeName" );
			}
		}
		
		// Camera description ready. Fire!
		mSigCamera( desc );
		
		// Store
		mCameraDescriptions[ name ] = desc;
	}

} // dotscene
} // parser
} // data
} // yake
