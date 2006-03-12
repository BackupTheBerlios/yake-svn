/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#include "yake/base/yake.h"
#include "yake/plugins/entLua/entLua.h"
#include "yake/plugins/scriptingLua/ScriptingSystemLua.h"

#define LUABIND_NO_HEADERS_ONLY
#include <luabind/luabind.hpp>

namespace yake {
namespace ent {
	namespace detail {
		void executeOfTable_2(scripting::IVM& vm, const std::string& tbl1name, const std::string& tbl2name, const std::string& fn)
		{
			try {
				scripting::LuaVM& luaVM = static_cast<scripting::LuaVM&>(vm);
				lua_State* luaState = luaVM.getLuaState();
				YAKE_ASSERT( luaState );

				luabind::object tbl1 = luabind::globals(luaState)[tbl1name.c_str()];
				if (luabind::type(tbl1) == LUA_TNIL)
					//if (!tbl1)
				{
					std::cerr << "vm error: could not find table '" << tbl1name << "'.\n";
					return;
				}
				assert( luabind::type(tbl1) == LUA_TTABLE );

				luabind::object tbl2 = tbl1[tbl2name.c_str()];
				if (luabind::type(tbl2) == LUA_TNIL)
					//if (!tbl2)
				{
					std::cerr << "vm error: could not find table '" << tbl1name << "." << tbl2name << "'.\n";
					return;
				}
				assert( luabind::type(tbl2) == LUA_TTABLE );

				luabind::object fnObj = tbl2[fn.c_str()];
				assert( luabind::type(fnObj) == LUA_TFUNCTION );

				fnObj();
			}
			catch (...)
			{
				YAKE_EXCEPT("executeOfTable_2(): Bad cast! VM is not a Lua VM!");
			}
		}
	}

	LuaFsmObjectListener::LuaFsmObjectListener(scripting::IScriptingSystem& scriptingSystem) :
		scriptingSystem_(scriptingSystem)
	{
		startScript_.reset( scriptingSystem_.createScriptFromFile("../../media/scripts/o_fsm_test.lua") );
		YAKE_ASSERT( startScript_.get() );
	}
	void LuaFsmObjectListener::onInit(Object& obj)
	{
		Entity* ent = Entity::cast(&obj);
		if (ent)
		{
			YAKE_ASSERT( ent->getFsmVM() );
			ent->getFsmVM()->execute("print('initializing...')");
			ent->getFsmVM()->execute( startScript_.get() );

			ent->getFsmVM()->execute("c = coroutine.create(main)\ncoroutine.resume(c,1)");
		}
	}
	void LuaFsmObjectListener::onTick(Object& obj)
	{
		Entity* ent = Entity::cast(&obj);
		if (ent)
		{
			ent->getFsmVM()->execute("event.on_tick()");
			ent->getFsmVM()->execute("state." + ent->getCurrentFsmState() + ".on_tick()");
		}
	}
	void LuaFsmObjectListener::onFsmEventHandled(Object& obj, const object_fsm&, const object_fsm::event_type& evt)
	{
		Entity* ent = Entity::cast(&obj);
		if (ent)
		{
			if (evt == "spawn")
				ent->getFsmVM()->execute("event.on_spawn()");
			else if (evt == "die")
				ent->getFsmVM()->execute("event.on_die()");
		}
	}
	void LuaFsmObjectListener::onFsmEnterState(Object& obj,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type& state)
	{
		Entity* ent = Entity::cast(&obj);
		if (ent)
		{
			scripting::IVM* vm = ent->getFsmVM();
			YAKE_ASSERT( vm );
			detail::executeOfTable_2(*vm,"state",state,"on_enter");
		}
	}
	void LuaFsmObjectListener::onFsmExitState(Object& obj,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type& state)
	{
		Entity* ent = Entity::cast(&obj);
		if (ent)
		{
			scripting::IVM* vm = ent->getFsmVM();
			YAKE_ASSERT( vm );
			detail::executeOfTable_2(*vm,"state",state,"on_exit");
		}
	}
	LuaObjectManagerListener::LuaObjectManagerListener(scripting::IScriptingSystem& scriptingSystem) :
		fsmListener_(scriptingSystem)
	{
	}
	void LuaObjectManagerListener::onObjectCreated(Object* obj)
	{
		Entity* ent = Entity::cast(obj);
		if (ent)
		{
			YAKE_ASSERT( ent );
			ent->attachListener( &fsmListener_, "lua_listener", ObjectListenerManager::kKeepOwnership );
		}
	}
} // namespace ent
} // namespace yake

