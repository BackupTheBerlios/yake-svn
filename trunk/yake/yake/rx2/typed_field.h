#ifndef typed_field_h
#define typed_field_h

#include "meta_field.h"
#include <boost/lexical_cast.hpp>

template< typename T, int bit_flags = none >
struct typed_field : public meta_field
{
	typedef typed_field< T, bit_flags > this_type;

	typed_field()
		: meta_field( typeid( value_ ).name() ), bit_flags_( bit_flags )
	{}

	typed_field( std::string field_name, T value = T() ) 
		: meta_field( field_name, typeid( value ).name() ), 
		  value_( value ), bit_flags_( bit_flags )
	{	
	}

	typed_field( meta_object & object, 
		std::string field_name, T value ) 
		: meta_field( object, field_name, typeid( value ).name() ), 
		  value_( value ), bit_flags_( bit_flags )
	{	
	}

	T & operator=( T const & value )
	{
		value_ = value;   
		std::cout << get_object().get_name() << "::" << field_name_ 
			<< "=" << value << std::endl; 
		get_object().on_change_field( *this );
		return value_;
	}

	void set( T const & value )
	{
		operator=( value );
	}

	T & get()
	{
		return value_;
	};

	// explicit conversion
	operator T&()
	{
		return value_;
	}

	std::string value_as_string()
	{
		return boost::lexical_cast<std::string>( value_ );
	}

	void add_to_object( meta_object & object )
	{
		// DISPATCH TRICK ( used by meta_class) : add clone to the meta object, this way we don't loose type information
		// and obj->add_field => hook::on_add_field can use typed_field<> instead of the abstract meta_field
		// interface, so the replication system has concrete information about the value offset and its' size
		object_ = &object;
    object.add_field< this_type >( *this );
	};

	void add_clone_to_object( meta_object & object )
	{
		// DISPATCH TRICK ( used by meta_class) : add clone to the meta object, this way we don't loose type information
		// and obj->add_field => hook::on_add_field can use typed_field<> instead of the abstract meta_field
		// interface, so the replication system has concrete information about the value offset and its' size
    object.add_field< this_type >( * new this_type( object, field_name_, value_ ) );
	};

	T value_;	
	int bit_flags_;
};

template< typename T >
std::ostream& operator<<( std::ostream & stream, typed_field<T> & field )
{
	stream << field.value_;
	return stream;    
}

template< typename T >
bool operator==( typed_field<T> & field, T value )
{
	return field.value_ == value;    
}

#endif