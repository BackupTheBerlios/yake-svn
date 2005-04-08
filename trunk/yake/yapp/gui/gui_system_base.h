#ifndef _YAPP_GUI_SYSTEM_H_
#define _YAPP_GUI_SYSTEM_H_

// stl
#include <typeinfo>
// boost
#include <boost/shared_ptr.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/lambda.hpp>
// yake
#include <yake/base/mpl/abstract_factory_pars.h>
#include <yake/base/templates/yakeRegistry.h>
// yapp
#include <yapp/gui/config.h>
#include <yapp/gui/widgets.h>

namespace yake
{
namespace gui 
{

/* create a list holding the constructor arguments for the abstract factory, currently
   only the info class is passed to the widget constructor, so each list holds one argument (the info class).
	 result:
		list
		<
			list<widget1::info &>,
			list<widget2::info &>,
			...
		>
*/
namespace // unnamed
{
	template <typename Widget>
	struct get_info_class_list
	{
		typedef boost::mpl::list<typename Widget::info &> type;
	};
} // namespace unnamed

template <typename Widgets>
struct get_widget_infos_lists 
	: boost::mpl::transform
		< 
			Widgets, 
			typename boost::mpl::lambda< get_info_class_list<boost::mpl::_1> >::type 
		>
{};

struct YAPP_GUI_API gui_system_base 
	: yake::base::mpl::abstract_factory
		<
			available_widgets, // products
			yake::base::mpl::abstract_factory_unit_pars, // creator
			typename get_widget_infos_lists<available_widgets>::type // constructor args
		>
{
	// virtual destructor
  virtual ~gui_system_base() {}

	// todo: meta: not needed, because typeid(ptr) returns final type of ptr
	// returns type info of the concrete type used by renderer adapters
	//virtual const std::type_info & get_type_info() = 0;

	// class registry for concrete backends
	YAKE_DECLARE_REGISTRY_0(gui_system_base, yake::base::String);
};

void set_gui_system(boost::shared_ptr<gui_system_base> system);
boost::shared_ptr<gui_system_base> get_gui_system();

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_SYSTEM_H_