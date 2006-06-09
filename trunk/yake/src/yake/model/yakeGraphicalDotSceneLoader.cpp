/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#include "yake/model/pch.h"
#include "yake/model/prerequisites.h"
#include "yake/model/model.h"
#include "yake/model/model_graphical_dotscene_loader.h"

namespace yake {
namespace model {

	//-----------------------------------------------------
	void DotSceneListener::processSceneNode( const SceneNodeDesc& desc )
	{
		const String fullName = mNamePrefix + desc.path;
		const String fullParentNodeName = mNamePrefix + desc.parentNodeName;
		YAKE_LOG( "Processing scene node " + desc.name + " (" + fullName + ") with parent node " + desc.parentNodeName + " (" + fullParentNodeName + ")" );

		// create scene node
		graphics::ISceneNode* pSN = mGWorld->createSceneNode( fullName );

		// add to parent node (if necessary)
		graphics::ISceneNode* parentSN = 0;
		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			parentSN = mSceneNodes[ desc.parentNodeName ];
			parentSN->addChildNode( pSN );
		}

		// set properties
		pSN->setName( fullName );
		pSN->setPosition( desc.transform.position );
		pSN->setOrientation( desc.transform.rotation );
		pSN->setScale( desc.transform.scale );

		mSceneNodes[ desc.name ] = pSN;

		// Register with Model object
		mOwner.addSceneNode( pSN, desc.path, parentSN ? false : true );

		// look if this node is tracked
		// TODO current scheme allows node to be tracked by only one target
		TrackerMap::iterator end = mTrackersTargets.end();
		for( TrackerMap::iterator trackRecord = mTrackersTargets.begin(); trackRecord != end; ++trackRecord )
		{
			if ( trackRecord->second == desc.name )
			{
				YAKE_LOG( "Setting track target for camera " + trackRecord->first + " ..." );

				CameraInfo info = mCameras[ trackRecord->first ];

				YAKE_LOG( "Camera info found. Acquiring camera..." );

				graphics::ICamera* tracker = info.camera;

				YAKE_LOG( "Got camera. Setting orientation..." );

				tracker->lookAt( pSN->getPosition( graphics::ISceneNode::TS_WORLD ) );

				YAKE_LOG( "Orientation was set up..." );

				// cleaning up
				mTrackersTargets.erase( trackRecord );
				break;
			}
		}
	}

	//-----------------------------------------------------
	void DotSceneListener::processEntity( const EntityDesc& desc )
	{
		const String fullName = mNamePrefix + desc.name;
		const String fullParentNodeName = mNamePrefix + desc.parentNodeName;
		YAKE_LOG( "Processing entity " + desc.name + " (" + fullName + ") with parent node " + desc.parentNodeName + " (" + fullParentNodeName + ")" );

		// create entity
		graphics::IEntity* pEntity = mGWorld->createEntity( desc.meshFile );
		YAKE_ASSERT( pEntity );

		// lookup scene node
		bool parented = false;
		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			graphics::ISceneNode* parentSN = mSceneNodes[ desc.parentNodeName ];
			YAKE_ASSERT( parentSN );
			parentSN->attachEntity( pEntity );
			parented = true;
		}

		// configure entity
		pEntity->setCastsShadow( desc.castsShadows );

		// Register with Model object
		mOwner.addEntity( pEntity, desc.path, !parented );

		// store entity meta info
		EntityInfo info;

		info.entity = pEntity;
		info.parentSceneNode = desc.parentNodeName;

		mEntities[ desc.name ] = info;
	}

	//-----------------------------------------------------
	void DotSceneListener::processCamera( const CameraDesc& desc )
	{
		YAKE_LOG( "Processing camera " + desc.name + " with parent node " + desc.parentNodeName );

		graphics::ICamera* pCamera = mGWorld->createCamera();

		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			graphics::ISceneNode* parentSN = mSceneNodes[ desc.parentNodeName ];

			parentSN->attachCamera( pCamera );
		}

		pCamera->setName( desc.name );
		pCamera->setFOV( Math::DegreesToRadians( desc.fov ) );
		pCamera->setAspectRatio( desc.aspectRatio );
		pCamera->setProjectionType( desc.projectionType );
		pCamera->setNearClipDistance( desc.clipping.nearClip );
		pCamera->setFarClipDistance( desc.clipping.farClip );

		//	pCamera->setDirection( desc.normal );

		pCamera->setPosition( Vector3() );
		pCamera->setOrientation( Quaternion::kIdentity );


		if ( desc.trackTargetName != "" )
		{
			YAKE_LOG( "Saving tracking record for camera " + desc.name + " ... " );

			mTrackersTargets.insert( TrackerMap::value_type( desc.name, desc.trackTargetName ) );
		}

		CameraInfo info;

		info.camera = pCamera;
		info.parentSceneNode = desc.parentNodeName;

		mCameras[ desc.name ] = info;
	}

	//-----------------------------------------------------
	void DotSceneListener::processLight( const LightDesc& desc )
	{
		const String fullName = mNamePrefix + desc.name;
		const String fullParentNodeName = mNamePrefix + desc.parentNodeName;
		YAKE_LOG( "Processing light " + desc.name + " (" + fullName + ") with parent node " + desc.parentNodeName + " (" + fullParentNodeName + ")" );

		// create light
		graphics::ILight* pLight = mGWorld->createLight();

		// lookup parent scene node
		bool parented = false;
		if ( desc.parentNodeName != yake::data::parser::dotscene::DotSceneParserV1::ROOT_NODE_NAME )
		{
			graphics::ISceneNode* parentSN = mSceneNodes[ desc.parentNodeName ];
			YAKE_ASSERT( parentSN );

			parentSN->attachLight( pLight );
			parented = true;
		}

		// register with Model
		mOwner.addLight( pLight, desc.path, !parented );

		// configure light
		pLight->setName( desc.name );
		pLight->setType( desc.type );
		pLight->setCastsShadows( desc.castsShadows );

		if ( desc.castsShadows )
		{
			YAKE_LOG( "  light casts shadows" );
		}
		else
		{
			YAKE_LOG( "  light doesn't cast shadows" );
		}

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

		// store light meta info
		LightInfo info;

		info.light = pLight;
		info.parentSceneNode = desc.parentNodeName;

		mLights[ desc.name ] = info;
	}

} // namespace model
} // namespace yake
