// yake
#include <yake/base/yakeLog.h>
// yapp
#include <yapp/gui/gui_system_base.h>

namespace yake
{
namespace gui
{

static boost::shared_ptr<gui_system_base> static_system = boost::shared_ptr<gui_system_base>();

void set_gui_system(boost::shared_ptr<gui_system_base> system)
{
	static_system = system;
}

boost::shared_ptr<gui_system_base> get_gui_system()
{
	if(!static_system)
	{
		yake::base::log_warning("get_gui_system()", "user did not sepcify a gui system - using default");
		static_system = yake::base::templates::create_default<gui_system_base>();
	}
	return static_system;
}

} // namespace gui
} // namespace yake