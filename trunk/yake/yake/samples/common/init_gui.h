#ifndef _YAKE_SAMPLES_COMMON_INIT_GUI_H_
#define _YAKE_SAMPLES_COMMON_INIT_GUI_H_

// stl

// boost
#include <boost/shared_ptr.hpp>
// yake
#include <yake/base/type_info.h>
// local
#include "initialization.h"

using namespace yake::base;
using namespace yake::base::mpl;
using namespace yake::base::mpl::sequences;

namespace yake
{
namespace samples
{
namespace common
{
	// todo: del
	struct input_system {};
	struct physics_system {};

	struct graphics_system 
	{
		virtual const std::type_info & get_type_info()
		{ return typeid(graphics_system); }
	};
	struct gui_system
	{
		virtual const std::type_info & get_type_info()
		{ return typeid(gui_system); }
	};

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
	struct auto_init_system_holder<graphics_system, Parent>
	{
		// the user can add an adapter for a gui/renderer combination without changing the code
		auto_init_system_holder()
			: m_system(create<graphics_system>())
		{
			std::cout << "auto_init_system_holder<graphics_system, Parent>::auto_init_system_holder()\n";
		}

		boost::shared_ptr<graphics_system> get_system()
		{ return m_system; }

		boost::shared_ptr<graphics_system> m_system;
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
							static_cast<Parent*>(this)->get_system<graphics_system>()->get_type_info(),
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
