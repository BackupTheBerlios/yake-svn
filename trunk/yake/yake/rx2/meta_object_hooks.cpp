#include "meta_object_hooks.h"
#include "meta_object.h"

// todo codegen
void lua_hook::add_field_int( std::string field_name )
{
	dynamic_cast< meta_object* >( this )->add_field<int, none>( field_name );
}

void lua_hook::add_field_int( std::string field_name, int flags )
{
	dynamic_cast< meta_object* >( this )->add_field<int>( field_name, flags );
}

void lua_hook::add_field_int( std::string field_name, int default_value, int flags )
{
	dynamic_cast< meta_object* >( this )->add_field<int>( field_name, default_value, flags );
}


int_field & lua_hook::field_int( std::string field_name )
{
	return dynamic_cast< meta_object* >( this )->field<int>( field_name );
}