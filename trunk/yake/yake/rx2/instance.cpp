#include "instance.h"


namespace rx
{

// used by regular meta object instancing
meta_object & instance( const meta_class & meta_class_, std::string object_name )
{
		meta_class & non_const_class = const_cast< meta_class & >( meta_class_ );

		meta_object * obj = new meta_object( object_name );

		for( meta_class::fields_list::iterator iter = non_const_class.fields_.begin(); 
			iter != non_const_class.fields_.end(); iter++ )
		{ 
			// call the according attach method for this field (using cloning)
			( iter->second )( *obj, *iter->first );
		}

		return *obj;
}

} // namespace rx