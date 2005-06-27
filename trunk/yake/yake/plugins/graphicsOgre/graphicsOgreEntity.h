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
#ifndef INC_YAKE_GRAPHICOGREENTITY_H
#define INC_YAKE_GRAPHICOGREENTITY_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreEntity : public graphics::IEntity
	{
		OgreEntity();
		OgreEntity( const OgreEntity & );
	public:
		OgreEntity( ::Ogre::SceneManager * sceneMgr, const String & mesh );
		virtual ~OgreEntity();

		virtual ISkeleton* getSkeleton() const;

		virtual void setVisible( bool visible );
		virtual void setMaterial( const String & materialName );
		virtual void setSubEntityMaterial( const String & subEntity, const String & materialName );
		virtual void setCastsShadow( bool castsShadow );

		virtual String getName() const;
		virtual void getName(String& name);
		//virtual void setName(const String& name);

		Ogre::Entity* getEntity_() const
		{ return mEntity; }
	protected:
		Ogre::SceneManager* mSceneMgr;
		Ogre::Entity*		mEntity;
	};

}
}
}

#endif
