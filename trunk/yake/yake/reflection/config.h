#ifndef _YAKE_REFLECTION_CONFIG_H_
#define _YAKE_REFLECTION_CONFIG_H_


namespace yake
{
namespace reflection
{

/* compile time flags */
// dll import/export
#if defined( YAPP_GUI_EXPORTS )
#	define YAPP_GUI_API DLLEXPORT
#else
#	define YAPP_GUI_API DLLIMPORT
#endif

#define LUABIND_6 = 6
#define LUABIND_7RC3 = 73;
#define LUABIND_7RC4 = 74;

#define LUABIND_VER = LUABIND_7RC4;


// enable/disable lua bindings
//#define YAKE_REFLECTION_DISABLE_LUA

// todo: check whether reflection supports lua (is YAKE_RX_DISABLE_LUA defined?) or not, if disabled we cannot enable gui lua binding support

// todo: #include <bind_lua/bind_lua.h> if lua enabled
#ifndef YAKE_REFLECTION_DISABLE_LUA
#	define GUI_CLASS(CLASS_NAME, SUPER_CLASS_NAME) CLASS(CLASS_NAME, SUPER_CLASS_NAME, lua)
#	define GUI_CUSTOM_EVENT(ACCESS_ATTR, EVENT_CLASS, EVENT_NAME) CUSTOM_EVENT(ACCESS_ATTR, EVENT_CLASS, EVENT_NAME, lua)
#	define GUI_PROPERTY(TYPE, NAME) PROPERTY(TYPE, NAME, lua)
# define GUI_IMPLEMENT_CLASS(CLASS_NAME) IMPLEMENT_CLASS(CLASS_NAME, lua)
#else
#	define GUI_CLASS(CLASS_NAME, SUPER_CLASS_NAME) CLASS(CLASS_NAME, SUPER_CLASS_NAME)
#	define GUI_CUSTOM_EVENT(ACCESS_ATTR, EVENT_CLASS, EVENT_NAME) CUSTOM_EVENT(ACCESS_ATTR, EVENT_CLASS, EVENT_NAME)
#	define GUI_PROPERTY(TYPE, NAME) PROPERTY(TYPE, NAME)
# define GUI_IMPLEMENT_CLASS(CLASS_NAME) IMPLEMENT_CLASS(CLASS_NAME)
#endif

} // namespace reflection
} // namespace yake

#endif // _YAKE_REFLECTION_CONFIG_H_