#ifndef OBJECT_CREATORS_H
#define OBJECT_CREATORS_H

namespace rx
{

// for meta
meta_object & get_object( std::string class_name, std::string object_name )
{
	return class_registry::classes_.find( 
		class_name )->second->get_object( object_name );
}

// for c++
template< class T >
meta_object & get_object( std::string object_name )
{
	return T::get_class().get_object( object_name );
}

} // namespace rx

#endif