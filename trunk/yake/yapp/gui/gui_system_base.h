#ifndef _YAPP_GUI_SYSTEM_H_
#define _YAPP_GUI_SYSTEM_H_

// stl
#include <typeinfo>
// yake
#include <yake/base/mpl/abstract_factory.h>
// yapp
#include <yapp/gui/config.h>

namespace yake
{
namespace gui 
{

namespace // unnamed
{
	template <typename Widget>
	struct get_info_class_list
	{
		typedef boost::mpl::list<typename Widget::Info &> type;
	};
} // namespace unnamed

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
template <typename Widgets>
struct get_widget_infos_lists 
	: boost::mpl::transform< Widgets, typename boost::mpl::lambda< get_info_class_list<boost::mpl::_1> >::type >
{};

struct YAPP_GUI_API gui_system_base 
	: yake::base::mpl::abstract_factory
		<
			implemented_widgets,
			abstract_factory_unit_pars,
			typename get_widget_infos_lists<implemented_widgets>::type
		>
{
	// virtual destructor
  virtual ~gui_system_base() {}

	// todo: meta: not needed, because typeid(ptr) returns final type of ptr
	// returns type info of the concrete type used by renderer adapters
	//virtual const std::type_info & get_type_info() = 0;

	// class registry for concrete backends
	YAKE_DECLARE_REGISTRY_0(gui_system_base, yake::base::String)
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_SYSTEM_H_