#ifndef meta_object_h
#define meta_object_h

#include <map>

#include <yake/base/templates/yakePointer.h>

#include "typed_field.h"
#include "meta_hooks.h"
#include "object_registry.h"
#include "events.h"

namespace rx
{

class meta_object : public meta_object_hooks
{
public: // types
	typedef std::map< std::string, meta_field * > fields_map;
  typedef std::map< std::string, boost::function_base * > handlers;
	typedef std::map< std::string, event_base * > events;

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

	meta_object( const meta_object & copy )
		: name_( copy.name_ ),
			fields_( copy.fields_ ),
			events_( copy.events_ ),
			handlers_( copy.handlers_ )
	{
		register_object( *this );       
	}

	~meta_object()
	{
		// delete fields
		/*for( fields_map::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		  delete iter->second;*/ // todo they can be c++ object member fields

		// delete handlers
		for( handlers::iterator iter = handlers_.begin(); 
			iter != handlers_.end(); iter++ )
		  delete iter->second;

		// unregister object
		unregister_object( *this );
	}

public: // field management
	// used by c++
	template< typename T, int flags >
	meta_object & add_field( std::string field_name, T default_value = T() )
	{
		add_field( field_name, default_value, flags );
		return *this;
	}

	// used by script
	template< typename T >
	meta_object & add_field( std::string field_name, T default_value = T(), int flags = none )
	{
		typed_field<T> * field = new typed_field<T>( 
			*this, field_name, default_value, flags );
		fields_.insert( fields_map::value_type( field_name, field ) );
		on_add_field( *field );
		return *this;
	}

	// used by c++ classes within the constructor
	template< typename T >
	meta_object & add_field( T & field )
	{
		field.object_ = this;
		fields_.insert( fields_map::value_type( field.name_, &field ) );
		on_add_field( field );
		return *this;
	}

	template< typename T >
	typed_field<T> & field( std::string name ) const
	{
		fields_map::const_iterator iter = fields_.find( name );
		if( iter == fields_.end() ) throw exception();
		return static_cast< typed_field<T>& >( *iter->second );
	}

	fields_map::const_iterator begin() const
	{
		return fields_.begin();
	}

	fields_map::const_iterator end() const
	{
		return fields_.end();
  }

public: // event and handlers
	template< class event_class >
	meta_object & add_event( 
		const std::string & name, 
		event_class & this_event )
	{
		events_.insert( events::value_type( name, &this_event ) );
		return *this;    
	}

	template< typename parameter1, typename class_function_ptr, typename class_this >
	meta_object & add_handler( 
		const std::string & name, 
		class_function_ptr ptr, 
		class_this cls )
	{
    boost::function_base * func = 
			new boost::function< void ( parameter1 ) >( 
				boost::bind( ptr, cls, _1 ) );
		handlers_.insert( handlers::value_type( name, func ) );  
		return *this;
	}

	boost::function_base & get_handler( const std::string & name )
	{
    handlers::iterator iter( handlers_.find( name ) );
		if( iter == handlers_.end() ) throw exception();
    return *iter->second;
	}

	event_base & get_event( const std::string & name )
	{
    events::iterator iter( events_.find( name ) );
		if( iter == events_.end() ) throw exception();
    return *iter->second;
	}

	template< typename T1 >
	event<T1> & get_event( const std::string & name )
	{
    events::iterator iter( events_.find( name ) );
		if( iter == events_.end() ) throw exception();
    return dynamic_cast< event<T1> & >( *iter->second );
	}

public: // info
	std::string & get_name() const
	{
		return name_;
	}

public: // data
	mutable std::string name_;
	fields_map fields_; 
	events events_;
	handlers handlers_;
};

typedef yake::base::templates::SharedPtr<meta_object> meta_object_ptr;

} // namespace rx

#endif