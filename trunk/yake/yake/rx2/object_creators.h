#ifndef OBJECT_CREATORS_H
#define OBJECT_CREATORS_H

#include "meta_class.h"

namespace rx
{

// used by regular meta object instancing
meta_object & create( const meta_class & meta_class_, std::string object_name );

struct meta_object_holder
{
  meta_object_holder( const meta_class & meta_class_, meta_object & object )
		: object_( object ), 
			field_pos_(0),
			event_pos_(0),
			handler_pos_(0),
			class_( meta_class_ )
	{
	}

	template< typename T >
	meta_object_holder & add_field( T & field )
	{
		field.field_name_ = class_.fields_[ field_pos_++ ].first->get_name();		
		object_.add_field( field );
	}

	template< class event_class >
	meta_object_holder & add_event( const event_class & this_event )
	{
		object_.events_.insert( meta_object::events::value_type( 
			class_.event_traits_[event_pos_++].first, this_event ) );
		return *this;    
	}

	template< typename parameter1, typename class_function_ptr, typename class_this >
	meta_object_holder & add_handler( 
		class_function_ptr ptr, 
		class_this cls )
	{
    boost::function_base * func = 
			new boost::function< void ( parameter1 ) >( 
				boost::bind( ptr, cls, _1 ) );
		object_.handlers_.insert( meta_object::handlers::value_type( 
			class_.handler_traits_[handler_pos_++].first, func ) );  
		return *this;
	}

	operator meta_object&()
	{
    return object_;
	}

	const meta_class & class_;
	meta_object & object_;
	int field_pos_;
	int handler_pos_;
	int event_pos_;
};

// used by c++ classes
template< typename T >
meta_object_holder create( const meta_class & meta_class_, std::string object_name )
{
	meta_object & obj = *new meta_object( object_name );
	return meta_object_holder( meta_class_, obj );
}

} // namespace rx

#endif