#ifndef _CPP_CLASS_H_
#define _CPP_CLASS_H_

#include <string>
#include <iostream>

#include "meta_class.h"

// todo meta_object deserialization should set cpp_class members


	/* todo

<psy|code> if i want to replicate a position, do i _have_ to use typed_field<Vector3> or sth like that?
<psy|code> and now i have to tie this to the actual Movable...

<metaX> I think there are two possibilities : 
<metaX> 1. a simply class-to-member-ptr wrapper
<metaX> 2. used typed_field<vector> ;P
<metaX> -d
* FuSiON has joined #yake
<metaX> how would you do it with rx1?
<metaX> well, would be possible to do this:
<metaX> struct player : public movable, rxobject {}; and then add a class-member-ptr to movable::position ...
<metaX> but there would be no sychronization
==><metaX> perhaps it's possible to do typed_field<vector&>( "blah", this->myvector );?



	*/





struct cpp_class
{
public:
	// we can use a chain of typed_fields here, as long as
	// they have the same order as within the meta_class
	// field container see build_meta_class
	cpp_class( std::string object_name ) 
		:	//position_wrapped( "position_wrapped", ref<int>(position_) ),
			position_wrapped( position_, none ),			
			meta_object_( 
				create<meta_object>( meta_class_, object_name )
					.add_field( test_int )
					.add_field( test_string )
					.add_field( test_float )
					.add_field( position_wrapped ) )
	{
	}

	// c++ members
	int position_;

	// rx members
	typed_field<int> test_int;
	typed_field<std::string> test_string;
	typed_field<float> test_float;
	typed_field<int&> position_wrapped;
	
public:
	static void build_meta_class()
	{
		// register fields
		meta_class_
			.add_field<int>( "test_int" )
		  .add_field<std::string>( "test_string" )
			.add_field<float>( "test_float" )
			.add_field<int&>( "position_wrapped" ); 
	}	

	static meta_class & get_class()
	{
		return meta_class_;
	}


	meta_object & get_object()
	{
		return *meta_object_.get();
	}

private:
	// one c++ class needs one static meta class
	static meta_class meta_class_;
	// but one c++ class can have n meta objects
	meta_object_ptr meta_object_;
};

/* todo
the static class calls the meta_class constructor which calls
the register_class function of the class registry and this is
not initialised at this time, so we need a singleton class
registry/yake registry */
meta_class cpp_class::meta_class_( "cpp_class" );

namespace
{
	static struct cpp_class_initor
	{
		cpp_class_initor()
		{
			static int counter = 0;
			if( counter++ > 0 ) return;
			cpp_class::build_meta_class();
		} \
	} g_cpp_class_initor;
} // nameless

#endif