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
	typedef T value_type;
	typedef typename boost::remove_reference<T>::type no_ref_T;
	typedef typename boost::add_reference<T>::type ref_T;
	typedef typename boost::add_reference<const T>::type const_ref_T;

public: // constructors
	typed_field( int flags = none )
		: meta_field( flags ), 
			value_( boost::remove_reference<T>::type() ),
			ref_( value_ ),
			use_ref(false)
	{}

	// used by wrapper c++ members, called within c++ class constructor
	typed_field( T value, int flags ) 
		: meta_field( flags ),
			ref_( value ),
			use_ref(true)
	{
	}

	typed_field( std::string field_name, const_ref_T value = const_ref_T(), int flags = none ) 
		: meta_field( field_name, flags ), 
		  value_( value ),
			ref_( value_ ),
			use_ref(false)
	{	
	}

	typed_field( meta_object & object, std::string field_name, T value, int flags = none ) 
		: meta_field( object, field_name, flags ),
		  value_( value ),
			ref_( value_ ),
			use_ref(false)
	{	
	}

	// todo typedef for ref_T and T

public: // value access operations
	ref_T operator=( const_ref_T value )
	{
		( use_ref ? ref_ : value_ ) = value;
		if( object_ )
		{
			std::cout << get_object().get_name() << "::" << name_ 
				<< "=" << value << std::endl; 
			get_object().on_change_field( *this );
		}
		return get();
	}

	// used by script
	void set( const_ref_T value )
	{
		operator=( value );
	}

	ref_T get()
	{
		return use_ref ? ref_ : value_;
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

public: // data
 	no_ref_T value_;
	ref_T ref_;
	bool use_ref; // If this field is a "wrapper" for a c++ pod field like int my_int
								// we have to use a reference to that instead instead of the value.
								// The trick is that we hold both, a value and a reference, so that
								// we don't have to access a wrapper/reference field via obj.field<int&>( "my_int" ) ...

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
bool operator==( typed_field<T> & field, T value )
{
	return field.get() == value;    
}

/*template< typename T >
bool operator==( typed_field< ref<T> > & field, T value )
{
	return field.value_ == value;    
}*/

} // namespace rx

#endif