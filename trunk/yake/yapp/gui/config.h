#ifndef _YAPP_GUI_CONFIG_H_
#define _YAPP_GUI_CONFIG_H_

// todo: remove
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/remove_if.hpp>

#include <yake/base/mpl/sequences.h>
#include <yake/base/mpl/compare_sequences.h>
#include <yake/base/mpl/get_type_or_null.h>
#include <yake/base/mpl/dispatch_arbitrary_types.h>

namespace yake
{
namespace gui
{

using namespace yake::base::mpl;

/* compile time flags */
// dll import/export
#if defined( YAPP_GUI_EXPORTS )
#	define YAPP_GUI_API DLLEXPORT
#else
#	define YAPP_GUI_API DLLIMPORT
#endif


// enable/disable lua bindings
//#define YAKE_GUI_DISABLE_LUA

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

/* list: implemented types */
// properties
struct point;
typedef sequences::list
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
typedef sequences::list
	<
		button_base,
		static_text_base,
		multi_line_edit_box_base 
	> implemented_widgets;

/* helper: checks whether all implemented types are handled by a function or structure */
// accepts n types and returns a list of concrete types
template
<
	typename T1 = null_type, typename T2 = null_type, 
	typename T3 = null_type, typename T4 = null_type,
	typename T5 = null_type, typename T6 = null_type
>
struct get_type_list 
{
	typedef boost::mpl::list<T1, T2, T3, T4, T5, T6> given_types;
	typedef typename boost::mpl::remove_if< given_types, boost::is_same<boost::mpl::_, null_type> >::type type;
};

// accept n widgets and checks whether all widget types are handled or not at compile time
#define YAKE_STATIC_ASSERT_WIDGETS(WIDGET1, WIDGET2, WIDGET3, WIDGET4, WIDGET5, WIDGET6, WIDGET7, WIDGET8, WIDGET9, WIDGET10, WIDGET11, WIDGET12,	WIDGET13, WIDGET14, WIDGET15, WIDGET16) \
	YAKE_STATIC_ASSERT( \
		(compare_sequences /* condition */ \
		 < \
			 implemented_widgets, /* conditional types */ \
			 get_type_list /* implemented types */ \
			 < \
					typename yake::base::mpl::get_type_or_null<WIDGET1>::type, \
				  typename yake::base::mpl::get_type_or_null<WIDGET2>::type, \
				  typename yake::base::mpl::get_type_or_null<WIDGET3>::type \
			 >::type \
     >::type::value), \
    conditional_and_implemented_widget_lists_do_not_match) /* error */


} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONFIG_H_