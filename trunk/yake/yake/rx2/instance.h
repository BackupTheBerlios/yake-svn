#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "meta_class.h"

namespace rx
{

struct meta_object_holder;

// used by regular meta object instancing
meta_object & instance( const meta_class & meta_class_, std::string object_name );

// used by c++ classes
meta_object_holder instance_holder( const meta_class & meta_class_, std::string object_name );


/* This class is used by instance() and allows the user to do not
   specify the typed_field names within the constructor or initialization
	 function of a c++ class as long as the order of fields are the same as 
	 within the accoring meta_class. */
struct meta_object_holder
{
public: // constructors
	meta_object_holder( const meta_class & meta_class_, const std::string & object_name )
		: object_( new meta_object( object_name ) ), 
			field_pos_(0),
			event_pos_(0),
			handler_pos_(0),
			class_(  meta_class_ )
	{
	}

public: // wrapped meta object functions
	template< typename T >
	meta_object_holder & add_field( T & field )
	{
		field.name_ = class_.fields_[ field_pos_++ ].get<1>()->get_name();		
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

public: 
	/* explicitly conversion used by a c++ class constructor
		 or initialization function for its smart pointer to a 
		 meta_object ( the pointer is needed because we want to
		 delete the meta_object of a c++ object at the time of the 
		 c++ object destruction see c++ helper macros ) */
	operator meta_object_ptr()
	{
    return object_;
	}

private: // data
	const meta_class & class_;
	meta_object_ptr object_;
	int field_pos_;
	int handler_pos_;
	int event_pos_;
};

} // namespace rx

#endif // _INSTANCE_H_