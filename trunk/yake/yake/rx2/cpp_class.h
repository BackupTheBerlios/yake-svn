#ifndef _CPP_CLASS_H_
#define _CPP_CLASS_H_

#include <string>
#include <iostream>

#include "meta_class.h"
#include "cpp_to_meta_helpers.h"

struct cpp_class
{
public:
	cpp_class( std::string object_name )
	{
		INIT_RX( object_name )
	}

	// c++ members
	int position_;

	DEFINE_RX_FIELDS_ONE_REF(
		int&, position_wrapped, position_,
		int, test_int, 
		std::string, test_string, 
		float, test_float )
};

DEFINE_RX( cpp_class )

/* todo
the static class calls the meta_class constructor which calls
the register_class function of the class registry and this is
not initialised at this time, so we need a singleton class
registry/yake registry */


#endif