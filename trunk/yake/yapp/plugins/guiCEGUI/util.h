#ifndef _UTIL_H_
#define _UTIL_H_

#include <CEGUI.h>

namespace yake
{
namespace gui
{
namespace cegui
{

template<class WidgetInfo>
static typename WidgetInfo::widget & create_widget()
{
	return static_cast<typename WidgetInfo::widget&>(
		*CEGUI::WindowManager::getSingleton().createWindow(
			WidgetInfo::class_name(), WidgetInfo::url()));
}

static CEGUI::Window & get_widget(const char * title)
{	return *CEGUI::WindowManager::getSingleton().getWindow(title); }

static CEGUI::Window & get_root_widget()
{	return get_widget("root_wnd"); } // todo: use sheet not string id, because we have to set the root window/sheet manually at startup

} // namespace cegui
} // namespace gui
} // namespace yake


#endif // _UTIL_H_