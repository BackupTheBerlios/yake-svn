#ifndef _EVENTS_H_
#define _EVENTS_H_

// stl
#include <list>
#include <algorithm>

// boost
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/any.hpp>

// rx 
#include "reflection.h"

// lua
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#define LUABIND_NO_HEADERS_ONLY
#include <luabind/luabind.hpp>
#include <luabind/functor.hpp>

// using std and boost::lambda placeholders
boost::lambda::placeholder1_type _l1;

// todo move this to yake::base

// abstract class without virtuals
struct event_base
{
	virtual ~event_base() {}
  virtual void attach_handler( const boost::function_base & this_handler ) = 0;
};


template< typename arg1 >
struct regular_cast_1
{
	typedef boost::function< void (arg1) > result_functor;

	static result_functor cast( boost::function_base & function )
	{
			return static_cast< result_functor & >( function );
	}
};


template< typename arg1, typename arg2 >
struct regular_cast_2
{
	typedef boost::function< void (arg1, arg2) > result_functor;

	static result_functor cast( boost::function_base & function )
	{
			return static_cast< result_functor & >( function );
	}
};


// todo: BOOST_IO_DEFINE_FORWARDING_FUNCTIONS see io detail/forwarding
// PUSH_PARAMS etc. see io for event?
template
< 
	typename arg1,
	template<typename> class handler_cast_policy = regular_cast_1
>
struct event : public event_base
{
	// handlers
  typedef boost::function<void (arg1)> handler;
	typedef std::list<const handler*> handlers;

  ~event()
  {
    for(handlers::iterator iter(handlers_.begin());
      iter != handlers_.end(); iter++)
		{ delete *iter; }
  }

	// hard wiring
  template<typename class_function_ptr, typename class_this>
  void attach_handler(class_function_ptr ptr, class_this cls) const
  { 
		// create new handler
    handlers_.push_back(new handler(boost::bind(ptr, cls, _1)));
  }

	// hard wiring
  template<typename function_ptr>
  void attach_handler(function_ptr ptr)
  { 
		// create new handler
    handlers_.push_back(new handler(boost::bind(ptr, _1)));
  }

	// soft wiring
  void attach_handler(const boost::function_base & this_handler)
  {
		// clone handler so that we can delete it without ownership problems
    handlers_.push_back(new handler(handler_cast_policy<arg1>::cast(const_cast<boost::function_base&>(this_handler))));
  }

	// firing
	virtual fire(arg1 a1)
	{
    using namespace boost::lambda;
		std::for_each(handlers_.begin(), handlers_.end(), bind<void>(*_l1, a1));
	}

  void operator()(arg1 a1)
  {
		fire(a1);
  }

protected:
  handlers handlers_;
};


// todo split files and place event into yake::base
// todo lua_handler_to_cpp_1 simpliy => add attach policy with event::attach_handler( lua::functor ) and bind to lua
#include "bind_lua/lua_handler_to_cpp.h"

namespace reflection
{

struct rx_event_base
{
	virtual ~rx_event_base() {}
	virtual void attach_handler(void * object, const reflection::Method & this_handler) = 0;
};

struct lua_event_base
{
	virtual ~lua_event_base() {}
	virtual void attach_handler(const luabind::functor<void> & lua_function) = 0;
};

	// todo move to rx

template<typename arg1>
struct event : public rx_event_base, public lua_event_base, public ::event<arg1 /*, lua::search_for_lua_handlers_1*/> 
{
	typedef ::event<arg1/*, lua::search_for_lua_handlers_1*/> base;

	// add base functions to scope
	using base::attach_handler;

	// attach reflected method
	void attach_handler(void * object, const reflection::Method & this_handler)
	{
		// rebind functor with 'this pointer' of the parent object as first argument and clone
		boost::function<void (void*, arg1)> & hdl 
			= static_cast<boost::function<void (void*, arg1)>&>(this_handler.get_function_base());
		handlers_.push_back(new handler(boost::bind(hdl, object, _1)));
	}

	typedef std::vector< const luabind::functor<void> > lua_functor_list;

	// attach lua functor
	void attach_handler(const luabind::functor<void> & lua_function)
	{
		m_lua_functor_list.push_back(lua_function);
	}

	// firing
	virtual fire(arg1 a1)
	{
		// call handlers of the base class
		base::fire(a1);
		// call lua handlers
		for(lua_functor_list::const_iterator iter = m_lua_functor_list.begin();
			iter != m_lua_functor_list.end(); iter++)
		{ iter->operator()(a1); }
	}

	lua_functor_list m_lua_functor_list;
};

}

/* todo for EVENT macro args:
template< typename arg1 = Null, typename arg2 = Null, typename arg3 = Null, typename arg4 = Null, typename arg5 = Null >
struct event : public ::event< arg1, lua::search_for_lua_handlers_1 > {};

template<typename arg1>
struct event<Null, Null, Null> ... specialization
*/


// todo real __event__ structs and we need the attach_handler in lua

/*#define __EVENT_METHOD__(ACCESS_ATTR, VIRTUAL, RETURN_TYPE, METHOD_NAME, METHOD_ARGS) \
private: \
    struct __method_##METHOD_NAME##__ \
		{ \
        __method_##METHOD_NAME##__() \
				{ \
					static agm::reflection::__register_method__ reg(__create_callable__(&fire##_METHOD_NAME), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS, #VIRTUAL); \
        } \
    } __method_##METHOD_NAME##__; \
    friend struct __method_##METHOD_NAME##__; \
ACCESS_ATTR: \
	  VIRTUAL RETURN_TYPE fire##_METHOD_NAME METHOD_ARGS


#define EVENT1(ACCESS_ATTR, EVENT_NAME, ARG1, NAME1) \
ACCESS_ATTR: \
		__EVENT_METHOD__(ACCESS_ATTR, __NOT_VIRTUAL__, void, EVENT_NAME, (ARG1 NAME1)) { EVENT_NAME(NAME1); } \
		rx::event<ARG1> EVENT_NAME*/

#endif _EVENTS_H_