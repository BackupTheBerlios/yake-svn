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
#ifndef YAKE_PLUGINS_GRAPHICSWORLD_H
#define YAKE_PLUGINS_GRAPHICSWORLD_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/graphics/yakeGraphicalWorld.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>

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
	class OgreEntity;
	/** The graphics system implementation. 
	*/
	class YAKE_GRAPHICS_CONCRETE_API GraphicalWorld	: public IWorld
	{
	public:
		GraphicalWorld();
		virtual ~GraphicalWorld();

		virtual StringVector getShadowTechniques() const;
		virtual bool selectShadowTechnique(const String& name, const StringMap& params);
		/** Globally activates the default shadowing method, often stencil shadows.	*/
		void setShadowsEnabled( bool enabled );

		/** Render a single frame. */
		void render( real timeElapsed );

		virtual void setSkyBox( const String& name );

		virtual ISceneNode* createSceneNode( const String& tag = "" );
		virtual IEntity* createEntity(const String & mesh);
		virtual ILight* createLight();
		virtual IParticleSystem* createParticleSystem( const String& rPSTemplateName );
		virtual ICamera* createCamera();
		virtual IViewport* createViewport(ICamera* pCamera);

		virtual IMeshGeometryAccess* createProceduralMesh(const String & name);
		virtual void destroyProceduralMesh(const String & name);
		virtual IMeshGeometryAccess* getProceduralMesh(const String & name);

		virtual real getRenderWindowWidth() const;
		virtual real getRenderWindowHeight() const;

		virtual IEntity* pickEntity(const Ray& ray);

		static void setCore( OgreCore* core );

		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void(real), PreRender )
		YAKE_MEMBERSIGNAL_FIRE_FN1( public, PreRender, real timeElapsed, timeElapsed )
		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void(real), PostRender )
		YAKE_MEMBERSIGNAL_FIRE_FN1( public, PostRender, real timeElapsed, timeElapsed )

		void reg( OgreWrappedObject* pO );
		void unreg( OgreWrappedObject* pO );
	private:
		static OgreCore*		msCore;
		typedef AssocVector< String, Ogre::MeshPtr > ProcMeshMap;
		ProcMeshMap				mProcMeshes;
		Ogre::RaySceneQuery*	mpRaySceneQuery;

		typedef std::map< Ogre::Entity*, OgreEntity* > EntityMap;
		EntityMap				mEntityMap;

		typedef std::deque< OgreWrappedObject* > WrappedList;
		WrappedList				mWrapped;
		uint32					mLastWrappedId;

		String					mCurrentShadowTechnique;
		StringMap				mCurrentShadowTechniqueParams;
	};

} // ogre3d
} // graphics
} // yake

#endif // YAKE_PLUGINS_GRAPHICSWORLD_H
