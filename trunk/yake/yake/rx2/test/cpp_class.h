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

/*struct cpp_class2
{
public:
	cpp_class( std::string object_name )
	{
		rx_init( object_name );
	}

	// c++ members
	int position_;

public:
static rx::meta_class & get_class(){ return meta_class_; }
rx::meta_object & get_object(){ return *meta_object_.get(); }

private:
	static rx::meta_class meta_class_;
	rx::meta_object_ptr meta_object_;

public:
rx::typed_field<int&> position_wrapped;
rx::typed_field<int> test_int;
rx::typed_field<std::string> test_string;
rx::typed_field<float> test_float;

static void rx_construct_meta_class()
{
	meta_class_
		.add_field<int&>( "position_wrapped" )
		.add_field<int>( "test_int" )
		.add_field<std::string>( "test_string" )
		.add_field<float>( "test_float" );
}

void rx_init( const std::string & object_name )
{
  meta_object_ =
	  rx::instance_holder( meta_class_, object_name )
			.add_field( set_ref( position_wrapped, position_ ) )
			.add_field( test_int )
			.add_field( test_string )
			.add_field( test_float );
}

};

rx::meta_class class_name::meta_class_( cpp_class2 );
STATIC_INITOR_CL1( cpp_class2, rx_construct_meta_class )*/

#endif // _CPP_CLASS_H_