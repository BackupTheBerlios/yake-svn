#include "object_registry.h"
#include "meta_object.h"

#include <iostream>

namespace rx
{

objects object_registry::objects_;

void register_object( const meta_object & meta_object_ )
{
		object_registry::objects_.insert( 
			objects::value_type( 
				meta_object_.get_name(), &meta_object_ ) );
}

void unregister_object( const meta_object & meta_object_ )
{
  object_registry::objects_.erase( meta_object_.get_name() );
}

const meta_object & get_object( const std::string & name )
{
	objects::iterator iter = object_registry::objects_.find( name );
	if( iter == object_registry::objects_.end() ) throw exception();
	return *iter->second;  
}

event_base & get_event( const std::string & name )
{
	std::string obj_name( name.substr( 0, name.find( '.' ) ) );
	std::string event_name( name.substr( name.find( '.' ) + 1, name.length() - obj_name.length() - 1 ) );
	return const_cast<meta_object&>( get_object( obj_name ) ).get_event( event_name );
}

boost::function_base & get_handler( const std::string & name )
{
	// todo use meta object functions ...
	std::string obj_name( name.substr( 0, name.find( '.' ) ) );
	std::string handler_name( name.substr( name.find( '.' ) + 1, name.length() - obj_name.length() - 1 ) );
	return const_cast<meta_object&>( get_object( obj_name ) ).get_handler( handler_name );
}

void soft_wire( const std::string & event, const std::string & handler )
{
  event_base & this_event = get_event( event );
	boost::function_base & this_handler = get_handler( handler );
	this_event.attach_handler( this_handler );  
}

} // namespace rx