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
#include <yapp/model/yakePhysicalDataImporter.h> 

namespace yake {
namespace model {

	//-----------------------------------------------------
	void XODEListener::processBody( const XODEParser::BodyDesc& desc )
	{
		YAKE_LOG( "XODE listener: processing body description..." );

		// create actor
		physics::IActorPtr pDynActor = physicalWorld_->createActor( physics::ACTOR_DYNAMIC );
		YAKE_ASSERT( pDynActor != NULL ).error( "Failed to create actor!" );

		// set transform. TODO what about scale?
		pDynActor->setPosition( desc.transform_.position_ );
		pDynActor->setOrientation( desc.transform_.rotation_ );

		// adding to collection
		ActorInfo info;
		info.actor_ = pDynActor;
		info.parentName_ = desc.parentName_;

		actors_[ desc.name_ ] = info;

		// filling owner model
		owner_.addActor( pDynActor, desc.name_ );

		YAKE_LOG( "XODE listener: Success." );
	}

	//-----------------------------------------------------
	void XODEListener::processGeom( const XODEParser::GeomDesc& desc )
	{
		YAKE_LOG( "XODE listener: processing geom description..." );

		physics::IActorPtr parentActor = NULL;

		// search for parent in actors
		ActorMap::iterator actor = actors_.find( desc.parentName_ );

		if ( actor != actors_.end() )
		{
			// parent actor found!
			parentActor = actor->second.actor_;
		}
		else
		{
			// create static actor
			parentActor = physicalWorld_->createActor( physics::ACTOR_STATIC );

			// add to collection
			ActorInfo info;
			info.actor_ = parentActor;
			info.parentName_ = ""; // FIXME what should be here? a space perhaps?

			actors_[ desc.parentName_ ] = info; 
		}

		IShape* shape = parentActor->createShape( *desc.shape_ );

		GeomInfo info;
		info.shape_ = shape;
		info.parentName_ = desc.parentName_;

		// add to collection 
		geoms_[ desc.name_ ] = info;

		YAKE_LOG( "XODE listener: Success." );
	}

	//-----------------------------------------------------
	void XODEListener::processMaterial( const XODEParser::MaterialDesc& desc )
	{
		YAKE_LOG( "XODE listener: processing material description..." );

		IMaterial* material = physicalWorld_->createMaterial( desc.material_, desc.name_ );

		// find parent
		GeomMap::iterator shape = geoms_.find( desc.parentName_ );

		if ( shape != geoms_.end() )
		{
			YAKE_LOG( "XODE listener: shape found! Applying material..." );

			// apply material to shape
			shape->second.shape_->setMaterial( material );
		}

		MaterialInfo info;
		info.material_ = material;
		info.parentName_ = desc.parentName_;

		// add to collection
		materials_[ desc.name_ ] = info;

		YAKE_LOG( "XODE listener: Success." );
	}

	//-----------------------------------------------------
	void XODEListener::processMass( const XODEParser::MassDesc& desc )
	{
		YAKE_LOG( "XODE listener: processing mass description..." );

		// search for parent in actors
		ActorMap::iterator actor = actors_.find( desc.parentName_ );

		if ( actor != actors_.end() )
		{
			// parent actor found!
			IBody& body = actor->second.actor_->getBody();

			YAKE_LOG( "XODE listener: owner body found. Adding mass..." );

			body.addMass( *desc.mass_ );

			// TODO process mass... add to collection etc.
		}

		YAKE_LOG( "XODE listener: Success." );
	}

	//-----------------------------------------------------
	void XODEListener::processJoint( const XODEParser::JointDesc& desc )
	{
		YAKE_LOG( "XODE listener: processing joint description..." );

		// storing joint description for further processing...
		jointDescriptions_.push_back( desc );

		YAKE_LOG( "XODE listener: Success." );
	}

	//-----------------------------------------------------
	void XODEListener::postprocess()
	{
		// create joints here
		YAKE_LOG( "XODE listener: postprocessing..." );

		// TODO only implemented creation of joint connecting dynamic objects

		JointDescVector::iterator end = jointDescriptions_.end();
		for( JointDescVector::iterator i = jointDescriptions_.begin(); i != end; ++i )
		{
			XODEParser::JointDesc& desc = *i;

			// search for connected actors

			YAKE_LOG( "XODE listener: searching for actors [0]: " + desc.parentName_ + ", [1]: " + desc.otherBody_ );

			ActorMap::iterator actor0 = actors_.find( desc.parentName_ );
			ActorMap::iterator actor1 = actors_.find( desc.otherBody_ );

			if ( actor0 == actors_.end() || actor1 == actors_.end() )
			{
				YAKE_LOG( "XODE listener: Failed to find actors... :( " );
				continue;
			}

			desc.joint_->actor0 = actor0->second.actor_;
			desc.joint_->actor1 = actor1->second.actor_;

			IJointPtr joint = physicalWorld_->createJoint( *desc.joint_ );

			JointInfo info;
			info.joint_ = joint;
			info.parentName_ = desc.parentName_;

			// add to collection
			joints_[ desc.name_ ] = info;
		}

		YAKE_LOG( "XODE listener: finished postprocessing." );
	}

} // model
} // yake
