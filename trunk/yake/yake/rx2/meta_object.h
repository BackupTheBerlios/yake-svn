#ifndef meta_object_h
#define meta_object_h

#include <map>
#include "typed_field.h"
#include "meta_hooks.h"

#include <iostream> // todo del

namespace rx
{

class meta_object : public meta_object_hooks
{
public:
	typedef std::map< std::string, meta_field * > fields_map;

public:
	meta_object() {}

	meta_object( std::string object_name ) 
		: object_name_( object_name )
	{
	}

	virtual ~meta_object()
	{
		for( fields_map::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		{ delete iter->second; }
	}

	// used by c++
	template< typename T, int flags >
	void add_field( std::string field_name, T default_value = T() )
	{
		typed_field<T> * field = new typed_field<T>( 
			*this, field_name, default_value, flags );
		fields_.insert( fields_map::value_type( field_name, field ) );
		on_add_field( *field );
	}

	// used by script etc.
	template< typename T >
	void add_field( std::string field_name, T default_value = T(), int flags = none )
	{
		typed_field<T> * field = new typed_field<T>( 
			*this, field_name, default_value, flags );
		fields_.insert( fields_map::value_type( field_name, field ) );
		on_add_field( *field );
	}


	template< typename T >
	void add_field( T & field )
	{
		fields_.insert( fields_map::value_type( field.field_name_, &field ) );
		on_add_field( field );
	}

	template< typename T >
	typed_field<T> & field( std::string name )
	{
		fields_map::iterator iter = fields_.find( name );
		if( iter == fields_.end() ) throw exception(); // todo
		return *static_cast< typed_field<T>* >( iter->second );
	}

	fields_map::const_iterator begin() const
	{
		return fields_.begin();
	}

	fields_map::const_iterator end() const
	{
		return fields_.end();
  }

	std::string & get_name()
	{
		return object_name_;
	}

	std::string object_name_;
	fields_map fields_; 
};

} // namespace rx

#endif