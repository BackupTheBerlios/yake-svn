#ifndef _YAPP_GUI_CONFIG_H_
#define _YAPP_GUI_CONFIG_H_

// todo: remove
#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/not_equal_to.hpp>

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





// todo: use boost include iteration macros

// defines a type list of widgets according to the number of arguments
template 
<
	int number_of_arguments, 
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder
{
	typedef list<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::type types;
	typedef typename copy_if
	<
		types,
		vector,
		push_back<_1, _2>,
		not_equal_to<size<_1>::type::value, number_of_arguments> 
	>::type type;
};



	
/*; // throw compile time error

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>; // throw compile time error

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<1, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
{ 
	typedef sequences::list<T1> type; 
};

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<2, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
{ 
	typedef sequences::list<T1, T2> type; 
};

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<3, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
{ 
	typedef sequences::list<T1, T2, T3> type; 
};

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<4, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
{ 
	typedef sequences::list<T1, T2, T3, T4> type; 
};

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<5, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
{ 
	typedef sequences::list<T1, T2, T3, T4, T5> type; 
};

template 
<
	typename T1, typename T2,	typename T3, typename T4,	typename T5,
	typename T6, typename T7,	typename T8, typename T9,	typename T10,
	typename T11,	typename T12,	typename T13,	typename T14,	typename T15
>
struct list_holder<6, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
{ 
	typedef sequences::list<T1, T2, T3, T4, T5, T6> type; 
};
*/
// todo: 7,8,9,10,...

template
<
	typename T1 = null_type, typename T2 = null_type, 
	typename T3 = null_type, typename T4 = null_type,
	typename T5 = null_type, typename T6 = null_type
>
struct get_type_list : dispatch_arbitrary_types<list_holder, T1, T2, T3> {};

#define YAKE_STATIC_ASSERT_WIDGETS(WIDGET1, WIDGET2, WIDGET3, WIDGET4, WIDGET5, WIDGET6,
	WIDGET7, WIDGET8, WIDGET9, WIDGET10, WIDGET11, WIDGET12, WIDGET13, WIDGET14, WIDGET15) \
YAKE_STATIC_ASSERT( \
   (compare_sequences \
	 < \
	    implemented_widgets, /* conditional types */ \
      get_type_list /* implemented types */ \
      < \
        typename yake::base::mpl::get_type_or_null<WIDGET1>::type, \
        typename yake::base::mpl::get_type_or_null<WIDGET2>::type, \
        typename yake::base::mpl::get_type_or_null<WIDGET3>::type \
      >::type \
   >::type::value), \
   conditional_and_implemented_widget_lists_do_not_match)


} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONFIG_H_