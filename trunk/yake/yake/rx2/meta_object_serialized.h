#ifndef META_OBJECT_SERIALIZED_H
#define META_OBJECT_SERIALIZED_H

#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>
#include "meta_object.h"

BOOST_CLASS_EXPORT(rx::meta_object)

namespace boost 
{ 
namespace serialization 
{

template<class Archive>
void serialize(Archive & ar, rx::meta_object & object, const unsigned int version)
{
	  ar & object.name_;
	  ar & object.fields_;
}

}
}

#endif