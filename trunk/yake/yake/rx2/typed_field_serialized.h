#ifndef TYPED_FIELD_SERIALIZED_H
#define TYPED_FIELD_SERIALIZED_H

#include <boost/serialization/split_free.hpp>
#include "meta_field_serialized.h"
#include "concrete_fields.h"

BOOST_CLASS_EXPORT(rx::bool_field)
BOOST_CLASS_EXPORT(rx::int_field)
BOOST_CLASS_EXPORT(rx::float_field)
BOOST_CLASS_EXPORT(rx::string_field)

namespace boost 
{ 
namespace serialization 
{

template<class Archive, typename T>
void serialize(Archive & ar, rx::typed_field<T> & field, const unsigned int version)
{
	ar & boost::serialization::base_object<rx::meta_field>(field);

		if( ( ( ar.is_loading::value ) &&
			(	field.flags_ & rx::load ) ) || 
			( ( ar.is_saving::value ) && 
			(	field.flags_ & rx::save ) ) )
		{
			ar & field.value_;
			ar & field.predict_min_error;
			ar & field.predict_max_error;
		}
}

} // namespace serialization
} // namespace boost


#endif