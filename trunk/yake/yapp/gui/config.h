#ifndef _YAPP_GUI_CONFIG_H_
#define _YAPP_GUI_CONFIG_H_

#include <yake/base/mpl/sequences>

namespace yake
{
namespace gui
{

/* compile time flags */
// enable/disable lua bindings
//#define YAKE_GUI_DISABLE_LUA

// todo: check whether reflection supports lua (is YAKE_RX_DISABLE_LUA defined?) or not

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

/* implemented types */
// properties
struct point;
typedef yake::base::mpl::sequences::list
	<
		bool,
		float,
		const char *,
		const point &	
	> implemented_properties;

// widgets
class button_base;
class static_text_base;
class multi_line_edit_box_base;
typedef yake::base::mpl::sequences
	<
		button_base,
		static_text_base,
		multi_line_edit_box_base	
	> implemented_widgets;

// todo: use get_type_or_null etc. see reflection event macros
/*#define YAKE_STATIC_ASSERT_WIDGETS(WIDGET1, WIDGET2, WIDGET3, WIDGET4) 
		BOOST_STATIC_ASSERT((
			compare_sequences
			< 
				implemented_widgets, // conditional types
				boost::mpl::list // handled/implemented (by this function) types
				<
					button_base, 
					static_text_base,
					multi_line_edit_box_base
				>
			>::type::value));*/

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONFIG_H_