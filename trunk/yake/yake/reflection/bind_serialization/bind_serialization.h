#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include <boost/archive/text_oarchive.hpp> // we have to include the archives _before_ using BOOST_CLASS_EXPORT
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_free.hpp>

#include "static_init.h"
#include "reflection.h"
using namespace reflection;

#define IMPLEMENT_SERIALIZATION_CLASS(CLASS_NAME) \
BOOST_CLASS_EXPORT(CLASS_NAME) \
namespace boost { namespace serialization { \
template<class Archive> \
inline void serialize(Archive & ar, CLASS_NAME & obj, const BOOST_PFTO unsigned int file_version) \
{	ar & boost::serialization::base_object<reflection::Object>(obj); } } } \
\
namespace { \
static void register_##CLASS_NAME##() \
{ ClassRegistry::registerClass(CLASS_NAME##::getClassStatic()); } }
//STATIC_INIT( register_##CLASS_NAME ) todo need singleton, yake class::registry => class registry?

BOOST_CLASS_EXPORT(reflection::Object)

namespace boost
{ 
namespace serialization 
{

template<typename ValueType>
ValueType getPropertyValue(const Property & prop, const reflection::Object & obj)
{
	ValueType value;
	prop.get(value, &obj);
	return value;
}

template<typename ValueType, class Archive>
bool selectiveSave(Archive & ar, const reflection::Object & obj, const Property & prop)
{
  if(prop.getTypeInfo() == typeid(ValueType))
	{
		ar << getPropertyValue<ValueType>(prop, obj);
		return true;
	}
	else
	{
		return false;
	}
}

template<class Archive>
void save(Archive & ar, const reflection::Object & obj, const BOOST_PFTO unsigned int file_version)
{
	// properties
  for(Class::PropertyList::const_iterator iter(obj.getClass().getProperties().begin());
		iter != obj.getClass().getProperties().end();
		++iter)
	{
		const Property & prop = *iter;
		selectiveSave<bool> (ar, obj, prop) || 
		selectiveSave<int>  (ar, obj, prop) ||
		selectiveSave<float>(ar, obj, prop) ||
		selectiveSave<long> (ar, obj, prop);
	}
}

template<typename ValueType, class Archive>
bool selectiveLoad(Archive & ar, const reflection::Object & obj, const Property & prop)
{
  if(prop.getTypeInfo() == typeid(ValueType))
	{
		ValueType value;
		ar >> value;
		prop.set( &obj, value );
		return true;
	}
	else
	{
		return false;
	}
}

template<class Archive>
void load(Archive & ar, reflection::Object & obj, const BOOST_PFTO unsigned int file_version)
{
	// properties
  for(Class::PropertyList::const_iterator iter(obj.getClass().getProperties().begin());
		iter != obj.getClass().getProperties().end();
		++iter)
	{
		const Property & prop = *iter;
		selectiveLoad<bool> (ar, obj, prop) || 
		selectiveLoad<int>  (ar, obj, prop) ||
		selectiveLoad<float>(ar, obj, prop) ||
		selectiveLoad<long> (ar, obj, prop);
	}
}

} // serialization
} // boost

// todo: hmm ... does not work with boost 1.32?
//BOOST_SERIALIZATION_SPLIT_FREE(reflection::Object)
namespace boost { namespace serialization {
template<class Archive>
inline void serialize(
    Archive & ar,
    Object & obj,
    const unsigned int file_version)
{ split_free(ar, obj, file_version); }
}}

#endif // _SERIALIZE_H_