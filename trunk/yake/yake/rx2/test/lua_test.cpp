#pragma warning(disable: 4305) // int to unsigned short
#pragma warning(disable: 4309) // contraction const value
#pragma warning(disable: 4244) // __w64 int to int
#pragma warning(disable: 4267) // site_t to unsigned int
#pragma warning(disable: 4800) // bits AND => bool

// stl
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

// boost
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

// mop
#include "meta_class.h"
#include "class_registry.h"

// scripting
#include "lua/bind_to_lua.h"
#include "lua/lua_handler_to_cpp.h"
#include "lua/any_convertors.h"
#include "lua/do_string.h"

/* ### calling function with any argument from lua ### */
std::string fnc_has_any_arg( boost::any test )
{
	std::cout << test.type().name() << " " << 
		boost::lexical_cast<bool>( boost::any_cast<bool>( test ) ) << " ";
	assert(boost::any_cast<bool>(test));
	return boost::any_cast<bool>(test) ? "ok" : "no";
}

int main()
{
	using namespace rx;
	using namespace rx::lua;

	// lua bindings
	std::cout << "[ lua bindings ]" << std::endl;	

	// open
	lua_State * L = lua_open();
	lua_baselibopen(L);
	luaopen_io(L);
	luabind::open(L);
	lua::bind_to_lua(*L);

	// binding calling lua functions with any (any convertors)
	{
		// calling c++ function with any argument
		luabind::function(L, "fnc_has_any_arg", &fnc_has_any_arg);
		do_string(L, "print( fnc_has_any_arg(true) )");

		// calling lua function from cpp with any arguments
		do_string(L, "function update(p) print(p) end");
		boost::any param = std::string("update(foo)");
		luabind::call_function<void>(L, "update", param);
	}

	// callback/event handler with n any arguments
	{

		// define class
		meta_class test_class =
			define<meta_class>( "test_class" )
				.add_event<bool>( "click" );

		get_class( "test_class" );

		// lua handler
		do_string(L, "function do_it_in_script( arg1 ) print( arg1 ) end");
		lua_dostring(	L, "meta_obj = instance( get_class('test_class'), 'hello_object')" );
		lua_dostring(	L, "meta_obj:get_event( 'click' ):attach_handler( lua_function_to_cpp_1( do_it_in_script ) )" );

		// test
		get_object( "hello_object" ).get_event<bool>( "click" )( true );

		// c++ handler
		/*boost::function< void( const std::string&, int ) > fnc = boost::bind( &cpp_handler, _1, _2 );
		cpp_e.set( fnc );

		// test
		cpp_e.cpp_handler( "hellooo2", 123 );*/
	}
  
	lua_close(L);

	std::cin.get();

	return 0;
}