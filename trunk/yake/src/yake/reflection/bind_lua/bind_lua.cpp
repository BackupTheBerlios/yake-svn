#include "bind_lua/bind_lua.h"

// -----------------------------------------
// lua utility
bool dostring(lua_State * L, const char * str)
{
	if (luaL_loadbuffer(L, str, std::strlen(str), str) || lua_pcall(L, 0, 0, 0))
	{
		const char * a = lua_tostring(L, -1);
		std::cout << a << "\n";
		lua_pop(L, 1);
		return true;
	}
	return false;
}

// -----------------------------------------
// lua state
global_lua_state::holder * global_lua_state::m_holder = NULL;

// -----------------------------------------
// lua event bindings
namespace
{
	static void register_lua_event_base()
	{
		using namespace luabind;
		module(L)
		[ 
			class_<boost::function_base>("function_base"),
			class_<reflection::lua_event_base>("lua_event_base")
				.def("attach_handler", &reflection::lua_event_base::attach_handler)
		];
	}
	STATIC_INIT(register_lua_event_base)
}