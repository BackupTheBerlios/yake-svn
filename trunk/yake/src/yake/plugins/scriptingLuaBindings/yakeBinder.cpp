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
#include <yake/plugins/scriptingLuaBindings/pch.h>
#include <yake/plugins/scriptingLuaBindings/yakeBinder.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>

namespace luabind {
namespace converters {

	yes_t is_user_defined(by_value<yake::base::String>);
	yes_t is_user_defined(by_const_reference<yake::base::String>);

	void convert_cpp_to_lua(lua_State* L, const yake::base::String& s)
	{
		lua_pushstring(L, s.c_str());
	}

	yake::base::String convert_lua_to_cpp(
	lua_State* L, by_value<yake::base::String>, int index)
	{
		return yake::base::String(lua_tostring(L, index));
	}

	yake::base::String convert_lua_to_cpp(
	lua_State* L, by_const_reference<yake::base::String>, int index)
	{
		return yake::base::String(lua_tostring(L, index));
	}

	int match_lua_to_cpp(
	lua_State* L, by_value<yake::base::String>, int index)
	{
		if (lua_type(L, index) == LUA_TSTRING) return 0;
		else return -1;
	}

	int match_lua_to_cpp(
	lua_State* L, by_const_reference<yake::base::String>, int index)
	{
		if (lua_type(L, index) == LUA_TSTRING) return 0;
		else return -1;
	}

}
}


namespace yake {
namespace scripting {
namespace binder {
namespace lua {

	struct Player
	{
		String	mName;

		void setName( const String & name )
		{ mName = name; }
		String getName() const
		{ return mName; }
	};


	void Binder::bind(scripting::IVM* pVM)
	{
		YAKE_ASSERT( pVM );
		if (!pVM)
			return;

		LuaVM* pLuaVM = static_cast<LuaVM*>(pVM);
		YAKE_ASSERT( pLuaVM );

		using namespace luabind;

		luabind::open( pLuaVM->getLuaState() );
		module(pLuaVM->getLuaState())
		[
			class_<Version>("Version")
				.def(constructor<uint16,uint16,uint16>())
				.def("major", &yake::base::Version::major)
		];

		module(pLuaVM->getLuaState(), "yake")
		[
			class_<Player>("Player")
				.def(constructor<>())
				.def("getName", &Player::getName)
				.def("setName", &Player::setName)
		];
	}


}
}
}
}