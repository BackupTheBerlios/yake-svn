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
#include <plugins/yakeInputOgre/inc/pch.h>
#include <yakeBase/inc/yake.h>
#include <yakeInput/inc/yakeInputSystem.h>
#include <plugins/yakeInputOgre/inc/InputSystemOgre.h>
#include <plugins/yakeInputOgre/inc/inputPlugin.h>
#include <new.h>
//---------------------------------------------------------
// This is an example of an exported function.
yake::base::Plugin* dynlibStartPlugin(void)
{
	return new OgreInputPlugin();
}

//---------------------------------------------------------
OgreInputPlugin::OgreInputPlugin()
{
	yake::input::InputSystem* i = new yake::input::InputSystemOgre();
	delete i;
}

//---------------------------------------------------------
OgreInputPlugin::~OgreInputPlugin()
{
}

//---------------------------------------------------------
yake::base::String OgreInputPlugin::getName() const
{
	return yake::base::String("yake.input.ogre");
}

//---------------------------------------------------------
yake::base::Version OgreInputPlugin::getVersion() const
{
	return yake::base::Version( 0, 1, 0 );
}

//---------------------------------------------------------
bool OgreInputPlugin::initialise()
{
	return true;
}

//---------------------------------------------------------
bool OgreInputPlugin::shutdown()
{
	return true;
}

//---------------------------------------------------------
yake::input::InputSystem* OgreInputPlugin::createSystem()
{
	return new yake::input::InputSystemOgre();
}