#ifndef object_registry_h
#define object_registry_h

#include <string>
#include <map>

#include "events.h"

namespace rx
{

class meta_object;

// todo use yake registry
typedef std::map< std::string, const meta_object* > objects;
struct object_registry
{
public:	
	static objects objects_;
};

// object registration
void register_object( const meta_object & obj );
void unregister_object( const meta_object & obj );
const meta_object & get_object( const std::string & name );

// soft wiring
boost::function_base & get_handler( const std::string & name );
event_base & get_event( const std::string & name );
void wire_events( const std::string & event, const std::string & handler );

} // namespace rx

#endif // object_registry_h