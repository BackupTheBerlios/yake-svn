#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <list>
#include <algorithm>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>


// using std and boost::lambda placeholders
boost::lambda::placeholder1_type _l1;

// abstract class without virtuals
struct event_base
{
  virtual ~event_base() = 0;
  virtual void attach_handler( const boost::function_base & this_handler ) = 0;
};

// todo: BOOST_IO_DEFINE_FORWARDING_FUNCTIONS see io detail/forwarding
// PUSH_PARAMS etc. see io for event?
template< typename t1 >
struct event : public event_base
{
	// handlers
	typedef t1 parameter1;
  typedef boost::function< void (parameter1) > handler;
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
  void attach_handler( const boost::function_base & this_handler )
  {
		// clone handler so that we can delete it without ownership problems
    handlers_.push_back( new handler( static_cast< const handler & >( this_handler ) ) ); 
  }

	// firing
  void operator() ( parameter1 p1 )
  {
    using namespace boost::lambda;
		std::for_each( handlers_.begin(), handlers_.end(), bind<void>( *_l1, p1 ) );
  }

private:
  handlers handlers_;
};

#endif _EVENTS_H_