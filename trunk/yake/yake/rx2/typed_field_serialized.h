#ifndef TYPED_FIELD_SERIALIZED_H
#define TYPED_FIELD_SERIALIZED_H

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
    ar & field.value_;
		ar & field.bit_flags_;
}

}
}

#endif