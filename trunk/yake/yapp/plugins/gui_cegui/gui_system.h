#ifndef _YAPP_PLUGINS_GUI_CEGUI_GUI_SYSTEM_H_
#define _YAPP_PLUGINS_GUI_CEGUI_GUI_SYSTEM_H_

// yapp
#include <yapp/gui/gui_system_base.h>
#include <yapp/plugins/gui_cegui/controls.h>
#include <yapp/plugins/gui_cegui/config.h>

namespace yake
{
namespace gui
{
namespace cegui
{

struct YAPP_GUI_CEGUI_API gui_system
	: yake::base::mpl::concrete_factory
		<
			gui_system_base,
			implemented_widgets
		>
{};

} // namespace cegui
} // namespace gui
} // namespace yake

#endif // _YAPP_PLUGINS_GUI_CEGUI_GUI_SYSTEM_H_