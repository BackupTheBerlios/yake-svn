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
#include <yapp/plugins/entLua/pch.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <yapp/plugins/entLua/yakeLuaBinder.h>
#include <yapp/plugins/entLua/yakeEvent_lua.h>
#include <yapp/plugins/entLua/yakeEntLua.h>
#include <yapp/plugins/entLua/yakeStateCb_lua.h>
#include <luabind/luabind.hpp>

namespace yake {
namespace ent {
namespace lua {

	bool executeEntityStateMachineCb(ent::Entity* pEntity, scripting::IVM* pVM, lua_State* L,
									const String& machineName,
									const String& stateName,
									const String& fn)
	{
		try {
			//std::cout << "executeEntityEvent(\"" << evtName.c_str() << "\")\n";
			luabind::object machineTbl = (machineName == "default") ? luabind::get_globals(L) : luabind::get_globals(L)["machine_"+machineName];
			if (!machineTbl.is_valid() || machineTbl.type() != LUA_TTABLE)
				throw "machine table is not valid";

			luabind::object statesTbl = machineTbl["states"];
			if (!statesTbl.is_valid() || statesTbl.type() != LUA_TTABLE)
				throw "states table is not valid";

			luabind::object stateTbl = statesTbl[stateName];
			if (!stateTbl.is_valid() || stateTbl.type() != LUA_TTABLE)
				throw "state table is not valid";

			//luabind::object fnTbl = pEntity->isServer() ? stateTbl.at("server") : stateTbl.at("client");
			luabind::object fnTbl = stateTbl["server"];
			if (!fnTbl.is_valid() || fnTbl.type() != LUA_TTABLE)
				throw "state functions table is not valid";

			luabind::object fnObj = fnTbl[ fn.c_str() ];
			if (!fnObj.is_valid() || fnObj.type() != LUA_TFUNCTION) // function defined?
				return true;

			luabind::call_member<void>( fnTbl, fn.c_str() );
			return true;
		}
		catch (luabind::error& e)
		{
			std::cout << "\nCATCHED Luabind EXCEPTION:\n" << e.what() << "\n";
			return false;
		}
		catch (const char* msg)
		{
			std::cout << "\nCATCHED (...) EXCEPTION:\n" << msg << "\n";
			return false;
		}
	}
	void EntityMachineStateCallbackLua::onEnter()
	{
		YAKE_ASSERT( mOwnerEntity && !mOwnerMachine.empty() && mpVM );
		if (mOwnerEntity && mpVM && !mOwnerMachine.empty())
			executeEntityStateMachineCb( mOwnerEntity, mpVM, mpVM->getLuaState(), mOwnerMachine, mId, "enter" );
	}
	void EntityMachineStateCallbackLua::onExit()
	{
		YAKE_ASSERT( mOwnerEntity && !mOwnerMachine.empty() && mpVM );
		if (mOwnerEntity && mpVM && !mOwnerMachine.empty())
			executeEntityStateMachineCb( mOwnerEntity, mpVM, mpVM->getLuaState(), mOwnerMachine, mId, "exit" );
	}
	void EntityMachineStateCallbackLua::onStep()
	{
		YAKE_ASSERT( mOwnerEntity && !mOwnerMachine.empty() && mpVM );
		if (mOwnerEntity && mpVM && !mOwnerMachine.empty())
			executeEntityStateMachineCb( mOwnerEntity, mpVM, mpVM->getLuaState(), mOwnerMachine, mId, "step" );
	}

} // namespace lua
} // namespace ent
} // namespace yake
