#ifndef _LUA_H_
#define _LUA_H_

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

bool dostring(lua_State* L, const char* str)
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

#define LUABIND_NO_HEADERS_ONLY
#include <luabind/luabind.hpp>


// Lua

/*
Each c++ class (foo) has static luabind class description (luabind::class_<foo>)
and these are stored within the c++ class as static member.
Due to the static initialization trick we can register the c++ class to luabind (class::register_lua_class),
but only the 'header' (i.e. the class name, type and constructor) because we don't
have access to the methods and properties (we would need a static container in the c++ class
holding all methods and that would mess up our METHOD etc. macros).

When we create an 'c++ object' in lua the member variables of the 'c++ object' are initialized
and at this point each member struct variable (represeting a property, method etc.) add the
needed binding information to luabind. 

So the whole process looks like this:
1. the global variable global_lua_state (L) is initialized and opens lua and luabind
2. the static initor defined by LUA_BIND_CLASS gets initialized and adds the class 'header'
	 to our static member within the c++ class using register_lua_class()
3. the client creates an object in lua and lua instances the according c++ class
4. c++ calls the constructor of each member variable, our registration struct constructor is called
5. the constructor of the member variable (registration struct) creates a static variable and its' constructor is called
   (so that we register the additional information just one time)
6. the static variable grabs the lua class information from our static member, adds its' 
	 own information (method pointer or getters/setters for properties)
	 and commits these changes to the actual lua metatable (luabind::crep)
7. the client can call the function or property now

o = LuaTest(); o.a=5; o:foo(); print(o.a);

Steps 1-2: initialization/registration of cleass headers in c++ at program startup
Step 3: create lua object => "o = LuaTest();"  !At this point the object o does _not_ have methods or properties
Step 4-6: c++ object is created => adding properties and methods to lua
Step 7: _now_ we can use the properties and methods within lua => "o.a=5; o:foo()"
*/

struct global_lua_state
{
	global_lua_state() : m_L(lua_open())
	{	// open lua
		lua_baselibopen(m_L);
		luabind::open(m_L);
	}

	~global_lua_state()
	{ lua_close(m_L); }

	lua_State * get()
	{	return m_L;	}

	operator lua_State*()
	{	return m_L;	}

	lua_State * m_L;
} L;

#define IMPLEMENT_LUA_CLASS(CLASS_NAME) \
static struct static_initor##CLASS_NAME \
{ \
	static_initor##CLASS_NAME() \
	{ \
		static int counter = 0;\
		if( counter++ > 0 ) return; \
		CLASS_NAME##::register_lua_class(); \
	} \
} static_init##CLASS_NAME;


#define DEFINE_LUA_CLASS(CLASS_NAME, SUPER_CLASS_NAME) \
protected: \
friend struct static_initor##CLASS_NAME;\
\
typedef CLASS_NAME class_type; \
\
static void register_lua_class() \
{ \
		/* class header with constructor */ \
		get_lua_class().def( luabind::constructor<>() ); \
		/* bind all class */ \
		luabind::module(L)[ get_lua_class() ]; \
} \
\
static luabind::class_<class_type> & get_lua_class() \
{ \
	static luabind::class_<CLASS_NAME> CLASS_NAME##_lua = luabind::class_<CLASS_NAME>(#CLASS_NAME); \
	return CLASS_NAME##_lua; \
} \
\
static luabind::detail::class_rep & get_lua_crep() \
{	return *luabind::detail::class_registry::get_registry(L)->find_class( get_lua_class().m_type );	} \
\
static void commit_lua_methods() \
{ \
	using namespace luabind; \
  /* get class rep */ \
	class_<class_type> & lua_class = get_lua_class(); \
	detail::class_rep & lua_crep = get_lua_crep(); \
\
	/* set reference to crep */ \
	for( std::map<const char*, detail::method_rep, detail::ltstr>::iterator iter( lua_class.m_methods.begin() ); \
		iter != lua_class.m_methods.end(); ++iter ) \
	{	iter->second.crep = &lua_crep; } \
\
	/* add to crep */ \
	lua_crep.m_methods = lua_class.m_methods; \
} \
\
static void commit_lua_properties() \
{ \
	using namespace luabind; \
  /* get class rep */ \
	class_<class_type> & lua_class = get_lua_class(); \
	detail::class_rep & lua_crep = get_lua_crep(); \
\
	/* copy getters and setters to crep */ \
	lua_crep.m_getters = lua_class.m_getters; \
	lua_crep.m_setters = lua_class.m_setters; \
}


#define LUA_METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS) \
private: \
struct register_method_##METHOD_NAME \
{ \
	struct initor \
	{ \
		initor() \
		{ \
			get_lua_class().def( #METHOD_NAME, &METHOD_NAME ); \
			commit_lua_methods(); \
		} \
	}; \
\
	register_method_##METHOD_NAME() \
	{ static initor init; } \
} reg_method_##METHOD_NAME;


#define LUA_DEFAULT_PROPERTY(TYPE, NAME) \
private: \
struct register_property_##NAME \
{ \
	struct initor \
	{ \
		initor() \
		{ \
			get_lua_class().property( #NAME, &get_##NAME, &set_##NAME ); \
			commit_lua_properties(); \
		} \
	}; \
\
	register_property_##NAME() \
	{ static initor init; } \
} reg_property_##NAME;


#define LUA_EVENT(ACCESS_ATTR, EVENT_NAME, ARGS) \
private: \
struct register_event_##EVENT_NAME \
{ \
	struct initor \
	{ \
		initor() \
		{ \
			/* todo: hold list of event typeids and check whether this event is registered or not */ \
			using namespace luabind; \
			module(L) \
			[ \
				class_< reflection::event<REMOVE_BRACES##ARGS> >(#EVENT_NAME) \
					.def(self(other<REMOVE_BRACES##ARGS>())) \
			]; \
			get_lua_class().def_readwrite(#EVENT_NAME, &##EVENT_NAME); \
			commit_lua_properties(); \
		} \
	}; \
\
	register_event_##EVENT_NAME() \
	{ static initor init; } \
} reg_event_##EVENT_NAME;

#endif // _LUA_H_
