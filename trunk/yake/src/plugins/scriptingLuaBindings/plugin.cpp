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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/

#include <inc/plugins/scriptingLuaBindings/pch.h>
#include <inc/plugins/scriptingLuaBindings/yakeBinder.h>

using namespace yake;
using namespace yake::base;
using namespace yake::base::templates;

//---------------------------------------------------------
// This is an example of an exported function.
yake::base::Plugin* dynlibStartPlugin(void)
{
	return new LuaBindingsPlugin();
}

//---------------------------------------------------------
LuaBindingsPlugin::LuaBindingsPlugin()
{
}

//---------------------------------------------------------
LuaBindingsPlugin::~LuaBindingsPlugin()
{
}

//---------------------------------------------------------
yake::base::String LuaBindingsPlugin::getName() const
{
	return yake::base::String("yake.scripting.lua.bindings");
}

//---------------------------------------------------------
/*yake::version LuaBindingsPlugin::getVersion() const
{
	return yake::version( 0, 1, 0 );
}*/

//---------------------------------------------------------
bool LuaBindingsPlugin::initialise()
{
	return true;
}

//---------------------------------------------------------
bool LuaBindingsPlugin::shutdown()
{
	return true;
}

//---------------------------------------------------------
SharedPtr<scripting::IBinder> LuaBindingsPlugin::createBinder()
{
	return SharedPtr<scripting::IBinder>( new scripting::binder::lua::Binder() );
}