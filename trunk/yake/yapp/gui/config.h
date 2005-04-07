#ifndef _YAPP_GUI_CONFIG_H_
#define _YAPP_GUI_CONFIG_H_

/* compile time flags */
// dll import/export
#if defined(YAPP_GUI_EXPORTS)
#	define YAPP_GUI_API DLLEXPORT
#else
#	define YAPP_GUI_API DLLIMPORT
#endif


// enable/disable lua bindings
//#define YAKE_GUI_DISABLE_LUA

#ifndef YAKE_GUI_DISABLE_LUA
#	pragma message("Yapp gui config: yapp.gui.config.lua_enabled")
#else
#	pragma message("Yapp gui config: yapp.gui.config.lua_disabled")
#endif

// todo: check whether reflection supports lua (is YAKE_RX_DISABLE_LUA defined?) or not, if disabled we cannot enable gui lua binding support

// todo: #include <bind_lua/bind_lua.h> if lua enabled
#ifndef YAKE_GUI_DISABLE_LUA
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

#endif // _YAPP_GUI_CONFIG_H_