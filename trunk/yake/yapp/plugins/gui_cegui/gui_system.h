#ifndef _YAPP_GUI_SYSTEM_H_
#define _YAPP_GUI_SYSTEM_H_

// stl
#include <typeinfo>
// boost
#include <boost/shared_ptr.hpp>
#ifndef _YAPP_PLUGINS_GUI_CEGUI_GUI_SYSTEM_H_
#define _YAPP_PLUGINS_GUI_CEGUI_GUI_SYSTEM_H_

// yapp
#include <yapp/gui/gui_system_base.h>

namespace yake
{
namespace gui
{
namespace cegui
{

struct YAPP_GUI_API gui_system
	: yake::base::mpl::concrete_factory
		<
			gui_system_base,
			implemented_widgets_concrete // todo: should be abstract_widgets, concrete_widgets or something
		>
{

};


} // namespace cegui
} // namespace gui
} // namespace yake

#endif // _YAPP_PLUGINS_GUI_CEGUI_GUI_SYSTEM_H_