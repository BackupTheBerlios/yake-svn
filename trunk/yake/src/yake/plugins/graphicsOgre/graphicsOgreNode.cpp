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
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreNode::OgreNode( GraphicalWorld& owningWorld, Ogre::SceneManager * sceneMgr, const String& name /*= ""*/ ) : 
			mWorld( owningWorld ),
			mSceneNode( 0 ) ,mSceneMgr( sceneMgr ), mParentNode(0)
	{
		YAKE_ASSERT( sceneMgr ).debug("need a scene manager!");
		String ogreid = name;
		if (ogreid.empty())
			ogreid = uniqueName::create("sn_"+name+"_");
		mName = name;
		mSceneNode = static_cast< Ogre::SceneNode* >( mSceneMgr->getRootSceneNode()->createChild( ogreid ) );
		YAKE_ASSERT( mSceneNode ).warning("Couldn't create a scene node!");
		mSceneNode->setPosition( 0, 0, 0 );
		mWorld.reg( this );
		//YAKE_LOG(String("gfx: new node '") << name << "' ogreid='" << String(mSceneNode->getName().c_str()) << "'");
	}

	//------------------------------------------------------
	template<class Ctr>
	void destroyPtrContainer(Ctr& ctr)
	{
		ConstDequeIterator< Ctr > it( ctr );
		while (it.hasMoreElements())
			delete it.getNext();
		ctr.clear();
	}

	//------------------------------------------------------
	template<class T>
	void detachAndDestroyPtrContainer(std::deque<T>& ctr, OgreNode& n)
	{
		ConstDequeIterator< std::deque<T> > it( ctr );
		while (it.hasMoreElements())
		{
			T obj = it.getNext();
			n.detach( obj );
			delete obj;
		}
		ctr.clear();
	}

	//------------------------------------------------------
	OgreNode::~OgreNode()
	{
		//YAKE_LOG(String("gfx: node d'tor '") << this->getName() << "' - cleaning... ");

		if (mParentNode)
		{
			mParentNode->detach( this );
			mParentNode = 0;
		}

		detachAndDestroyPtrContainer( mEntities, *this );
		detachAndDestroyPtrContainer( mLights, *this );
		detachAndDestroyPtrContainer( mCameras, *this );
		detachAndDestroyPtrContainer( mParticleSystems, *this );

		destroyPtrContainer( mChildren );

		YAKE_ASSERT( mSceneNode );
		if (mSceneNode)
		{
			YAKE_ASSERT( mSceneNode->numAttachedObjects() == 0 );
			YAKE_ASSERT( mSceneNode->numChildren() == 0 );
			if (mSceneNode->getParent())
				mSceneNode->getParent()->removeChild( mSceneNode );
			mSceneNode->detachAllObjects();
			mSceneNode->removeAndDestroyAllChildren(); // just in case...
			//YAKE_LOG(String("destroying OGRE SN '") << mSceneNode->getName().c_str() << "'");
			mSceneMgr->destroySceneNode( mSceneNode->getName() );
		}

		this->mWorld.unreg( this );
		//YAKE_LOG(String("gfx: node d'tor '") << this->getName() << "' - done. ");
	}

	//------------------------------------------------------
	void OgreNode::detach( ISceneNode* pNode )
	{
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( mSceneNode );

		Ogre::SceneNode* pChildSN = static_cast<OgreNode*>(pNode)->getSceneNode_();
		YAKE_ASSERT( pChildSN );
		YAKE_ASSERT( pChildSN->getParent() == mSceneNode );
		mSceneNode->removeChild( pChildSN->getName() );
		
		NodeList::iterator itFind = std::find( mChildren.begin(), mChildren.end(), pNode );
		if (itFind != mChildren.end())
			mChildren.erase( itFind );
		
		static_cast<OgreNode*>(pNode)->_setParent(0);
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
		
		OgreCamera* pCam = static_cast<OgreCamera*>( pCamera );
		mSceneNode->attachObject( pCam->getCamera_() );
		mCameras.push_back( pCamera );
	}

	//------------------------------------------------------
	void OgreNode::attachLight( ILight* pLight )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pLight ).warning("need a light");
		
		if ( NULL == pLight )
			return;
		
		OgreLight* pL = static_cast<OgreLight*>( pLight );
		mSceneNode->attachObject( pL->getLight_() );
		mLights.push_back( pLight );
	}

	//------------------------------------------------------
	void OgreNode::attachEntity( IEntity* pEntity )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		YAKE_ASSERT( pEntity ).warning("need an entity");

		if ( NULL == pEntity )
			return;

		OgreEntity* pE = static_cast<OgreEntity*>( pEntity );
		//YAKE_LOG(String("gfx node '") << this->getName() << "'): attachEntity '" << pE->getName() << "'");
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

		OgreParticleSystem* pPS = static_cast<OgreParticleSystem*>( pParticleSys );
		mSceneNode->attachObject( pPS->getParticleSystem_() );

		mParticleSystems.push_back( pParticleSys );
	}

	//------------------------------------------------------
	void OgreNode::_setParent( OgreNode* pParent )
	{
		mParentNode = pParent;
	}

	//------------------------------------------------------
	OgreNode* OgreNode::_getParent() const
	{
		return mParentNode;
	}

	//------------------------------------------------------
	void OgreNode::addChildNode( ISceneNode* pNode )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		OgreNode* pN = static_cast<OgreNode*>( pNode );

		OgreNode* pOtherParent = pN->_getParent();
		if (pOtherParent)
			pOtherParent->detach( pN );
		YAKE_ASSERT( !pN->_getParent() );

		Ogre::SceneNode* pChildSN = pN->getSceneNode_();
		YAKE_ASSERT( pChildSN );

		// in case pChildSN is a child of the OGRE scene manager's root node...
		if (pChildSN->getParent())
			pChildSN->getParent()->removeChild( pChildSN );

		mSceneNode->addChild( pChildSN );
		mChildren.push_back( pNode );
		pN->_setParent( this );
	}

	//------------------------------------------------------
	ISceneNode* OgreNode::createChildNode( const String& name /*= ""*/ )
	{
		YAKE_ASSERT( mSceneNode ).debug("need a scene node!");
		OgreNode* pChild = new OgreNode( mWorld, mSceneMgr, name );
		this->addChildNode( pChild );
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
		OgreEntity* pObj = static_cast<OgreEntity*>(pEntity);
		//YAKE_LOG(String("gfx node '") << this->getName() << "': detachEntity '" << pObj->getName() << "'");
		mSceneNode->detachObject( pObj->getEntity_() );
		
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
		OgreLight* pObj = static_cast<OgreLight*>(pLight);
		mSceneNode->detachObject( pObj->getLight_() );

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
		OgreCamera* pObj = static_cast<OgreCamera*>(pCamera);
		mSceneNode->detachObject( pObj->getCamera_() );

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
		OgreParticleSystem* pObj = static_cast<OgreParticleSystem*>(pPS);
		mSceneNode->detachObject( pObj->getParticleSystem_() );

		// removing from list
		ParticleSystemPtrList::iterator victim = std::find( mParticleSystems.begin(), mParticleSystems.end(), pPS );
		YAKE_ASSERT( victim != mParticleSystems.end() ).error( "particle system was NOT attached!" );

		mParticleSystems.erase( victim );
	}
}
}
}
