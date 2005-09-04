#ifndef _YAPP_PLUGINS_GUI_CEGUI_CONTROLS_H_
#define _YAPP_PLUGINS_GUI_CEGUI_CONTROLS_H_

// yake
#include <yake/base/mpl/sequences.h>
// yapp
#include <yapp/plugins/gui_cegui/widgets/button.h>

namespace yake
{
namespace gui
{
namespace cegui
{

/* list: implemented types */
// widgets
typedef sequences::list
	<
		button,
		static_text,
		multi_line_edit_box 
	> implemented_widgets;

} // namespace cegui
} // namespace gui
} // namespace yake

#endif // _YAPP_PLUGINS_GUI_CEGUI_CONTROLS_H_