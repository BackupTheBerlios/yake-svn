#ifndef _EVENT_TEST_H_
#define _EVENT_TEST_H_

#include <string>
#include <iostream>

#include "meta_class.h"

#include <yake/base/templates/yakePointer.h>

using namespace rx;

/* --------------------------------------  sample -------------------------------------- */
struct entity
{
	typedef event< const std::string & > birth_event;

	entity( const std::string & object_name )
		: meta_object_( 
				instance<meta_object>( meta_class_, object_name )
					.add_event( &birth )
					.add_handler<const std::string &>( &entity::on_birth, this ) )
	{
	}

	void on_birth( const std::string & cry )
  {
		std::cout << meta_object_->get_name() << ".on_birth( \"" << cry << "\" )" << std::endl;
  }

	birth_event birth;


	static void build_meta_class()
	{
		// register fields
		meta_class_
			.add_event< const std::string & >( "birth" )
			.add_handler< const std::string & >( "on_birth" );
	}	

	static meta_class & get_class()
	{
		return meta_class_;
	}


	meta_object & get_object()
	{
		return *meta_object_.get();
	}

	// one c++ class needs one static meta class
	static meta_class meta_class_;
	// but one c++ class can have n meta objects
	meta_object_ptr meta_object_;
};

meta_class entity::meta_class_( "entity" );

namespace 
{
	static struct entity_initor
	{
		entity_initor()
		{
			static int counter_ = 0;
			if( counter_++ > 0 ) return;
			entity::build_meta_class();
		}		
	} global_entity_initor;
}; // unnamed namespace

#endif _EVENTS_H_