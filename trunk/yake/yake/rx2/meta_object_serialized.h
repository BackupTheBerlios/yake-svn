#ifndef META_OBJECT_SERIALIZED_H
#define META_OBJECT_SERIALIZED_H

#include <boost/serialization/map.hpp>
#include "meta_object.h"

BOOST_CLASS_EXPORT(meta_object)

namespace boost 
{ 
namespace serialization 
{

template<class Archive>
void serialize(Archive & ar, meta_object & object, const unsigned int version)
{
	  ar & object.object_name_;
	  ar & object.fields_;
}

}
}

#endif