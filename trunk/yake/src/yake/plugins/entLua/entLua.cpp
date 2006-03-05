#include "yake/base/yake.h"
#include "yake/plugins/entLua/entLua.h"
#include "yake/plugins/scriptingLua/ScriptingSystemLua.h"

#define LUABIND_NO_HEADERS_ONLY
#include <luabind/luabind.hpp>

namespace yake {
namespace object {
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
		startScript_.reset( scriptingSystem_.createScriptFromFile("../o_test.lua") );
		YAKE_ASSERT( startScript_.get() );
	}
	void LuaFsmObjectListener::onInit(Object& obj)
	{
		//YAKE_ASSERT( obj.getFsmVM() );
		obj.getFsmVM()->execute("print('initializing...')");
		obj.getFsmVM()->execute( startScript_.get() );

		//std::cout << "vm: exec start_main()\n";
		//@todo make sure "main()" exists
		//obj.getVM().execute("start_main()");

		obj.getFsmVM()->execute("c = coroutine.create(main)\ncoroutine.resume(c,1)");
	}
	void LuaFsmObjectListener::onTick(Object& obj)
	{
		obj.getFsmVM()->execute("event.on_tick()");
		obj.getFsmVM()->execute("state." + obj.getCurrentFsmState() + ".on_tick()");
	}
	void LuaFsmObjectListener::onFsmEventHandled(Object& obj, const object_fsm&, const object_fsm::event_type& evt)
	{
		if (evt == "spawn")
			obj.getFsmVM()->execute("event.on_spawn()");
		else if (evt == "die")
			obj.getFsmVM()->execute("event.on_die()");
	}
	void LuaFsmObjectListener::onFsmEnterState(Object& obj,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type& state)
	{
		scripting::IVM* vm = obj.getFsmVM();
		YAKE_ASSERT( vm );
		detail::executeOfTable_2(*vm,"state",state,"on_enter");
	}
	void LuaFsmObjectListener::onFsmExitState(Object& obj,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type& state)
	{
		scripting::IVM* vm = obj.getFsmVM();
		YAKE_ASSERT( vm );
		detail::executeOfTable_2(*vm,"state",state,"on_exit");
	}
	LuaObjectManagerListener::LuaObjectManagerListener(scripting::IScriptingSystem& scriptingSystem) :
		fsmListener_(scriptingSystem)
	{
	}
	void LuaObjectManagerListener::onObjectCreated(Object* obj)
	{
		YAKE_ASSERT( obj );
		obj->attachListener( &fsmListener_, "lua_listener", object_listener_mgr::kKeepOwnership );
	}
}
} // namespace yake

