#ifndef _EVENT_TEST_H_
#define _EVENT_TEST_H_

#include <string>
#include <iostream>

#include "meta_class.h"
#include "cpp_to_meta_helpers.h"

struct entity
{
	entity( const std::string & object_name )
	{
		RX_INIT( object_name )
	}

	void on_birth( const std::string & cry )
  {
		std::cout << meta_object_->get_name() << ".on_birth( \"" << cry << "\" )" << std::endl;
  }

	// todo define EVENT( type, name ) type, name
	RX_DEFINE_ONE_EVENT_ONE_HANDLER(
		const std::string &, birth,
		const std::string &, on_birth )
};

#endif // _EVENTS_H_