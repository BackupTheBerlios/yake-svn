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

// using std and boost::lambda placeholders
boost::lambda::placeholder1_type _l1;

// abstract class without virtuals
struct event_base
{
  virtual ~event_base() = 0;
  virtual void attach_handler( boost::function_base & this_handler ) = 0;
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
  typedef boost::function< void (arg1) > handler;
	typedef std::list< const handler * > handlers;

  ~event()
  {
    for( handlers::iterator iter( handlers_.begin() );
      iter != handlers_.end();
      iter++ )
			delete *iter;
  }

	// hard wiring
  template< typename class_function_ptr, typename class_this >
  void attach_handler( class_function_ptr ptr, class_this cls )
  { 
		// create new handler
    handlers_.push_back( new handler( boost::bind( ptr, cls, _1 ) ) );
  }

	// soft wiring
  void attach_handler( boost::function_base & this_handler )
  {
		// clone handler so that we can delete it without ownership problems
    handlers_.push_back( new handler( handler_cast_policy<arg1>::cast( this_handler ) ) );
  }

	// firing
  void operator() ( arg1 a1 )
  {
    using namespace boost::lambda;
		std::for_each( handlers_.begin(), handlers_.end(), bind<void>( *_l1, a1 ) );
  }

private:
  handlers handlers_;
};


// todo split files and place event into yake::base
#include "lua/lua_handler_to_cpp.h"
namespace rx
{

template< typename arg1 >
struct event : public ::event< arg1, lua::search_for_lua_handlers_1 > {};

} // namespace rx

#endif _EVENTS_H_