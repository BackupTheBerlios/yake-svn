#include "meta_class.h"

namespace rx
{

// used by regular meta object instancing
meta_object & meta_class::create_object( std::string object_name )
{
		meta_object * obj = new meta_object( object_name );

		for( fields_list::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		{ 
			// call the according attach method for this field (using cloning)
			( iter->second )( *obj, *iter->first, true );
		}

		objects_.insert( objects::value_type( object_name, obj ) ); 

		return *obj;
}

} // namespace rx