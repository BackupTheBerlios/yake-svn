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
	OgreNode::OgreNode( Ogre::SceneManager * sceneMgr, const String& name /*= ""*/ ) : 
			mSceneNode( 0 ) ,mSceneMgr( sceneMgr )
	{
		YAKE_ASSERT( sceneMgr ).debug("need a scene manager!");
		String id = name;
		if (id.empty())
			id = uniqueName::create("sn_");
		mSceneNode = static_cast< Ogre::SceneNode* >( mSceneMgr->getRootSceneNode()->createChild( id ) );
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
		mLights.clear();
		mEntities.clear();
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
	Vector3 OgreNode::getPosition( ISceneNode::TransformSpace ts ) const
	{
		switch( ts )
		{
			case TS_LOCAL:
				return Vector3();
				
			case TS_PARENT:
				return getPosition();
				
			case TS_WORLD:
				return getDerivedPosition();
			
			default:
				YAKE_ASSERT( 0 ).error( "Unknown transform space" );
				return Vector3();
		}
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
	Vector3 OgreNode::getDerivedPosition() const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		return VEC_OGRE2YAKE( mSceneNode->_getDerivedPosition() );
	}

	//------------------------------------------------------
	void OgreNode::getDerivedPosition( Vector3& retPos ) const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		retPos = VEC_OGRE2YAKE( mSceneNode->_getDerivedPosition() );
	}

	//------------------------------------------------------
	Quaternion OgreNode::getDerivedOrientation() const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		return QUAT_OGRE2YAKE( mSceneNode->_getDerivedOrientation() );
	}

	//------------------------------------------------------
	void OgreNode::getDerivedOrientation( Quaternion& retRot ) const
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		retRot = QUAT_OGRE2YAKE( mSceneNode->_getDerivedOrientation() );
	}

	//------------------------------------------------------
	void OgreNode::translate( const Vector3& rDelta, const TransformSpace relativeTo /*= TS_PARENT*/ )
	{
		mSceneNode->translate( VEC_YAKE2OGRE(rDelta), TS_YAKE2OGRE(relativeTo) );
	}

	//------------------------------------------------------
	void OgreNode::rotate( const Quaternion& rDelta, const TransformSpace relativeTo /*= TS_PARENT*/ )
	{
		mSceneNode->rotate( QUAT_YAKE2OGRE(rDelta), TS_YAKE2OGRE(relativeTo) );
	}

	//------------------------------------------------------
	void OgreNode::removeAndDestroyAllChildren()
	{
		YAKE_ASSERT( 0 );
		//isn't enough: mSceneNode->removeAndDestroyAllChildren();
	}

	//------------------------------------------------------
	void OgreNode::attachCamera( ICamera* pCamera )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pCamera ).warning("need a camera");
		
		if ( NULL == pCamera )
			return;
		
		mSceneNode->attachObject( static_cast<OgreCamera*>( pCamera )->getCamera_() );
		mCameras.push_back( pCamera );
	}

	//------------------------------------------------------
	void OgreNode::attachLight( ILight* pLight )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pLight ).warning("need a light");
		
		if ( NULL == pLight )
			return;
		
		mSceneNode->attachObject( static_cast<OgreLight*>( pLight )->getLight_() );
		mLights.push_back( pLight );
	}

	//------------------------------------------------------
	void OgreNode::attachEntity( IEntity* pEntity )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pEntity ).warning("need an entity");

		if ( NULL == pEntity )
			return;

		mSceneNode->attachObject( static_cast<OgreEntity*>(pEntity)->getEntity_() );
		mEntities.push_back( pEntity );
	}

	//------------------------------------------------------
	void OgreNode::attachParticleSystem( IParticleSystem* pParticleSys )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pParticleSys ).warning("need a particle system!");
		
		if ( NULL == pParticleSys )
			return;

		mSceneNode->attachObject( static_cast<OgreParticleSystem*>( pParticleSys )->getParticleSystem_() );

		mParticleSystems.push_back( pParticleSys );
	}

	//------------------------------------------------------
	void OgreNode::addChildNode( ISceneNode* pNode )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		mSceneNode->addChild( static_cast<OgreNode*>(pNode)->getSceneNode_() );
		mChildren.push_back( pNode );
	}

	//------------------------------------------------------
	ISceneNode* OgreNode::createChildNode( const String& name /*= ""*/ )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		OgreNode* pChild = new OgreNode( mSceneMgr, name );
		mSceneNode->addChild( pChild->getSceneNode_() );
		mChildren.push_back( pChild );
		return pChild;
	}

	//------------------------------------------------------
	void OgreNode::getChildren( SceneNodePtrList& ret, bool bRecursive /*= false*/ ) const
	{
		std::copy( mChildren.begin(), mChildren.end(), std::back_inserter( ret ) );
		if (bRecursive)
		{
			ConstVectorIterator<NodeList> it(mChildren);
			while (it.hasMoreElements())
			{
				ISceneNode* pNode = it.getNext();
				pNode->getChildren( ret, bRecursive );
			}
		}
	}

	//------------------------------------------------------
	ISceneNode* OgreNode::getChildByName( const String& name, bool bRecursive /*= false*/ )
	{
		VectorIterator<NodeList> it(mChildren);
		while (it.hasMoreElements())
		{
			ISceneNode* pNode = it.getNext();
			if (pNode->getName() == name)
				return pNode;
			if (bRecursive)
			{
				pNode = pNode->getChildByName( name, bRecursive );
				if (pNode)
					return pNode;
			}
		}
		
		return 0;
	}

	//------------------------------------------------------
	const EntityPtrList& OgreNode::getEntities() const
	{
		return mEntities;
	}

	//------------------------------------------------------
	const LightPtrList& OgreNode::getLights() const
	{
		return mLights;
	}

	//------------------------------------------------------
	const CameraPtrList& OgreNode::getCameras() const
	{
		return mCameras;
	}

	//------------------------------------------------------
	const ParticleSystemPtrList& OgreNode::getParticleSystems() const
	{
		return mParticleSystems;
	}

	//------------------------------------------------------
	void OgreNode::detach( IEntity* pEntity )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pEntity ).warning("passed a null ptr!");
		if (!pEntity)
			return;
		mSceneNode->detachObject( static_cast<OgreEntity*>(pEntity)->getEntity_() );
		
		// removing from list
		EntityPtrList::iterator victim = std::find( mEntities.begin(), mEntities.end(), pEntity );
		YAKE_ASSERT( victim != mEntities.end() ).error( "entity was NOT attached!" );

		mEntities.erase( victim );
	}

	//------------------------------------------------------
	void OgreNode::detach( ILight* pLight )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pLight ).warning("passed a null ptr!");
		if (!pLight)
			return;
		mSceneNode->detachObject( static_cast<OgreLight*>(pLight)->getLight_() );

		// removing from list
		LightPtrList::iterator victim = std::find( mLights.begin(), mLights.end(), pLight );
		YAKE_ASSERT( victim != mLights.end() ).error( "light was NOT attached!" );

		mLights.erase( victim );
	}

	//------------------------------------------------------
	void OgreNode::detach( ICamera* pCamera )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pCamera ).warning("passed a null ptr!");
		if (!pCamera)
			return;
		mSceneNode->detachObject( static_cast<OgreCamera*>(pCamera)->getCamera_() );

		// removing from list
		CameraPtrList::iterator victim = std::find( mCameras.begin(), mCameras.end(), pCamera );
		YAKE_ASSERT( victim != mCameras.end() ).error( "camera was NOT attached!" );

		mCameras.erase( victim );
	}

	//------------------------------------------------------
	void OgreNode::detach( IParticleSystem* pPS )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pPS ).warning("passed a null ptr!");
		if (!pPS)
			return;
		mSceneNode->detachObject( static_cast<OgreParticleSystem*>(pPS)->getParticleSystem_() );

		// removing from list
		ParticleSystemPtrList::iterator victim = std::find( mParticleSystems.begin(), mParticleSystems.end(), pPS );
		YAKE_ASSERT( victim != mParticleSystems.end() ).error( "particle system was NOT attached!" );

		mParticleSystems.erase( victim );
	}
}
}
}
