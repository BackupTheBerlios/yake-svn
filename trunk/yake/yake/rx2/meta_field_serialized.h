#ifndef META_FIELD_SERIALIZED_H
#define META_FIELD_SERIALIZED_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "meta_field.h"

BOOST_IS_ABSTRACT(rx::meta_field)

namespace boost 
{ 
namespace serialization 
{

template<class Archive>
void serialize(Archive & ar, rx::meta_field & field, const unsigned int version)
{
  ar & field.flags_;
	ar & field.object_;
	ar & field.name_;
}

}
}

#endif