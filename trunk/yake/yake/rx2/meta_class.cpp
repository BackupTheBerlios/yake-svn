#include "meta_class.h"

namespace rx
{

// used by regular meta object instancing
meta_object & new_( const meta_class & meta_class_, std::string object_name )
{
		meta_class & non_const_class = const_cast< meta_class & >( meta_class_ );

		meta_object * obj = new meta_object( object_name );

		for( meta_class::fields_list::iterator iter = non_const_class.fields_.begin(); 
			iter != non_const_class.fields_.end(); iter++ )
		{ 
			// call the according attach method for this field (using cloning)
			( iter->second )( *obj, *iter->first, true );
		}

		non_const_class.objects_.insert( meta_class::objects::value_type( object_name, obj ) ); 

		return *obj;
}

} // namespace rx