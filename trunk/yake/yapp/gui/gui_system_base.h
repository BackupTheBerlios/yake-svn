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

struct YAPP_GUI_API gui_system_base : yake::base::mpl::abstract_factory<implemented_widgets>
{
	// virtual destructor
  virtual ~gui_system_base() {}

	// returns type info of the concrete type used by renderer adapters
	virtual const std::type_info & get_type_info() = 0;

	// class registry for concrete backends
	YAKE_DECLARE_REGISTRY_0(gui_system_base, yake::base::String)
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_SYSTEM_H_