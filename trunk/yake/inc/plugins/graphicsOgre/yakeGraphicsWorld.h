/**------------------------------------------------------------------------------------
This file is part of YAKE
Copyright � 2004 The YAKE Team
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
#ifndef YAKE_PLUGINS_GRAPHICSWORLD_H
#define YAKE_PLUGINS_GRAPHICSWORLD_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <inc/graphics/yakeGraphicalWorld.h>
#include <inc/plugins/graphicsOgre/yakeGraphicsSystem.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::mpl;
using namespace yake::base;
namespace yake {
namespace graphics {
	class IMeshGeometryAccess;
namespace ogre3d {

	class OgreCore;
	/** The graphics system implementation. 
	*/
	class YAKE_GRAPHICS_CONCRETE_API GraphicalWorld	: public IGraphicalWorld
	{
	public:
		GraphicalWorld();
		/** Globally activates the default shadowing method, often stencil shadows.	*/
		void setShadowsEnabled( bool enabled );

		/** Render a single frame. */
		void render( real timeElapsed );

		virtual ISceneNode* createSceneNode();
		virtual IEntity* createEntity(const base::String & mesh);
		virtual ILight* createLight();
		virtual ICamera* createCamera();
		virtual IViewport* createViewport(ICamera* pCamera);

		virtual IMeshGeometryAccess* createProceduralMesh(const String & name);
		virtual void destroyProceduralMesh(const base::String & name);
		virtual IMeshGeometryAccess* getProceduralMesh(const String & name);

		static void setCore( OgreCore* core );
	private:
		static OgreCore*	msCore;
		typedef base::templates::AssocVector< String, Ogre::Mesh* > ProcMeshMap;
		ProcMeshMap			mProcMeshes;
	};

} // ogre3d
} // graphics
} // yake

#endif // YAKE_PLUGINS_GRAPHICSWORLD_H