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
// scriptingLua.cpp : Defines the entry point for the DLL application.
//

#include <plugins/yakeGraphicsOgre/yake/pch.h>
#include <plugins/yakeGraphicsOgre/yake/graphicsOgreCore.h>
#include <plugins/yakeGraphicsOgre/yake/GraphicsSystemOgre.h>
#include <plugins/yakeGraphicsOgre/yake/plugin.h>

//---------------------------------------------------------
// This is an example of an exported function.
yake::base::Plugin* dynlibStartPlugin(void)
{
	return new OgrePlugin();
}

//---------------------------------------------------------
OgrePlugin::OgrePlugin() : mOgreCore(0)
{
}

//---------------------------------------------------------
OgrePlugin::~OgrePlugin()
{
	YAKE_SAFE_DELETE(mOgreCore);
}

//---------------------------------------------------------
yake::base::String OgrePlugin::getName() const
{
	return yake::base::String("yake.graphics.ogre");
}

//---------------------------------------------------------
yake::base::Version OgrePlugin::getVersion() const
{
	return yake::base::Version( 0, 1, 0 );
}

//---------------------------------------------------------
bool OgrePlugin::initialise()
{
	mOgreCore = new yake::graphics::OgreCore();
	//mOgreCore->update( 0. ); // Force rendering a frame!
    return (mOgreCore != 0);
}

//---------------------------------------------------------
bool OgrePlugin::shutdown()
{
	YAKE_SAFE_DELETE(mOgreCore);
	return true;
}

//---------------------------------------------------------
yake::graphics::GraphicsSystem* OgrePlugin::createSystem()
{
	YAKE_ASSERT( mOgreCore ).debug("No graphics core - no graphics!");
	return new yake::graphics::GraphicsSystemOgre( mOgreCore );
}