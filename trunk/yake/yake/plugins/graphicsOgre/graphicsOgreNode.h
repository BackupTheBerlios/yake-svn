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
#ifndef INC_YAKE_GRAPHICOGRENODE_H
#define INC_YAKE_GRAPHICOGRENODE_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreEntity;
	class OgreNode : public graphics::ISceneNode
	{
	public:
		OgreNode( Ogre::SceneManager * sceneMgr );
		virtual ~OgreNode();

		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;

		virtual void setScale( const Vector3 & scale );
		virtual Vector3 getScale() const;

		virtual void addChildNode( ISceneNode* pNode );
		virtual void attachEntity( IEntity* pEntity );
		virtual void attachCamera( ICamera* pCamera );
		virtual void attachLight( ILight* pLight );
		virtual void attachParticleSystem( IParticleSystem* pParticleSys );
		virtual void removeAndDestroyAllChildren();

		Ogre::SceneNode* getSceneNode_() const
		{ return mSceneNode; }
	protected:
		typedef base::templates::Vector< OgreNode* > NodeList;
		NodeList			mChildren;

		typedef base::templates::Vector< OgreEntity* > EntityList;
		EntityList			mEntities;

		Ogre::SceneNode		* mSceneNode;
		Ogre::SceneManager	* mSceneMgr;
	};

}
}
}

#endif
