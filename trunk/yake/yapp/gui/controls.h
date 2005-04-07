#ifndef _YAPP_GUI_CONTROLS_H_
#define _YAPP_GUI_CONTROLS_H_

// boost
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/remove_if.hpp>
// yake
#include <yake/base/mpl/sequences.h>
#include <yake/base/mpl/compare_sequences.h>
#include <yake/base/mpl/get_type_or_null.h>
#include <yake/base/mpl/dispatch_arbitrary_types.h>
#include <yake/base/mpl/yakeStaticAssert.h>
// yapp
#include <yapp/gui/button_base.h>
#include <yapp/gui/static_text_base.h>
#include <yapp/gui/multi_line_edit_box_base.h>

namespace yake
{
namespace gui
{

using namespace yake::base::mpl;

/* list: implemented types */
// properties
typedef sequences::list
	<
		bool,
		float,
		const char *,
		const point &	
	> available_properties;

// widgets
typedef sequences::list
	<
		button_base,
		static_text_base,
		multi_line_edit_box_base 
	> available_widgets;

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

// accept n properties and checks whether all widget types are handled or not at compile time
#define YAKE_STATIC_ASSERT_PROPERTIES(PROP1, PROP2, PROP3, PROP4, PROP5, PROP6, PROP7, PROP8, PROP9, PROP10, PROP11, PROP12, PROP13, PROP14, PROP15, PROP16) \
	YAKE_STATIC_ASSERT( \
		(compare_sequences /* condition */ \
		 < \
			 available_properties, /* conditional types */ \
			 get_type_list /* implemented types */ \
			 < \
				  typename yake::base::mpl::get_type_or_null<PROP1>::type, \
				  typename yake::base::mpl::get_type_or_null<PROP2>::type, \
				  typename yake::base::mpl::get_type_or_null<PROP3>::type, \
				  typename yake::base::mpl::get_type_or_null<PROP4>::type \
			 >::type \
     >::type::value), \
    conditional_and_available_properties_lists_do_not_match) /* error */

// accept n widgets and checks whether all widget types are handled or not at compile time
#define YAKE_STATIC_ASSERT_WIDGETS(WIDGET1, WIDGET2, WIDGET3, WIDGET4, WIDGET5, WIDGET6, WIDGET7, WIDGET8, WIDGET9, WIDGET10, WIDGET11, WIDGET12,	WIDGET13, WIDGET14, WIDGET15, WIDGET16) \
	YAKE_STATIC_ASSERT( \
		(compare_sequences /* condition */ \
		 < \
			 available_widgets, /* conditional types */ \
			 get_type_list /* implemented types */ \
			 < \
				  typename yake::base::mpl::get_type_or_null<WIDGET1>::type, \
				  typename yake::base::mpl::get_type_or_null<WIDGET2>::type, \
				  typename yake::base::mpl::get_type_or_null<WIDGET3>::type \
			 >::type \
     >::type::value), \
    conditional_and_available_widget_lists_do_not_match) /* error */

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONTROLS_H_