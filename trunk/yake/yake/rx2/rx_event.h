#ifndef _RX_EVENTS_H_
#define _RX_EVENTS_H_

#include "events.h"
#include "lua/lua_handler_to_cpp.h"

namespace rx
{

template< typename arg1 >
struct event : public ::event< arg1, lua::search_for_lua_handlers_1 >
{
}

} // namespace rx

#endif _RX_EVENTS_H_