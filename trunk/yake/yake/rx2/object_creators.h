#ifndef OBJECT_CREATORS_H
#define OBJECT_CREATORS_H

#include "meta_class.h"

namespace rx
{

// used by regular meta object instancing
meta_object & create( const meta_class & meta_class_, std::string object_name );

struct meta_object_holder
{
	meta_object_holder( const meta_class & meta_class_, const std::string & object_name )
		: object_( new meta_object( object_name ) ), 
			field_pos_(0),
			event_pos_(0),
			handler_pos_(0),
			class_(  meta_class_ )
	{
	}

	template< typename T >
	meta_object_holder & add_field( T & field )
	{
		field.name_ = class_.fields_[ field_pos_++ ].first->get_name();		
		object_->add_field( field );
		return *this;
	}

	template< class event_class >
	meta_object_holder & add_event( const event_class & this_event )
	{
		object_->add_event( class_.event_traits_[event_pos_++].first, this_event );
		return *this;    
	}

	template< typename parameter1, typename class_function_ptr, typename class_this >
	meta_object_holder & add_handler( 
		class_function_ptr ptr, 
		class_this cls )
	{
		object_->add_handler< parameter1 >( class_.handler_traits_[handler_pos_++].first, ptr, cls );  
		return *this;
	}

	operator meta_object_ptr()
	{
    return object_;
	}

	const meta_class & class_;
	meta_object_ptr object_;
	int field_pos_;
	int handler_pos_;
	int event_pos_;
};

// used by c++ classes
template< typename T >
meta_object_holder create( const meta_class & meta_class_, std::string object_name )
{
	return meta_object_holder( meta_class_, object_name );
}

} // namespace rx

#endif