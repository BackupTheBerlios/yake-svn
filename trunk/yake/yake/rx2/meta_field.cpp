#include "meta_field.h"
#include "meta_object.h"

std::string meta_field::as_string()
{
		return std::string( 
			get_object().get_name() + "::" + get_name() + 
				"[" + type_as_string() + "]=" + value_as_string() );
}