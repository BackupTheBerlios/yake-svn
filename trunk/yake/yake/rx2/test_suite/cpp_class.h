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
		RX_INIT( object_name )
	}

	// c++ members
	int position_;

	RX_DEFINE_ONE_REF_THREE_FIELDS(
		int&, position_wrapped, position_,
		int, test_int, 
		std::string, test_string, 
		float, test_float )
};

#endif // _CPP_CLASS_H_