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
#include <typeinfo>
// yake
#include <yake/samples/common/application.h>
#include <yake/samples/common/initialization.h>
#include <yake/samples/common/configs.h>
#include <yake/samples/common/library_manager.h>
#include <yake/samples/common/ext_lib_man.h>

#include <yake/samples/common/init_gui.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace testsuite {

using namespace yake::base;
using namespace yake::samples::common;

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
	{	get_lib_man() << "graphicsOGRE.dll"; /* << "cegui_plugin" << "ode_plugin";*/ }
};

struct my_app_manual : application<manual_init<basic_config>, ext_lib_man>
{
   my_app_manual()
   {
     load_library("graphicsOGRE.dll");
     load_system<yake::graphics::IGraphicsSystem>("yake.graphics.ogre3d");
     /*load_library("cegui_plugin");
     load_system<gui_system>("yake.gui.cegui");
     m_gui_renderer_adapter =
			create<gui_renderer_adapter>(
				gui_renderer_adapter::identifier(
					get_system<graphics_system>()->get_type_info(), 
					get_system<gui_system>()->get_type_info()));*/
   }

	 void load_library(const char * lib)
	 { get_lib_man() << lib; }

	 boost::shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
};


} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	using namespace yake::testsuite;

	{ // automatic
		std::cout << "- automatic -\n";
		my_app_auto app_auto;
	}

	{ // mini automatic
		std::cout << "\n- automatic mini -\n";
		my_app_auto_mini app_auto_mini;
	}

	{ // delayed automatic
		std::cout << "\n- automatic delayed -\n";
		my_app_auto_delayed app_auto_delayed;
	}

	{ // semi automatic
		std::cout << "\n- semi automatic -\n";
		my_app_semi_auto app_semi_auto;
	}

	{ // manual
		std::cout << "\n- manual -\n";
		my_app_manual app_manual;
	}

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}