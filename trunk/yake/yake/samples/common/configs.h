#ifndef _YAKE_SAMPLES_COMMON_CONFIGS_H_
#define _YAKE_SAMPLES_COMMON_CONFIGS_H_

// yake
#include <yake/base/mpl/sequences.h>
// local
#include "libraries.h"

using namespace yake::base::mpl::sequences;

namespace yake
{
namespace samples
{
namespace common
{

// -----------------------------------------
// system forward declares
struct graphics_system;
struct gui_system;
struct input_system;
struct physics_system;

// -----------------------------------------
// libraries for a specific config
template <class Config>
struct config_libraries;

// -----------------------------------------
// basic config
typedef list<graphics_system, gui_system, input_system, physics_system> basic_config;

template <>
struct config_libraries<basic_config>
{
	typedef basic_config config;

	// todo
	static std::vector<std::string> get_libraries()
	{ return libraries() << "graphicsOGRE.dll"; /*<< "cegui_plugin" << "ode_plugin"*/; }
};

// -----------------------------------------
// minimum config
typedef list<graphics_system, input_system> minimum_config;

template <>
struct config_libraries<minimum_config>
{
	typedef minimum_config config;

	// todo: base::native::Load_Library should add YAKE_DLL_POSTFIX (".dll" and the linux equivalent) to the library name
	static std::vector<std::string> get_libraries()
	{ return libraries() << "graphicsOGRE.dll"; }
};

// -----------------------------------------
// default config
typedef basic_config default_config;

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_CONFIGS_H_
