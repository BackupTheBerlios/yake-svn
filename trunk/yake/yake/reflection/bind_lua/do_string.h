#ifndef _DO_STRING_H
#define _DO_STRING_H

#include <iostream>

namespace yake
{
namespace reflection 
{
namespace lua
{

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

bool do_string(lua_State* L, const char* str)
{
	if (luaL_loadbuffer(L, str, std::strlen(str), str) || lua_pcall(L, 0, 0, 0))
	{
		const char* a = lua_tostring(L, -1);
		std::cout << a << "\n";
		lua_pop(L, 1);
		return true;
	}
	return false;
}

} // namespace lua
} // namespace reflection
} // namespace yake

#endif // _DO_STRING_H