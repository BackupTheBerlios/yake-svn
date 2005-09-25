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
namespace model {

	//-----------------------------------------------------
	Graphical::~Graphical()
	{
		VectorIterator<NodeList> it(mNodes);
		while (it.hasMoreElements())
		{
			SceneNodeEntry e = it.getNext();
			if (e.bOwned)
				delete e.pSN;
		}
		mNodes.clear();
	}
	//-----------------------------------------------------
	void Graphical::addSceneNode( graphics::ISceneNode* pSceneNode, bool bTransferOwnership )
	{
		YAKE_ASSERT( pSceneNode );
		if (!pSceneNode)
			return;
		
		SceneNodeEntry e;
		e.bOwned = bTransferOwnership;
		e.pSN = pSceneNode;
		mNodes.push_back( e );
	}
	//-----------------------------------------------------
	Graphical::SceneNodeList Graphical::getSceneNodes(bool bRecursive /*= false*/) const
	{
		SceneNodeList nodes;
		ConstVectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext().pSN;
			nodes.push_back( pSN );
			if (bRecursive)
				pSN->getChildren( nodes, true );
		}
		return nodes;
	}
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getSceneNode(const String & rName, bool bRecursive /*= false*/)
	{
		VectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pNode = it.getNext().pSN;
			if (pNode->getName() == rName)
				return pNode;
			if (bRecursive)
			{
				pNode = pNode->getChildByName(rName,true);
				if (pNode)
					return pNode;
			}
		}
		return 0;
	}
	//-----------------------------------------------------
	void Graphical::setPosition( const Vector3& pos )
	{
		VectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pNode = it.getNext().pSN;
			YAKE_ASSERT( pNode );
			if (pNode)
				pNode->setPosition( pos );
		}
	}
	//-----------------------------------------------------
	void Graphical::fromDotScene(const String & fn, graphics::IWorld* pGWorld)
	{
		YAKE_ASSERT( pGWorld );
	
		// 1. read dotscene file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( fn, false );
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotScene document!");

		// 2. parse DOM and create graphical objects

		yake::data::parser::dotscene::DotSceneParserV1 dsp;
		
		Graphical::DotSceneListener dotSceneListener(*this);
		dotSceneListener.reset( pGWorld );
		
		dsp.subscribeToNodeSignal( Bind1( &DotSceneListener::processSceneNode, &dotSceneListener ) );
		dsp.subscribeToEntitySignal( Bind1( &DotSceneListener::processEntity, &dotSceneListener ) );
		dsp.subscribeToCameraSignal( Bind1( &DotSceneListener::processCamera, &dotSceneListener ) );
		dsp.subscribeToLightSignal( Bind1( &DotSceneListener::processLight, &dotSceneListener ) );
		
		dsp.load( ser.getDocumentNode() );
	}
	
/*	
	//-----------------------------------------------------
	Graphical::NameList Graphical::getAllSceneNodes()
	{
		YAKE_LOG( "INFO: Getting all scene nodes" );

		NameList list;
		
		DotSceneListener::SceneNodeMap::iterator i = mDotSceneListener.mSceneNodes.begin();
		DotSceneListener::SceneNodeMap::iterator end = mDotSceneListener.mSceneNodes.end();
		
		for( ; i != end; ++i )
		{
			YAKE_LOG( " Adding to list: " + i->first );
			
			list.push_back( i->first );
		}
		
		return list;
	}

	//-----------------------------------------------------
	Graphical::NameList Graphical::getAllCameras()
	{
		NameList list;
		
		DotSceneListener::CameraMap::iterator i = mDotSceneListener.mCameras.begin();
		DotSceneListener::CameraMap::iterator end = mDotSceneListener.mCameras.end();
		
		for( ; i != end; ++i )
		{
			list.push_back( i->first );
		}
		
		return list;
	}
	
	//-----------------------------------------------------
	Graphical::NameList Graphical::getAllEntities()
	{
		NameList list;
		
		DotSceneListener::EntityMap::iterator i = mDotSceneListener.mEntities.begin();
		DotSceneListener::EntityMap::iterator end = mDotSceneListener.mEntities.end();
		
		for( ; i != end; ++i )
		{
			list.push_back( i->first );
		}
		
		return list;
	}
	
	//-----------------------------------------------------
	Graphical::NameList Graphical::getAllLights()
	{
		NameList list;
		
		DotSceneListener::LightMap::iterator i = mDotSceneListener.mLights.begin();
		DotSceneListener::LightMap::iterator end = mDotSceneListener.mLights.end();
		
		for( ; i != end; ++i )
		{
			list.push_back( i->first );
		}
		
		return list;
	}
	
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getSceneNode( String name )
	{
		graphics::ISceneNode* result = NULL;
		
		result = mDotSceneListener.mSceneNodes[ name ];
		
		return result;
	}
	
	//-----------------------------------------------------
	graphics::IEntity* Graphical::getEntity( String name )
	{
		graphics::IEntity* result = NULL;
		
		result = mDotSceneListener.mEntities[ name ].entity;
		
		return result;
	}
	
	//-----------------------------------------------------
	graphics::ICamera* Graphical::getCamera( String name )
	{
		graphics::ICamera* result = NULL;
		
		result = mDotSceneListener.mCameras[ name ].camera;
		
		return result;
	}
	
	//-----------------------------------------------------
	graphics::ILight* Graphical::getLight( String name )
	{
		graphics::ILight* result = NULL;
		
		result = mDotSceneListener.mLights[ name ].light;
		
		return result;
	}

	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getParentForEntity( String name )
	{
		String parentName = mDotSceneListener.mEntities[ name ].parentSceneNode;
		return getSceneNode( parentName );
	}
	
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getParentForLight( String name )
	{
		String parentName = mDotSceneListener.mLights[ name ].parentSceneNode;
		return getSceneNode( parentName );
	}
	
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getParentForCamera( String name )
	{
		String parentName = mDotSceneListener.mCameras[ name ].parentSceneNode;
		return getSceneNode( parentName );
	}
*/

	//-----------------------------------------------------
	void Graphical::DotSceneListener::processSceneNode( const SceneNodeDesc desc )
	{
		YAKE_DECLARE_FUNCTION( processSceneNode );
		
		YAKE_LOG( "Processing scene node " + desc.name + " with parent node " + desc.parentNodeName );
				
		graphics::ISceneNode* pSN = mGWorld->createSceneNode( desc.name );

		graphics::ISceneNode* parentSN = 0;
		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			parentSN = mSceneNodes[ desc.parentNodeName ];
			parentSN->addChildNode( pSN );
		}
		
		pSN->setPosition( desc.transform.position );
		pSN->setOrientation( desc.transform.rotation );
		pSN->setScale( desc.transform.scale );
		
		mSceneNodes[ desc.name ] = pSN;

		mOwner.addSceneNode( pSN, parentSN ? false : true );
	}
	
	//-----------------------------------------------------
	void Graphical::DotSceneListener::processEntity( const EntityDesc desc )
	{
		YAKE_DECLARE_FUNCTION( processEntity );
		
		YAKE_LOG( "Processing entity " + desc.name + " with parent node " + desc.parentNodeName );
		
		graphics::IEntity* pEntity = mGWorld->createEntity( desc.meshFile );

		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			graphics::ISceneNode* parentSN = mSceneNodes[ desc.parentNodeName ];
			
			parentSN->attachEntity( pEntity );
		}
		
		pEntity->setCastsShadow( desc.castsShadows );
		
		EntityInfo info;
		
		info.entity = pEntity;
		info.parentSceneNode = desc.parentNodeName;
		
		mEntities[ desc.name ] = info;
	}
	
	//-----------------------------------------------------
	void Graphical::DotSceneListener::processCamera( const CameraDesc desc )
	{
		YAKE_DECLARE_FUNCTION( processCamera );
		
		YAKE_LOG( "Processing camera " + desc.name + " with parent node " + desc.parentNodeName );
		
		graphics::ICamera* pCamera = mGWorld->createCamera();

		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			graphics::ISceneNode* parentSN = mSceneNodes[ desc.parentNodeName ];
			
			parentSN->attachCamera( pCamera );
		}
		
		pCamera->setFOV( desc.fov );
		pCamera->setAspectRatio( desc.aspectRatio );
		pCamera->setProjectionType( desc.projectionType );
		pCamera->setNearClipDistance( desc.clipping.nearClip );
		pCamera->setFarClipDistance( desc.clipping.farClip );
		pCamera->setDirection( desc.normal );
		
		CameraInfo info;
		
		info.camera = pCamera;
		info.parentSceneNode = desc.parentNodeName;
		
		mCameras[ desc.name ] = info;
	}
	
	//-----------------------------------------------------
	void Graphical::DotSceneListener::processLight( const LightDesc desc )
	{
		YAKE_DECLARE_FUNCTION( processLight );
		
		YAKE_LOG( "Processing light " + desc.name + " with parent node " + desc.parentNodeName );
		
		graphics::ILight* pLight = mGWorld->createLight();

		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			graphics::ISceneNode* parentSN = mSceneNodes[ desc.parentNodeName ];
			
			parentSN->attachLight( pLight );
		}
		
		pLight->setType( desc.type );
		pLight->setCastsShadows( desc.castsShadows );
		pLight->setDiffuseColour( desc.diffuseColor );
		pLight->setSpecularColour( desc.specularColor );
		pLight->setAttenuation( desc.attenuation.range,
								desc.attenuation.constant,
								desc.attenuation.linear,
								desc.attenuation.quadratic );
		
		if ( desc.type == graphics::ILight::LT_SPOT )
			pLight->setSpotlightRange( desc.range.inner, desc.range.outer, desc.range.falloff );
		
		if ( desc.type != graphics::ILight::LT_POINT )
			pLight->setDirection( desc.normal );
		
		LightInfo info;
		
		info.light = pLight;
		info.parentSceneNode = desc.parentNodeName;
		
		mLights[ desc.name ] = info;
	}
}
}
