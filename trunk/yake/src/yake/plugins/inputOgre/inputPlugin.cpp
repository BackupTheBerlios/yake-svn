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
#include <yake/plugins/inputOgre/yakePCH.h>
#include <yake/base/yake.h>
#include <yake/input/yakeInputSystem.h>
#include <yake/plugins/inputOgre/InputSystemOgre.h>
#include <yake/plugins/inputOgre/inputPlugin.h>
#include <new>

using namespace std;

//---------------------------------------------------------
// This is an example of an exported function.
extern "C" yake::base::Plugin* dynlibStartPlugin(void)
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
