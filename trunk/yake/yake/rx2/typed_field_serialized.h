#ifndef TYPED_FIELD_SERIALIZED_H
#define TYPED_FIELD_SERIALIZED_H

#include <boost/serialization/split_free.hpp>
#include "meta_field_serialized.h"
#include "concrete_fields.h"

BOOST_CLASS_EXPORT(bool_field)
BOOST_CLASS_EXPORT(int_field)
BOOST_CLASS_EXPORT(float_field)
BOOST_CLASS_EXPORT(string_field)

namespace boost 
{ 
namespace serialization 
{

template<class Archive, typename T>
void serialize(Archive & ar, typed_field<T> & field, const unsigned int version)
{
		ar & boost::serialization::base_object<meta_field>(field);

		if( ( ( ar.is_loading::value ) &&
			(	field.flags_ & ::load ) ) || 
			( ( ar.is_saving::value ) && 
			(	field.flags_ & ::save ) ) )
		{
			ar & field.value_;
		}
}

} // namespace serialization
} // namespace boost


#endif