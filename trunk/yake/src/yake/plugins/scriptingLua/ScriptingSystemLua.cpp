/*
   ------------------------------------------------------------------------------------
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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#include <yake/plugins/scriptingLua/yakePCH.h>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define LUABIND_NO_HEADERS_ONLY
#include <dependencies/luabind/luabind/luabind.hpp>

#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <fstream>

namespace yake {
namespace scripting {

	//-----------------------------------------------------------------------
	YAKE_REGISTER_CONCRETE( ScriptingSystemLua );

	//-----------------------------------------------------------------------
	#define IS_MASK_SET(val, mask) ((mask & val) == mask)

	//------------------------------------------------------
	LuaVM::LuaVM( ScriptingSystemLua* pSystem, uint32 libs ) : mCreator( pSystem )//, mPropInterface( this )
	{
		mLuaState = lua_open();

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

		luabind::open( mLuaState );
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
	void LuaVM::execute( const String & rData )
	{
		if (mLuaState)
			lua_dostring( mLuaState, rData.c_str() );
	}

	//------------------------------------------------------
	void LuaVM::execute( scripting::IScript* pScript )
	{
		if ( ( pScript->getCreator()->getLanguage() != scripting::IScriptingSystem::L_LUA ) || 
			( 0 == mLuaState ) )
			return;

		LuaScript* pLuaScript = static_cast< LuaScript* > ( pScript );

		const String& rFileName = pLuaScript->getData();

		lua_dofile( mLuaState, rFileName.c_str() ); 
	}

	//------------------------------------------------------
	ScriptingSystemLua::ScriptingSystemLua()
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
	scripting::IScript* ScriptingSystemLua::createScriptFromFile( const String & rFile )
	{
		LuaScript* pLuaScript = new LuaScript( this );

		pLuaScript->setData( rFile );

		return pLuaScript;
	}

	//------------------------------------------------------
	LuaScript::LuaScript( scripting::IScriptingSystem* pCreator ) : mCreator( pCreator )
	{
		YAKE_ASSERT( mCreator ).debug( "creator scripting system undefined!" );
	}

	//------------------------------------------------------
	LuaScript::~LuaScript()
	{
	}

	//------------------------------------------------------
	const IScriptingSystem* LuaScript::getCreator() const 
	{
		return mCreator;
	}

	//------------------------------------------------------
	bool LuaScript::isLoaded() 
	{
		return mFileName.size() != 0;
	}
		
	//------------------------------------------------------
	void LuaScript::setData( const String& rData )
	{
		mFileName = rData;
	}

	//------------------------------------------------------
	const String& LuaScript::getData() const 
	{
		return mFileName;
	}
}
}
