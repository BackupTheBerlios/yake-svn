#ifndef _EVENT_TEST_H_
#define _EVENT_TEST_H_

#include <string>
#include <iostream>

#include "meta_class.h"
#include "cpp_to_meta_helpers.h"

using namespace rx;

/* --------------------------------------  sample -------------------------------------- */
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

	RX_DEFINE_ONE_EVENT_ONE_HANDLER(
		const std::string &, birth,
		const std::string &, on_birth );
};

RX_DEFINE( entity )

#endif _EVENTS_H_