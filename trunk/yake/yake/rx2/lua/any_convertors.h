#ifndef _ANY_CONVERTORS_H_
#define _ANY_CONVERTORS_H_

#include <map>
#include <string>

#include "static_initor.h"

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#define LUABIND_NO_HEADERS_ONLY

#include <luabind/luabind.hpp>
#include <boost/any.hpp>

namespace rx
{
namespace lua
{

template<class T>
struct convert_any
{
	static void convert(lua_State* L, const boost::any& a)
	{
		typename luabind::detail::default_policy::template generate_converter<T, luabind::detail::cpp_to_lua>::type conv;

		conv.apply(L, *boost::any_cast<T>(&a));
	}
};

std::map<const std::type_info*, void(*)(lua_State*, const boost::any&)> any_converters;

template<class T>
void register_any_converter()
{
	any_converters[&typeid(T)] = convert_any<T>::convert;
}

} // namespace lua
} // namespace rx

namespace luabind
{
	namespace converters
	{
		yes_t is_user_defined(by_value<boost::any>);
		yes_t is_user_defined(by_const_reference<boost::any>);
		yes_t is_user_defined(by_reference<boost::any>);

		void convert_cpp_to_lua(lua_State * L, const boost::any & a)
		{
			using namespace rx::lua;
			typedef void(*conv_t)(lua_State * L, const boost::any&);
			conv_t conv = any_converters[&a.type()];
			conv(L, a);
		}

		bool convert_lua_to_cpp(lua_State * L, boost::any, int index)
		{
			using namespace rx::lua;
			detail::default_policy::generate_converter<bool, detail::lua_to_cpp>::type converter;
			return converter.apply(L, LUABIND_DECORATE_TYPE(bool), index);
		}

		int match_lua_to_cpp(lua_State * L, boost::any, int index)
		{
			using namespace rx::lua;
			typedef detail::default_policy::generate_converter<bool, detail::lua_to_cpp>::type converter_t;
			return converter_t::match(L, LUABIND_DECORATE_TYPE(bool), index);
		}

	}
}

namespace
{

void register_any_convertors()
{
	register_any_converter<int>();
	register_any_converter<float>();
	register_any_converter<bool>();
	register_any_converter<std::string>();
}

} // unnamed namespace

STATIC_INITOR_FNC1( register_any_convertors )

#endif // _ANY_CONVERTORS_H_