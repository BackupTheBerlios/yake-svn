#ifndef _LUA_HANDLER_TO_CPP_H_
#define _LUA_HANDLER_TO_CPP_H_

#include <vector>
#include <algorithm>

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#define LUABIND_NO_HEADERS_ONLY
#include <luabind/luabind.hpp>
#include <luabind/functor.hpp>

#include <boost/any.hpp>
#include <boost/function.hpp>

namespace rx
{
namespace lua
{

namespace
{
	template< int arguments >
	struct args_dispatch
	{
	};

	template<>
	struct args_dispatch<0>
	{
    struct forward_call
		{
			forward_call( const luabind::functor<void> & lua_function )
				: lua_functor_( lua_function ) {}

			void on_forward_call()
			{ lua_functor_();	}

			luabind::functor<void> lua_functor_;
		};

		static boost::function_base & lua_function_to_cpp( const luabind::functor<void> & lua_function )
		{
			forward_call * forward = new forward_call( lua_function );
			return *(new boost::function<void()>(boost::bind(&forward_call::on_forward_call, forward)));
		}
	};

	template<>
	struct args_dispatch<1>
	{
    struct forward_call
		{
			forward_call( const luabind::functor<void> & lua_function )
				: lua_functor_( lua_function ) {}

			void on_forward_call( boost::any arg1 )
			{ lua_functor_( arg1 );	}

			luabind::functor<void> lua_functor_;
		};

		static boost::function_base & lua_function_to_cpp( const luabind::functor<void> & lua_function )
		{
			forward_call * forward = new forward_call( lua_function );
			return *(new boost::function<void(boost::any)>(boost::bind(&forward_call::on_forward_call, forward, _1 )));
		}
	};

	template<>
	struct args_dispatch<2>
	{
    struct forward_call
		{
			forward_call( const luabind::functor<void> & lua_function )
				: lua_functor_( lua_function ) {}

			void on_forward_call( boost::any arg1, boost::any arg2 )
			{ lua_functor_( arg1, arg2 );	}

			luabind::functor<void> lua_functor_;
		};

		static boost::function_base & lua_function_to_cpp( const luabind::functor<void> & lua_function )
		{
			forward_call * forward = new forward_call( lua_function ); // todo delete
			return *(new boost::function<void(boost::any, boost::any)>(boost::bind( 
				&forward_call::on_forward_call, forward, _1, _2)));
		}
	};

} // unnamed namespace

static std::vector<const boost::function_base*> is_lua_handler;

template< int arguments >
boost::function_base & lua_handler_to_cpp( const luabind::functor<void> & lua_function )
{
	boost::function_base & cpp_func = typename args_dispatch< arguments >::lua_function_to_cpp( lua_function );
	is_lua_handler.push_back( &cpp_func );
  return cpp_func;
}

// event handler cast policy
template< typename arg1 >
struct search_for_lua_handlers_1
{
	typedef boost::function< void (arg1) > result_cpp_functor;
	typedef boost::function< void (boost::any) > result_lua_functor;

	static result_cpp_functor cast( boost::function_base & function )
	{
		// we cannot use the ternary operator ?: (conditional) here
		if( std::find( is_lua_handler.begin(), is_lua_handler.end(), &function ) != is_lua_handler.end() )
			return static_cast< result_lua_functor & >( function );
		else
			return static_cast< result_cpp_functor & >( function );
	}
};

template< typename arg1, typename arg2 >
struct search_for_lua_handlers_2
{
	typedef boost::function< void (arg1, arg2) > result_cpp_functor;
	typedef boost::function< void (boost::any, boost::any) > result_lua_functor;

	static result_cpp_functor cast( boost::function_base & function )
	{
		// we cannot use the ternary operator ?: (conditional) here
    if( std::find( is_lua_handler.begin(), is_lua_handler.end(), &function ) != is_lua_handler.end() )
			return static_cast< result_lua_functor & >( function );
		else
			return static_cast< result_cpp_functor & >( function );
	}
};

} // namespace lua
} // namespace rx

#endif // _LUA_HANDLER_TO_CPP_H_