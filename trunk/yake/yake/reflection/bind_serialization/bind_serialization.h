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
void serialize(Archive & ar_, CLASS_NAME & obj_, const unsigned int) \
{	ar_ & boost::serialization::base_object<Object>(obj_); } } } \
\
namespace { \
static void register_##CLASS_NAME##() \
{ ClassRegistry::registerClass(CLASS_NAME##::getClassStatic()); } }
//STATIC_INIT( register_##CLASS_NAME ) todo need singleton, yake class::registry => class registry?

BOOST_CLASS_EXPORT(Object)

namespace boost 
{ 
namespace serialization 
{

template<typename ValueType_>
ValueType_ getPropertyValue(const Property & prop_, const Object & obj_)
{
	ValueType_ value;
	prop_.get(value, &obj_);
	return value;
}

template<typename ValueType_, class Archive_>
bool selectiveSave(Archive_ & ar_, const Object & obj_, const Property & prop_)
{
  if(prop_.getTypeInfo() == typeid(ValueType_))
	{
		ar_ << getPropertyValue<ValueType_>(prop_, obj_);
		return true;
	}
	else
	{
		return false;
	}
}

template<class Archive_>
void save(Archive_ & ar_, const Object & obj_, unsigned int)
{
	// properties
  for(Class::PropertyList::const_iterator iter(obj_.getClass().getProperties().begin());
		iter != obj_.getClass().getProperties().end();
		++iter)
	{
		const Property & prop = *iter;
		selectiveSave<bool> (ar_, obj_, prop) || 
		selectiveSave<int>  (ar_, obj_, prop) ||
		selectiveSave<float>(ar_, obj_, prop) ||
		selectiveSave<long> (ar_, obj_, prop);
	}
}

template<typename ValueType_, class Archive_>
bool selectiveLoad(Archive_ & ar_, const Object & obj_, const Property & prop_)
{
  if(prop_.getTypeInfo() == typeid(ValueType_))
	{
		ValueType_ value;
		ar_ >> value;
		prop_.set( &obj_, value );
		return true;
	}
	else
	{
		return false;
	}
}

template<class Archive>
void load(Archive & ar_, Object & obj_, unsigned int)
{
	// properties
  for(Class::PropertyList::const_iterator iter(obj_.getClass().getProperties().begin());
		iter != obj_.getClass().getProperties().end();
		++iter)
	{
		const Property & prop = *iter;
		selectiveLoad<bool> (ar_, obj_, prop) || 
		selectiveLoad<int>  (ar_, obj_, prop) ||
		selectiveLoad<float>(ar_, obj_, prop) ||
		selectiveLoad<long> (ar_, obj_, prop);
	}
}

} // serialization
} // boost

BOOST_SERIALIZATION_SPLIT_FREE(Object)

#endif // _SERIALIZE_H_