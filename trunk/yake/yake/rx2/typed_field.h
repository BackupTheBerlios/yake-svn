#ifndef typed_field_h
#define typed_field_h

#include <boost/lexical_cast.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include "meta_field.h"
#include "ref.h"

namespace rx
{

template< typename T >
struct typed_field : public meta_field
{
	typedef typed_field< T > this_type;

	typedef typename boost::remove_reference<T>::type no_ref_T;
	typedef typename boost::add_reference<T>::type ref_T;
	typedef typename boost::add_reference<const T>::type const_ref_T;

	typedef T value_type;
	typedef no_ref_T * pointer_T;

public: // constructors
	typed_field( int flags = none )
		: meta_field( flags ), 
			ptr_( NULL )
	{}

	// Used by wrapper c++ members, called by the c++ class constructor
	// We are using a reference to a c++ variable instead of holding the variable. 
	typed_field( ref_T ref_to_value, int flags ) 
		: meta_field( flags ),
			ptr_( &ref_to_value )
	{
	}

	typed_field( std::string field_name, const_ref_T value = const_ref_T(), int flags = none ) 
		: meta_field( field_name, flags ), 
		  value_( value ),
			ptr_( NULL )
	{	
	}

	typed_field( meta_object & object, std::string field_name, T value, int flags = none ) 
		: meta_field( object, field_name, flags ),
		  value_( value ),
			ptr_( NULL )
	{	
	}

public: // value access operations
	ref_T operator=( const_ref_T value )
	{
		( ptr_ ? *ptr_ : value_ ) = value;
		if( object_ )
		{
			std::cout << get_object().get_name() << "::" << name_ 
				<< "=" << value << std::endl; 
			get_object().on_change_field( *this );
		}
		return get();
	}

	void set( const_ref_T value )
	{
		operator=( value );
	}

	ref_T get()
	{
		return ( ptr_ ? *ptr_ : value_ );
	};

	// explicit conversion
	operator ref_T()
	{
		return get();
	}

public: // info
	const TypeInfo type()
	{
		return typeid( get() );
	}

	std::string value_as_string()
	{
		return boost::lexical_cast<std::string>( get() );
	}

public: // prediction
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

public: // data initialization access
	template< typename this_field >
	friend this_field & set_ref( this_field & field, typename this_field::value_type ref );

public: // data
 	no_ref_T value_;
	// If this field is a "wrapper" for a c++ pod field like int my_int
	// we have to use a reference to that instead instead of the value.
	// The trick is that we hold both, a value and a reference, so that
	// we don't have to access a wrapper/reference field via obj.field<int&>( "my_int" ) ...
	// Set the actual referene using the constructor or the set_ref helper function.
	pointer_T ptr_;

	// todo move this into observer
	float predict_min_error_;
	float predict_max_error_;
};

template< typename T >
std::ostream& operator<<( std::ostream & stream, typed_field<T> & field )
{
	stream << field.get();
	return stream;    
}

template< typename T >
bool operator==( typed_field<T> & field, const T & value )
{
	return field.get() == value;    
}

// special case: typed_field<std::string> == "hello"
// the compiler should create a temporary string object, but it does not?
template< typename T >
bool operator==( typed_field<T> & field, const char * value )
{
	return field.get() == value;    
}

template< typename this_field >
this_field & set_ref( this_field & field, typename this_field::value_type ref )
{
	field.ptr_ = &ref;
  return field;
};

/*template< typename T >
bool operator==( typed_field< ref<T> > & field, T value )
{
	return field.value_ == value;    
}*/

} // namespace rx

#endif