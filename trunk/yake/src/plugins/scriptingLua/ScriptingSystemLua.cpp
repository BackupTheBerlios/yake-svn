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
#include <yakeScriptingSystem.h>
extern "C" {
#include <dependencies/lua/include/lua.h>
#include <dependencies/lua/include/lualib.h>
#include <dependencies/lua/include/lauxlib.h>
}
#define LUABIND_NO_HEADERS_ONLY
#include <dependencies/luabind/luabind/luabind.hpp>
#include "../inc/ScriptingSystemLua.h"

namespace yake {
namespace scripting {

	//-----------------------------------------------------------------------
	#define IS_MASK_SET(val, mask) ((mask & val) == mask)

	//------------------------------------------------------
	LuaVM::LuaVM( ScriptingSystemLua* pSystem, uint32 libs ) : mCreator( pSystem )//, mPropInterface( this )
	{
		mLuaState = lua_open();
		luabind::open( mLuaState );

		if (IS_MASK_SET(libs, LUALIB_BASE))
			luaopen_base( mLuaState );
		if (IS_MASK_SET(libs, LUALIB_TABLE))
			luaopen_table( mLuaState );
		if (IS_MASK_SET(libs, LUALIB_IO))
			luaopen_io( mLuaState );
		if (IS_MASK_SET(libs, LUALIB_STRING))
			luaopen_string( mLuaState );
		if (IS_MASK_SET(libs, LUALIB_MATH))
			luaopen_math( mLuaState );
		if (IS_MASK_SET(libs, LUALIB_DEBUG))
			luaopen_debug( mLuaState );
	}

	//------------------------------------------------------
	LuaVM::~LuaVM()
	{
		if (mLuaState)
		{
			lua_close( mLuaState );
			mLuaState = 0;
		}
	}

	//------------------------------------------------------
	void LuaVM::execute( const String & data )
	{
		if (mLuaState)
			lua_dostring( mLuaState, data.c_str() );
	}

	//------------------------------------------------------
	ScriptingSystemLua::ScriptingSystemLua() : ScriptingSystem()
	{
	}

	//------------------------------------------------------
	ScriptingSystemLua::~ScriptingSystemLua()
	{
		mVMs.clear();
	}

	//------------------------------------------------------
	scripting::IVM* ScriptingSystemLua::createVM()
	{
		scripting::IVM* pVM = new LuaVM(this);
		YAKE_ASSERT( pVM ).debug("could not create scripting VM");
		mVMs.push_back( pVM );
		return pVM;
	}

	//------------------------------------------------------
	scripting::IScript* ScriptingSystemLua::createScriptFromFile( const String & file )
	{
		return 0;
	}

}
}