//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// std
#include <memory>
#include <string>
// yake
#include <yake/samples/common/application.h>
#include <yake/samples/common/initialization.h>
#include <yake/samples/common/configs.h>
#include <yake/samples/common/library_manager.h>
#include <yake/samples/common/ext_lib_man.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base;
using namespace yake::samples::common;

namespace yake 
{
namespace samples
{
namespace common
{
	struct graphics_system {};
	struct gui_system {};
	struct input_system {};
	struct physics_system {};
} // namespace common
} // namespace samples
} // namespace yake

namespace yake
{
namespace testsuite
{

class my_app_auto : application< auto_init<> >
{};

struct my_app_auto_mini : application< auto_init<minimum_config> >
{};

struct my_app_auto_delayed : application< delayed_auto_init<minimum_config> >
{
	my_app_auto_delayed()
	{	initialize();	}
};

struct my_app_semi_auto : application<semi_auto_init<my_app_semi_auto, minimum_config>, ext_lib_man>
{
	void load_libraries()
	{	get_lib_man() << "ogre3d_plugin" << "cegui_plugin" << "ode_plugin";	}
};

struct my_app_manual : application< manual_init<basic_config> >
{
   my_app_manual()
   {
     load_library("ogre3d_plugin");
     load_system<graphics_system>("yake.graphics.ogre3d");
     load_library("cegui_plugin");
     load_system<graphics_system>("yake.gui.cegui");
     /*m_gui_renderer_adapter.reset(
			create<gui_renderer_adapter>(
				gui_renderer_adapter::identifier(
					get_system<graphics_system>()->get_type_info(), 
					get_system<gui_system>()->get_type_info())));*/
   }

	 void load_library(const char * lib)
	 { m_lib_man.load_library(lib); }

	 library_manager m_lib_man;
	 //boost::shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
};


} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	using namespace yake::testsuite;
	// automatic
	my_app_auto app_auto;
	my_app_auto_mini app_auto_mini;
	my_app_auto_delayed app_auto_delayed;
	// semi automatic
	my_app_semi_auto app_semi_auto;
	// manual
	my_app_manual app_manual;

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}




