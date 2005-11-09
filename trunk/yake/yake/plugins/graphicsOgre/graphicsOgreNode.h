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
#ifndef INC_YAKE_GRAPHICOGRENODE_H
#define INC_YAKE_GRAPHICOGRENODE_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreEntity;
	class OgreNode : public graphics::ISceneNode, public OgreWrappedObject
	{
	public:
		OgreNode( GraphicalWorld& owningWorld, Ogre::SceneManager * sceneMgr, const String& name = "" );
		virtual ~OgreNode();

		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;
		virtual Vector3 getPosition( TransformSpace ts ) const;
		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;

		virtual void setScale( const Vector3 & scale );
		virtual Vector3 getScale() const;

		virtual Vector3 getDerivedPosition() const;
		virtual void getDerivedPosition( Vector3& retPos ) const;
		virtual Quaternion getDerivedOrientation() const;
		virtual void getDerivedOrientation( Quaternion& retRot ) const;
		virtual void translate( const Vector3& rDelta, const TransformSpace relativeTo = TS_PARENT );
		virtual void rotate( const Quaternion& rDelta, const TransformSpace relativeTo = TS_PARENT );

		virtual void addChildNode( ISceneNode* pNode );
		virtual ISceneNode* createChildNode( const String& name = "" );
		virtual void attachEntity( IEntity* pEntity );
		virtual void attachCamera( ICamera* pCamera );
		virtual void attachLight( ILight* pLight );
		virtual void attachParticleSystem( IParticleSystem* pParticleSys );
		virtual void removeAndDestroyAllChildren();
		virtual void getChildren( SceneNodePtrList& ret, bool bRecursive = false ) const;
		virtual ISceneNode* getChildByName( const String& name, bool bRecursive = false );

		virtual const EntityPtrList& getEntities() const;
		virtual const LightPtrList& getLights() const;
		virtual const CameraPtrList& getCameras() const;
		virtual const ParticleSystemPtrList& getParticleSystems() const;

		virtual void detach( IEntity* pEntity );
		virtual void detach( ILight* pLight );
		virtual void detach( ICamera* pCamera );
		virtual void detach( IParticleSystem* pPS );
		virtual void detach( ISceneNode* pNode );

		Ogre::SceneNode* getSceneNode_() const
		{ return mSceneNode; }
		void _onAttached( OgreNode* );
		void _onDetached();
	protected:
		typedef SceneNodePtrList NodeList;
		NodeList			mChildren;
		EntityPtrList		mEntities;
		LightPtrList		mLights;
		CameraPtrList		mCameras;
		ParticleSystemPtrList	mParticleSystems;

		Ogre::SceneNode* 	mSceneNode;
		Ogre::SceneManager*	mSceneMgr;
		OgreNode*			mParentNode;
	};

}
}
}

#endif
