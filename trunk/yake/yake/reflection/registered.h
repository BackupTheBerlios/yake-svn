#ifndef _REGISTERED_H_
#define _REGISTERED_H_

#include "reflection.h"

/*****************************************************************************
    MACROS
 *****************************************************************************/

#define create_reg(OBJECT_NAME) OBJECT_NAME(#OBJECT_NAME)

/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace reflection
{

struct Registered : Object
{
	typedef std::list<Registered*> InstanceList;
	typedef std::map<std::string, Registered*> InstanceMap;

	Registered() {}

	Registered(const char * name)
		: mName(name)
	{ 
		instances_list.push_back(this);
		instances_map.insert(InstanceMap::value_type(name, this));
	}

	const std::string & getName() const
	{	return mName;	}

	std::string mName;
	static InstanceList instances_list;
	static InstanceMap instances_map;
};

} // namespace reflection

#endif // _REGISTERED_H_