#ifndef _BIND_LUA_H_
#define _BIND_LUA_H_

#include "static_init.h"
#include "type_info.h"
#include "construct_type_from_arbitrary_types.h"
#include "get_type_or_null.h"
#include "events.h"

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

// -----------------------------------------
// lua utility
bool dostring(lua_State* L, const char* str);

#define LUABIND_NO_HEADERS_ONLY
#include <luabind/luabind.hpp>


// -----------------------------------------
// dynamic lua class bindings
/* Each c++ class (foo) has static luabind class description (luabind::class_<foo>)
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

// -----------------------------------------
// lua state
static struct global_lua_state
{
	struct holder
	{
		holder() : m_L(lua_open())
		{	// open lua
			lua_baselibopen(m_L);
			luabind::open(m_L);
		}

		~holder()
		{ lua_close(m_L); }

		lua_State * m_L;
	};
	static holder * m_holder;

	~global_lua_state()
	{	
		if(m_holder)
		{ 
			delete m_holder;
			m_holder = NULL;
		}
	}

	lua_State * get()
	{	
		if(!m_holder) m_holder = new holder();
		return m_holder->m_L;	
	}

	operator lua_State*()
	{	return get();	}
} L; 

// -----------------------------------------
// lua class bindings
#define IMPLEMENT_LUA_CLASS(CLASS_NAME) \
static struct static_initor##CLASS_NAME \
{ \
	static_initor##CLASS_NAME() \
	{ \
		static int counter = 0; \
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
		/* bind class */ \
		luabind::module(L)[ get_lua_class() ]; \
} \
\
static luabind::class_<class_type> & get_lua_class() \
{ \
	static luabind::class_<class_type> CLASS_NAME##_lua(#CLASS_NAME); \
	return CLASS_NAME##_lua; \
} \
\
static luabind::detail::class_rep & get_lua_crep() \
{	\
	luabind::detail::class_rep * crep = luabind::detail::class_registry::get_registry(L)->find_class(LUABIND_TYPEID(class_type)); \
	assert(crep && "get_lua_crep(): invalid class rep pointer"); \
	return *crep;	\
} \
\
static void commit_lua_methods() \
{ \
	using namespace luabind; \
  /* get class rep */ \
	class_<class_type> & lua_class = get_lua_class(); \
	detail::class_rep & lua_crep = get_lua_crep(); \
\
	/* get address of private class member of class_<> */ \
	typedef luabind::class_<class_type> asm_lua_class_type_fix; \
	std::map<const char*, detail::method_rep, detail::ltstr> * class_methods = NULL; \
	__ASM_OFFSET__(class_methods, [lua_class], asm_lua_class_type_fix::m_methods); \
\
	/* get address of private class member of class_rep */ \
	std::map<const char*, detail::method_rep, detail::ltstr> * crep_methods = NULL; \
	__ASM_OFFSET__(crep_methods, [lua_crep], detail::class_rep::m_methods); \
\
	/* set reference to crep */ \
	for(std::map<const char*, detail::method_rep, detail::ltstr>::iterator iter = class_methods->begin(); \
		iter != class_methods->end(); ++iter) \
	{	iter->second.crep = &lua_crep; } \
\
	/* add to crep */ \
	*crep_methods = *class_methods; \
} \
\
/* callback this is declared as private in crep */ \
struct luabind_callback_fix \
{\
	boost::function2<int, lua_State*, int> func; \
	int pointer_offset; \
}; \
\
static void commit_lua_properties() \
{ \
	using namespace luabind; \
  /* get class rep */ \
	class_<class_type> & lua_class = get_lua_class(); \
	detail::class_rep & lua_crep = get_lua_crep(); \
\
	/* get adddress of private class member of class_<> */ \
	typedef luabind::class_<class_type> asm_lua_class_type_fix; \
	std::map<const char*, luabind_callback_fix, detail::ltstr> * class_getters = NULL; \
	std::map<const char*, luabind_callback_fix, detail::ltstr> * class_setters = NULL; \
	__ASM_OFFSET__(class_getters, [lua_class], asm_lua_class_type_fix::m_getters); \
	__ASM_OFFSET__(class_setters, [lua_class], asm_lua_class_type_fix::m_setters); \
\
	/* get adddress of private class member of class_rep */ \
	std::map<const char*, luabind_callback_fix, detail::ltstr> * crep_getters = NULL; \
	std::map<const char*, luabind_callback_fix, detail::ltstr> * crep_setters = NULL; \
	__ASM_OFFSET__(crep_getters, [lua_crep], detail::class_rep::m_getters); \
	__ASM_OFFSET__(crep_setters, [lua_crep], detail::class_rep::m_setters); \
\
	/* copy getters and setters to crep */ \
	*crep_getters = *class_getters; \
	*crep_setters = *class_setters; \
}

// -----------------------------------------
// lua method bindings
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

// -----------------------------------------
// lua property bindings
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

// -----------------------------------------
// lua event bindings

// lua operator signatures
// The get function is returning a null pointer of a specific type,
// this type is used as function signature (constructor<> holds the signature
// and the second parameter serves as const/non-const flag). 
// The overloaded template function .def() of class_ accepts the
// signature and automagically resolves the paramater types, because
// it's a template function.
template <int, bool, typename T1, typename T2, typename T3>
struct signature_holder {};

template <bool Constant, typename T1, typename T2, typename T3>
struct signature_holder<0, Constant, T1, T2, T3> 
{
  static luabind::detail::application_operator
	< 
		luabind::constructor<>, 
		Constant
	> * signature()
	{ return 0; }
};

template <bool Constant, typename T1, typename T2, typename T3>
struct signature_holder<1, Constant, T1, T2, T3> 
{
		static luabind::detail::application_operator
		<
			luabind::constructor<	luabind::other<T1> >, 
			Constant
		> * signature()
		{ return 0; }
};

template <bool Constant, typename T1, typename T2, typename T3>
struct signature_holder<2, Constant, T1, T2, T3> 
{
	static luabind::detail::application_operator
	<
		luabind::constructor
		<
			luabind::other<T1>, 
			luabind::other<T2>
		>, 
		Constant
	> * signature()
	{ return 0; }
};

template <bool Constant, typename T1, typename T2, typename T3>
struct signature_holder<3, Constant, T1, T2, T3> 
{
	static luabind::detail::application_operator
	<
		luabind::constructor
		<
			luabind::other<T1>, 
			luabind::other<T2>,
			luabind::other<T3>
		>, 
		Constant
	> * signature()
	{ return 0; }
};

// selects a signature holding template according to the number of arguments
template <int num_args, typename T1, typename T2, typename T3>
struct get_const_signature_holder 
{ 
	typedef typename signature_holder<num_args, true, T1, T2, T3> type; 
};

template <int num_args, typename T1, typename T2, typename T3>
struct get_signature_holder 
{
	typedef typename signature_holder<num_args, false, T1, T2, T3> type;
};

template<typename T1 = null, typename T2 = null, typename T3 = null>
struct get_const_signature : construct_type_from_arbitrary_types<get_const_signature_holder, T1, T2, T3>::type {};

template<typename T1 = null, typename T2 = null, typename T3 = null>
struct get_signature : construct_type_from_arbitrary_types<get_signature_holder, T1, T2, T3>::type {};


template <typename class_> 
bool lua_is_class_registered()
{
	typedef std::vector<yake::base::type_info> type_info_list;
	static type_info_list type_infos;
	if(std::find(type_infos.begin(), type_infos.end(), typeid(class_)) == type_infos.end())
	{	type_infos.push_back(typeid(class_)); return false; }
	else
	{ return true; }
}


#define LUA_EVENT(ACCESS_ATTR, EVENT_NAME, ARGS) \
public: \
	const reflection::event<REMOVE_BRACES##ARGS> & get_##EVENT_NAME() const \
	{ return EVENT_NAME; } \
private: \
	struct register_event_##EVENT_NAME \
	{ \
		struct initor \
		{ \
			initor() \
			{ \
				/* register the event class to lua if it is not registered yet */ \
				if(!lua_is_class_registered< reflection::event<REMOVE_BRACES##ARGS> >()) \
				{ \
					luabind::module(L) \
					[ \
						luabind::class_<reflection::event<REMOVE_BRACES##ARGS>, reflection::lua_event_base>(#EVENT_NAME) \
							.def(get_signature<REMOVE_BRACES##ARGS>::signature()) \
					]; \
				} \
				/* bind the event as read-only property */ \
				get_lua_class().property(#EVENT_NAME, &get_##EVENT_NAME); \
				commit_lua_properties(); \
			} \
		}; \
		register_event_##EVENT_NAME() { static initor init; } \
	} reg_event_##EVENT_NAME;

#define LUA_CUSTOM_EVENT(ACCESS_ATTR, EVENT_CLASS, EVENT_NAME) \
public: \
	const EVENT_CLASS & get_##EVENT_NAME() const \
	{ return EVENT_NAME; } \
private: \
	struct register_event_##EVENT_NAME \
	{ \
		struct initor \
		{ \
			initor() \
			{ \
				/* register the event class to lua if it is not registered yet */ \
				if(!lua_is_class_registered<EVENT_CLASS>()) \
				{ \
					luabind::module(L) \
					[ \
						luabind::class_<EVENT_CLASS, reflection::lua_event_base>(#EVENT_NAME) \
					]; \
				} \
				/* bind the event as read-only property */ \
				get_lua_class().property(#EVENT_NAME, &get_##EVENT_NAME); \
				commit_lua_properties(); \
			} \
		}; \
		register_event_##EVENT_NAME() { static initor init; } \
	} reg_event_##EVENT_NAME;

#endif // _BIND_LUA_H_
