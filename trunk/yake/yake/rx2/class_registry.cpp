#include "class_registry.h"
#include "meta_class.h"

namespace rx
{

classes class_registry::classes_;

void register_class( meta_class & meta_class_ )
{
		class_registry::classes_.insert( 
			classes::value_type( 
				meta_class_.get_class_name(), &meta_class_ ) ); 
}

void unregister_class( std::string name )
{
	class_registry::classes_.erase( name );
}

meta_class & get_class( std::string name )
{
	classes::iterator iter = class_registry::classes_.find( name );
	if( iter == class_registry::classes_.end() ) throw exception();
	return *iter->second;  
}

} // namespace rx