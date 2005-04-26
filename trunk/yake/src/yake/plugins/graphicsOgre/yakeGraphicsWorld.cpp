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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/graphics/yakeGeometryAccess.h>
#include <OgreMeshManager.h>
#include <OgreNoMemoryMacros.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <yake/plugins/graphicsOgre/graphicsOgreNode.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <yake/plugins/graphicsOgre/graphicsOgreViewport.h>
#include <yake/plugins/graphicsOgre/graphicsOgreLight.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>
#include <yake/plugins/graphicsOgre/graphicsOgreParticleSystem.h>
#include <yake/plugins/graphicsOgre/graphicsOgreGeometryAccess.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {
namespace ogre3d {

	OgreCore* GraphicalWorld::msCore = 0;

	//-----------------------------------------------------
	void GraphicalWorld::setCore( OgreCore* core )
	{
		msCore = core;
		YAKE_ASSERT( msCore );
	}

	//-----------------------------------------------------
	GraphicalWorld::GraphicalWorld()
	{
		YAKE_ASSERT( msCore );
	}
	
	//-----------------------------------------------------
	ISceneNode* GraphicalWorld::createSceneNode()
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreNode( msCore->getSceneMgr() );
	}

	//-----------------------------------------------------
	IEntity* GraphicalWorld::createEntity( const String & mesh )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreEntity( msCore->getSceneMgr(), mesh );
	}

	//-----------------------------------------------------
	ILight* GraphicalWorld::createLight()
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreLight( msCore->getSceneMgr() );
 	}

	//-----------------------------------------------------
	IParticleSystem* GraphicalWorld::createParticleSystem( const String& rPSTemplateName )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreParticleSystem( *msCore->getParticleSysMgr(), rPSTemplateName );
	}

	//-----------------------------------------------------
	ICamera* GraphicalWorld::createCamera()
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreCamera( msCore->getSceneMgr() );
	}

	//------------------------------------------------------
	IViewport* GraphicalWorld::createViewport( ICamera * pCamera )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		YAKE_ASSERT( pCamera ).debug("need a camera!");
		YAKE_TRY
		return new OgreViewport( msCore, static_cast<OgreCamera*>( pCamera ) );
		YAKE_CATCH_OGRE_RETHROW
	}

	//-----------------------------------------------------
	void GraphicalWorld::setShadowsEnabled( bool enabled )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		Ogre::SceneManager* pSceneMgr = msCore->getSceneMgr();;
		YAKE_ASSERT( pSceneMgr ).debug("need a scene manager!");
		if (enabled)
		{
			//pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );
			//pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_MODULATIVE );
			pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE );
			pSceneMgr->setShadowTextureSize( 1024 );
			pSceneMgr->setShadowDirectionalLightExtrusionDistance( 500 );
			pSceneMgr->setShadowFarDistance( 75 );
			pSceneMgr->setShadowTextureCount( 3 );
			pSceneMgr->setShadowColour( Ogre::ColourValue(0.7,0.7,0.7) );
			pSceneMgr->setShowDebugShadows( false );
		}
		else
			pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_NONE );
	}

	//-----------------------------------------------------
	void GraphicalWorld::render( real timeElapsed )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		msCore->update( timeElapsed );
	}

	//-----------------------------------------------------
	IMeshGeometryAccess* GraphicalWorld::createProceduralMesh(const String & name)
	{
		IMeshGeometryAccess* pMGA = getProceduralMesh( name );
		if (pMGA)
			return 0;

		Ogre::MeshPtr pMesh = Ogre::Root::getSingleton().getMeshManager()->createManual(
										name.c_str(), "yakeGroup" /*@todo user another group*/ );
		YAKE_ASSERT( !pMesh.isNull() );
		//@todo deprecated? pMesh->setManuallyDefined( true );
		pMesh->setIndexBufferPolicy( Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );
		pMesh->setVertexBufferPolicy( Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

		pMGA = new OgreMeshGeometryAccess( pMesh );
		YAKE_ASSERT( pMGA );
		return pMGA;
	}

	//-----------------------------------------------------
	void GraphicalWorld::destroyProceduralMesh(const String & name)
	{
	}

	//-----------------------------------------------------
	IMeshGeometryAccess* GraphicalWorld::getProceduralMesh(const String & name)
	{
		ProcMeshMap::iterator itFind = mProcMeshes.find( name );
		if (itFind == mProcMeshes.end())
			return 0;
		return new OgreMeshGeometryAccess(itFind->second);
	}

	//-----------------------------------------------------
	real GraphicalWorld::getRenderWindowWidth() const
	{
		YAKE_ASSERT( msCore );
		Ogre::RenderWindow* pWin = msCore->getRenderWindow();
		YAKE_ASSERT( pWin );
		return pWin->getWidth();
	}

	//-----------------------------------------------------
	real GraphicalWorld::getRenderWindowHeight() const
	{
		YAKE_ASSERT( msCore );
		Ogre::RenderWindow* pWin = msCore->getRenderWindow();
		YAKE_ASSERT( pWin );
		return pWin->getHeight();
	}


} // ogre3d
} // graphics
} // yake
