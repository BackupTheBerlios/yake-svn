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
#include <inc/plugins/graphicsOgre/yakePCH.h>
#include <inc/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <inc/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <inc/plugins/graphicsOgre/graphicsOgreNode.h>
#include <inc/plugins/graphicsOgre/graphicsOgreCamera.h>
#include <inc/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <inc/plugins/graphicsOgre/graphicsOgreViewport.h>
#include <inc/plugins/graphicsOgre/graphicsOgreLight.h>
#include <inc/plugins/graphicsOgre/graphicsOgreCore.h>

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

} // ogre3d
} // graphics
} // yake