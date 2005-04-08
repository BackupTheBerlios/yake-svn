#ifndef _UTIL_H_
#define _UTIL_H_

#include <CEGUI.h>

namespace yake
{
namespace gui
{
namespace cegui
{

#define YAKE_DEFINE_CEGUI_WIDGET_INFO(CONCRETE_WIDGET, URL_PREFIX, CLASS_NAME) \
public: \
	struct cegui_widget_info \
	{ \
		typedef CONCRETE_WIDGET widget; \
\
		static std::string url() \
		{ \
			static unsigned int current_id = 0; \
			return std::string(URL_PREFIX + boost::lexical_cast<std::string>(current_id++)); \
		} \
\
		static const char * class_name() \
		{ return CLASS_NAME; } \
	}

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