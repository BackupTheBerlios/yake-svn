#include "meta_field.h"
#include "meta_object.h"

namespace rx
{

std::string meta_field::as_string()
{
		return std::string( 
			get_object().get_name() + "::" + get_name() + 
				"[" + type().name() + "]=" + value_as_string() );
}

} // namespace rx