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
*/
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <yake/plugins/graphicsOgre/graphicsOgreLight.h>
#include <yake/plugins/graphicsOgre/graphicsOgreNode.h>
#include <yake/plugins/graphicsOgre/graphicsOgreParticleSystem.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreNode::OgreNode( Ogre::SceneManager * sceneMgr ) : mSceneMgr( sceneMgr ), mSceneNode( 0 )
	{
		YAKE_ASSERT( sceneMgr ).debug("need a scene manager!");
		mSceneNode = static_cast< Ogre::SceneNode* >( mSceneMgr->getRootSceneNode()->createChild( base::uniqueName::create("sn_") ) );
		YAKE_ASSERT( mSceneNode ).warning("Couldn't create a scene node!");
		mSceneNode->setPosition( 0, 0, 0 );
	}

	//------------------------------------------------------
	OgreNode::~OgreNode()
	{
		if (mSceneNode)
		{
			mSceneNode->removeAllChildren();
			mSceneMgr->destroySceneNode( mSceneNode->getName() );
		}
	}

	//------------------------------------------------------
	void OgreNode::setPosition( const Vector3 & position )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		mSceneNode->setPosition( VEC_YAKE2OGRE( position ) );
	}

	//------------------------------------------------------
	Vector3 OgreNode::getPosition() const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		return VEC_OGRE2YAKE( mSceneNode->getPosition() );
	}

	//------------------------------------------------------
	void OgreNode::setOrientation( const Quaternion & orientation )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		mSceneNode->setOrientation( QUAT_YAKE2OGRE( orientation ) );
	}

	//------------------------------------------------------
	Quaternion OgreNode::getOrientation() const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		return QUAT_OGRE2YAKE( mSceneNode->getOrientation() );
	}

	//------------------------------------------------------
	void OgreNode::setScale( const Vector3 & scale )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		mSceneNode->setScale( VEC_YAKE2OGRE( scale ) );
	}

	//------------------------------------------------------
	Vector3 OgreNode::getScale() const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		return VEC_OGRE2YAKE( mSceneNode->getScale() );
	}

	//------------------------------------------------------
	void OgreNode::removeAndDestroyAllChildren()
	{
	}

	//------------------------------------------------------
	void OgreNode::attachCamera( ICamera* pCamera )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pCamera ).warning("need a camera");
		
		if ( NULL == pCamera )
			return;
		
		mSceneNode->attachObject( static_cast<OgreCamera*>( pCamera )->getCamera_() );
	}

	//------------------------------------------------------
	void OgreNode::attachLight( ILight* pLight )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pLight ).warning("need a light");
		
		if ( NULL == pLight )
			return;
		
		mSceneNode->attachObject( static_cast<OgreLight*>( pLight )->getLight_() );
	}

	//------------------------------------------------------
	void OgreNode::attachEntity( IEntity* pEntity )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pEntity ).warning("need an entity");

		if ( NULL == pEntity )
			return;

		mSceneNode->attachObject( static_cast<OgreEntity*>(pEntity)->getEntity_() );
	}

	//------------------------------------------------------
	void OgreNode::attachParticleSystem( IParticleSystem* pParticleSys )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pParticleSys ).warning("need a particle system!");
		
		if ( NULL == pParticleSys )
			return;

		mSceneNode->attachObject( static_cast<OgreParticleSystem*>( pParticleSys )->getParticleSystem_() );
	}

	//------------------------------------------------------
	void OgreNode::addChildNode( ISceneNode* pNode )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		mSceneNode->addChild( static_cast<OgreNode*>(pNode)->getSceneNode_() );
		mChildren.push_back( static_cast<OgreNode*>(pNode) );
	}

	//------------------------------------------------------
	base::String OgreNode::getTag() const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		return base::String( mSceneNode->getName().c_str() );
	}

	//------------------------------------------------------
	void OgreNode::getTag(base::String& tag)
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		tag = mSceneNode->getName().c_str();
	}
}
}
}
