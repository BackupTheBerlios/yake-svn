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
#ifndef INC_YAKE_GRAPHICSOGRESKELETON_H
#define INC_YAKE_GRAPHICSOGRESKELETON_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreSkeleton : public graphics::ISkeleton
	{
	public:
		OgreSkeleton( Ogre::Entity& rEntity );
		virtual ~OgreSkeleton();

		virtual void enableAnimation( const base::String& rAnimName, bool enable );
		virtual void advanceAnimation( const base::String& rAnimName, real timeDelta );
		virtual void advanceAllAnimations( real timeDelta );
		virtual void setAnimationWeight( const base::String& rAnimName, real weight );
		virtual void attachEntityToBone( const base::String& rBoneName, IEntity* pEntity );
	
	protected:
		Ogre::Entity& mEntity;
	};

}// ogre3d
}// graphics
}// yake

#endif 
