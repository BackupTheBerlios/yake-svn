#ifndef class_registry_h
#define class_registry_h

#include "meta_class.h"

// use yake registry
typedef std::map< std::string, meta_class * > classes;
struct class_registry
{
public:	
	static classes classes_;
};

classes class_registry::classes_;

#endif