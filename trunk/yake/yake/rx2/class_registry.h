#ifndef class_registry_h
#define class_registry_h

#include <string>
#include <map>

namespace rx
{

class meta_class;

// todo use yake registry
typedef std::map< std::string, const meta_class * > classes;
struct class_registry
{
public:	
	static classes classes_;
};

// class registration
void register_class( const meta_class & meta_class_ );
void unregister_class( const meta_class & meta_class_ );
const meta_class & get_class( const std::string & name );

// handler/event traits/signature validation
bool validate( const std::string & event, const std::string & handler );

} // namespace rx

#endif