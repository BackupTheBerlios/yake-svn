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
#include <dependencies/ogrenew/OgreMain/include/OgreMeshManager.h>
#include <dependencies/ogrenew/OgreMain/include/OgreNoMemoryMacros.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <yake/plugins/graphicsOgre/graphicsOgreNode.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <yake/plugins/graphicsOgre/graphicsOgreViewport.h>
#include <yake/plugins/graphicsOgre/graphicsOgreLight.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>
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
	IEntity* GraphicalWorld::createEntity( const base::String & mesh )
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
		return new OgreViewport( msCore, static_cast<OgreCamera*>( pCamera ) );
	}

	//-----------------------------------------------------
	void GraphicalWorld::setShadowsEnabled( bool enabled )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		Ogre::SceneManager* pSceneMgr = msCore->getSceneMgr();;
		YAKE_ASSERT( pSceneMgr ).debug("need a scene manager!");
		if (enabled)
			pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );
			//pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_MODULATIVE );
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

		Ogre::Mesh* pMesh = Ogre::Root::getSingleton().getMeshManager()->createManual( name.c_str() );
		YAKE_ASSERT( pMesh );
		pMesh->setManuallyDefined( true );
		pMesh->setIndexBufferPolicy( Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );
		pMesh->setVertexBufferPolicy( Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

		pMGA = new OgreMeshGeometryAccess( pMesh );
		YAKE_ASSERT( pMGA );
		return pMGA;
	}

	//-----------------------------------------------------
	void GraphicalWorld::destroyProceduralMesh(const base::String & name)
	{
	}

	//-----------------------------------------------------
	IMeshGeometryAccess* GraphicalWorld::getProceduralMesh(const base::String & name)
	{
		ProcMeshMap::const_iterator itFind = mProcMeshes.find( name );
		if (itFind == mProcMeshes.end())
			return 0;
		return new OgreMeshGeometryAccess(itFind->second);
	}



} // ogre3d
} // graphics
} // yake