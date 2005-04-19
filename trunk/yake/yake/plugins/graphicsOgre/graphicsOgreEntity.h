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
		OgreEntity( ::Ogre::SceneManager * sceneMgr, const base::String & mesh );
		virtual ~OgreEntity();

		virtual ISkeleton* getSkeleton() const;

		virtual void setVisible( bool visible );
		virtual void setMaterial( const base::String & materialName );
		virtual void setSubEntityMaterial( const base::String & subEntity, const base::String & materialName );
		virtual void setCastsShadow( bool castsShadow );

		virtual base::String getTag() const;
		virtual void getTag(base::String& tag);
		//virtual void setTag(const String& tag);

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
