#ifndef typed_field_h
#define typed_field_h

#include "meta_field.h"
#include <boost/lexical_cast.hpp>

namespace rx
{

template< typename T >
struct typed_field : public meta_field
{
	typedef typed_field< T > this_type;

	typed_field( int flags = none )
		: meta_field( flags )
	{}

	typed_field( std::string field_name, T value = T(), 
		int flags = none ) 
		: meta_field( field_name, flags ), 
		  value_( value )
	{	
	}

	typed_field( meta_object & object, std::string field_name, 
		T value, int flags = none ) 
		: meta_field( object, field_name, flags ),
		  value_( value )
	{	
	}

	T & operator=( T const & value )
	{
		value_ = value;   
		std::cout << get_object().get_name() << "::" << name_ 
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

	std::string type_as_string()
	{
		return typeid( value_ ).name();
	}

	std::string value_as_string()
	{
		return boost::lexical_cast<std::string>( value_ );
	}

	// todo via constr, clone
	void set_min_error( float predict_min_error )
	{
		if( !( flags_ & predict ) ) throw exception();
		predict_min_error_ = predict_min_error;    
	}

	void set_max_error( float predict_max_error )
	{
		if( !( flags_ & predict ) ) throw exception();
		predict_max_error_ = predict_max_error;    
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
    object.add_field< this_type >( * new this_type( object, name_, value_, flags_ ) );
	};

	T value_;
	float predict_min_error_;
	float predict_max_error_;
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

} // namespace rx

#endif