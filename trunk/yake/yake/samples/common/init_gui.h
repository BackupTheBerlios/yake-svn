#ifndef _YAKE_SAMPLES_COMMON_INIT_GUI_H_
#define _YAKE_SAMPLES_COMMON_INIT_GUI_H_

// boost
#include <boost/shared_ptr.hpp>
// yake
#include <yake/base/yake.h>
#include <yake/graphics/yakeGraphicsSystem.h>
// local
#include "initialization.h"

namespace yake 
{
namespace samples 
{
namespace common 
{

// todo: del
template <>
struct create_help <yake::graphics::IGraphicsSystem>
{
	static boost::shared_ptr<yake::graphics::IGraphicsSystem> create()
	{ return yake::base::templates::create<yake::graphics::IGraphicsSystem>(); }
};

using namespace yake::base;

	// system needs a pure virtual function get_type_info and concrete impl (plugin) needs to use return_type_info
	// HOWEVER this is not really useful ... just define the pure virtual function within the interface and the impl
	// within the plugin ... or?
	template <class Parent>
	struct return_type_info
	{
		virtual const std::type_info & get_type_info()
		{ return typeid(Parent); }
	};

	// todo: del
	struct input_system {};
	struct physics_system {};

	//struct graphics_system : return_type_info<graphics_system> {};
	struct gui_system : return_type_info<gui_system>	{};

	struct gui_renderer_adapter 
	{ 
		typedef std::pair<base::type_info, base::type_info> identifier; 
	};

	template <class System>
	boost::shared_ptr<System> create(const gui_renderer_adapter::identifier & id)
	{ return boost::shared_ptr<System>(new gui_renderer_adapter()); }

// -----------------------------------------
/* automatic initialization */
namespace // unnamed
{

	// todo: del, just for constructor order test
	template <class Parent>
	struct auto_init_system_holder<input_system, Parent>
	{
		// the user can add an adapter for a gui/renderer combination without changing the code
		auto_init_system_holder()
			: m_system(create<input_system>())
		{
			std::cout << "auto_init_system_holder<input_system, Parent>::auto_init_system_holder()\n";
		}

		boost::shared_ptr<input_system> get_system()
		{ return m_system; }

		boost::shared_ptr<input_system> m_system;
	};

	template <class Parent>
	struct auto_init_system_holder<yake::graphics::IGraphicsSystem, Parent>
	{
		// the user can add an adapter for a gui/renderer combination without changing the code
		auto_init_system_holder()
			: m_system(create<yake::graphics::IGraphicsSystem>())
		{
			std::cout << "auto_init_system_holder<yake::graphics::IGraphicsSystem, Parent>::auto_init_system_holder()\n";
		}

		boost::shared_ptr<yake::graphics::IGraphicsSystem> get_system()
		{ return m_system; }

		boost::shared_ptr<yake::graphics::IGraphicsSystem> m_system;
	};


	// note: you cannot use gui without a graphics system (todo: for win32 gui fake graphics system needed?)
	template <class Parent>
	struct auto_init_system_holder<gui_system, Parent>
	{
		// the user can add an adapter for a gui/renderer combination without changing the code
		auto_init_system_holder()
			: m_system(create<gui_system>()),
			  m_gui_renderer_adapter(
					create<gui_renderer_adapter>(
						gui_renderer_adapter::identifier(
							static_cast<Parent*>(this)->get_system<yake::graphics::IGraphicsSystem>()->get_type_info(),
							m_system->get_type_info())))
		{
			std::cout << "auto_init_system_holder<gui_system, Parent>::auto_init_system_holder()\n";
		}

		boost::shared_ptr<gui_system> get_system()
		{ return m_system; }

		boost::shared_ptr<gui_system> m_system;
		boost::shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
	};
} // namespace unnamed

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_INIT_GUI_H_
