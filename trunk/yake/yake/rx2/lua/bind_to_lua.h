#ifndef _BIND_TO_LUA_H_
#define _BIND_TO_LUA_H_

// lua
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <luabind/luabind.hpp>
#include <luabind/return_reference_to_policy.hpp>

// boost
#include <boost/function.hpp>

// rx
#include "meta_class.h"
#include "lua_handler_to_cpp.h"

namespace rx
{
namespace lua
{

namespace
{
	void bind_mop( lua_State & L )
	{
		using namespace rx;
		using namespace luabind;

		module(&L)
		[ class_<meta_field>("meta_field")
				.enum_("flags")
				[
					value("none",					1),
					value("save",					2),
					value("load",					4),
					value("replicate",		8),
					value("uncompressed", 16),
					value("compressed",		32),
					value("copy",					64),
					value("interpolate",	128),
					value("server",				256),
					value("client",				512)
				],
			class_<int_field>("int_field")
				.def("get", &int_field::get )
				.def("set", &int_field::set ),
			class_<meta_object>("meta_object") 	
				.def(constructor<std::string>())
				.def("add_field_int", (meta_object& (meta_object::*)(std::string, int)) &meta_object::add_field<int, none>, return_reference_to(self))
				.def("add_field_int", (meta_object& (meta_object::*)(std::string, int, int)) &meta_object::add_field<int>, return_reference_to(self))
				.def("field_int",	(int_field& (meta_object::*)(std::string) const) &meta_object::field<int> )
				.def("get_handler",	&meta_object::get_handler ),
			class_<meta_class>("meta_class") 	
				.def(constructor<std::string>())
				.def("add_field_int", (meta_class& (meta_class::*)(std::string, int)) &meta_class::add_field<int>, return_reference_to(self))
				.def("add_field_int", (meta_class& (meta_class::*)(std::string, int, int)) &meta_class::add_field<int>, return_reference_to(self))
				.def("field_int",	(int_field& (meta_class::*)(std::string) const) &meta_class::field<int> ),
			def("instance", &instance),
			def("get_object", &get_object)
		];
	}

	void bind_events( lua_State & L )
	{
		using namespace rx;
		using namespace rx::lua;
		using namespace luabind;

		/*module(&L)
		[ 
			class_<boost::function_base>("function_base"),
			class_<event_base>("event_base")
				.def("attach_handler", &event_base::attach_handler),
			def("lua_handler_to_cpp_0", &lua_handler_to_cpp<0>),
			def("lua_handler_to_cpp_1", &lua_handler_to_cpp<1>),
			def("lua_handler_to_cpp_2", &lua_handler_to_cpp<2>)
		];*/
	}

} // unnamed namespace

void bind_to_lua( lua_State & L )
{
	bind_mop( L );
	bind_events( L );
}

} // namespace lua
} // namespace rx

#endif // _BIND_TO_LUA_H_