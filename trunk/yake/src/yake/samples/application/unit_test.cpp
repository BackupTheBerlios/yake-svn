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

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base;
using namespace yake::samples::common;

struct graphics_system {};
struct gui_system {};
struct input_system {};
struct physics_system {};

namespace yake
{
namespace testsuite
{

class my_app_auto : application< auto_init<> >
{};

class my_app_auto_mini : application< auto_init<minimum_config> >
{};

class my_app_auto_delayed : application< delayed_auto_init<minimum_config> >
{
	my_app_auto_delayed()
	{
		initialize();
	}
};

class my_app_semi_auto : application< semi_auto_init<minimum_config> >
{
	virtual void load_libraries()
	{	m_lib_man << "ogre3d_plugin" << "cegui_plugin" << "ode_plugin";	}

	library_manager m_lib_man;
};

class my_app_manual : application< manual_init<basic_config> >
{
   my_app_manual()
   {
     load_library("ogre3d_plugin");
     load_system<graphics_system>("yake.graphics.ogre3d");
     load_library("cegui_plugin");
     load_system<graphics_system>("yake.gui.cegui");
     m_gui_renderer_adapter.reset(
		 create<gui_renderer_adapter>(
			gui_renderer_adapter::identifier(
				get_system<graphics_system>()->get_type_info(), 
				get_system<gui_system>()->get_type_info())));
   }

	 void load_library(const char * lib)
	 { m_lib_man.load_library(lib); }

	 library_manager m_lib_man;
	 boost::shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
};


} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}




