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
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsRegister.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {
namespace ogre3d {

	GraphicsSystem::GraphicsSystem() : mCore(0)
{
}

GraphicsSystem::~GraphicsSystem()
{
	YAKE_SAFE_DELETE( mCore );
}

bool strMapContains( const GraphicsSystem::ParamMap& container, const String& value )
{
	return (container.end() != container.find(value));
}
void GraphicsSystem::initialise(const ParamMap& rParams) throw(GraphicsException)
{
	YAKE_ASSERT( !mCore );
	if (mCore)
		YAKE_GRAPHICS_EXCEPT("Core already initialised!");

	bool bWindowAlreadyCreated = false;
	bool bShutdownOgre = true;
	bool bParseDefaultResourceFile = true;
	Ogre::RenderWindow* pRenderWindow = 0;

	if (strMapContains(rParams,"windowAlreadyCreated"))
	{
		bWindowAlreadyCreated = base::StringUtil::parseBool( rParams.find("windowAlreadyCreated")->second );
		const base::String test = rParams.find("renderWindowPtr")->second;
		int p = base::StringUtil::parseInt( test );
		pRenderWindow = reinterpret_cast<Ogre::RenderWindow*>(p);
	}
	if (strMapContains(rParams,"shutdownOgre"))
	{
		bShutdownOgre = base::StringUtil::parseBool( rParams.find("shutdownOgre")->second );
	}
	if (strMapContains(rParams,"parseDefaultResourceFile"))
	{
		bParseDefaultResourceFile = base::StringUtil::parseBool( rParams.find("parseDefaultResourceFile")->second );
	}

	mCore = new OgreCore(bWindowAlreadyCreated,bShutdownOgre,bParseDefaultResourceFile,pRenderWindow);
	YAKE_ASSERT( mCore );
	GraphicalWorld::setCore( mCore );
}
void GraphicsSystem::shutdown() throw(GraphicsException)
{
	YAKE_SAFE_DELETE( mCore );
}

SharedPtr<IWorld> GraphicsSystem::createWorld()
{
	return SharedPtr<IWorld>( new GraphicalWorld() );
}

void GraphicsSystem::subscribeToShutdownSignal( const ShutdownSignal::slot_type& rSlot )
{
	mCore->subscribeToShutdownSignal( rSlot );
}

} // ogre3d
} // graphics
} // yake
