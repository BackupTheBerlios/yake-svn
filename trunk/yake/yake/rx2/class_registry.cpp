#include "class_registry.h"
#include "meta_class.h"

namespace rx
{

classes class_registry::classes_;

void register_class( const meta_class & meta_class_ )
{
		class_registry::classes_.insert( 
			classes::value_type( 
				meta_class_.get_name(), &meta_class_ ) ); 
}

void unregister_class( const meta_class & meta_class_ )
{
	class_registry::classes_.erase( meta_class_.get_name() );
}

const meta_class & get_class( const std::string & name )
{
	classes::iterator iter = class_registry::classes_.find( name );
	if( iter == class_registry::classes_.end() ) throw exception();
	return *iter->second;  
}

} // namespace rx