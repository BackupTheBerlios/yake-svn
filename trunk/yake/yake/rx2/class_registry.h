#ifndef class_registry_h
#define class_registry_h

#include <string>
#include <map>

namespace rx
{

class meta_class;

// use yake registry
typedef std::map< std::string, meta_class * > classes;
struct class_registry
{
public:	
	static classes classes_;
};

void register_class( meta_class & meta_class_ );
void unregister_class( std::string name );
meta_class & get_class( std::string name );

} // namespace rx

#endif