#ifndef meta_object_h
#define meta_object_h

#include <map>
#include "typed_field.h"
#include "meta_hooks.h"
#include "object_registry.h"

namespace rx
{

class meta_object : public meta_object_hooks
{
public:
	typedef std::map< std::string, meta_field * > fields_map;

public: // constructors
	meta_object() 
	{
    register_object( *this );    
	}

	meta_object( std::string name ) 
		: name_( name )
	{
    register_object( *this );  
	}

	~meta_object()
	{
		// delete fields
		for( fields_map::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		{ delete iter->second; }
		// unregister object
		unregister_object( *this );
	}

public: // field management
	// used by c++
	template< typename T, int flags >
	void add_field( std::string field_name, T default_value = T() )
	{
		add_field( field_name, default_value, flags );
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
		fields_.insert( fields_map::value_type( field.name_, &field ) );
		on_add_field( field );
	}

	template< typename T >
	typed_field<T> & field( std::string name )
	{
		fields_map::iterator iter = fields_.find( name );
		if( iter == fields_.end() ) throw exception();
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

public: // info
	std::string & get_name() const
	{
		return name_;
	}

public: // data
	mutable std::string name_;
	fields_map fields_; 
};

} // namespace rx

#endif