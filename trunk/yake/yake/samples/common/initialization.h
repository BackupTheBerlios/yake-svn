#ifndef _YAKE_SAMPLES_COMMON_INITIALIZATION_H_
#define _YAKE_SAMPLES_COMMON_INITIALIZATION_H_

// stl
#include <vector>
#include <string>
// boost
#include <boost/shared_ptr.hpp>
// yake
#include <yake/base/mpl/inherit_linear.h>
#include <yake/base/mpl/yakeSequences.h>
// local
#include "load_libraries.h"

using namespace yake::base;
using namespace yake::base::mpl;
using namespace yake::base::mpl::sequences;

namespace yake
{
namespace samples
{
namespace common
{

// todo: use yake registry
template <class System>
boost::shared_ptr<System> create()
{ return boost::shared_ptr<System>(new System()); }

// -----------------------------------------
/* automatic initialization */
namespace // unnamed
{
	// holds a reference to a system and makes it accessable
	template <class System>
	struct auto_init_system_holder
	{
		auto_init_system_holder()
			: m_system(create<System>())
		{}

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};


	// todo: put this somewhere else?
	// note: you cannot use gui without a renderer (todo: for win32 gui fake renderer needed?)
	/*template <>
	struct auto_init_system_holder<gui_system>
	{
		// the user can add an adapter for a gui/renderer combination without changing the code
		auto_init_system_holder()
			: m_system(create<gui_system>()),
			  m_gui_renderer_adapter(
				create<gui_renderer_adapter>(
					gui_renderer_adapter::identifier(
						static_cast<auto_init_system_holder<graphics_system>*>(this)->get_system()->get_type_info(),
						m_system->get_type_info())))
		{}

		boost::shared_ptr<gui_system> get_system()
		{ m_system.reset(system); }

		boost::shared_ptr<gui_system> m_system;
		boost::shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
	};*/
} // namespace unnamed

// loads the libraries, initializes the systems and makes them accessable
template <class Config = default_config>
struct auto_init : 
	load_libraries<Config>, 
	inherit_linear<Config, typename lambda< auto_init_system_holder<_> >::type >::type
{
public: // methods
  template <class System>
  System & get_system()
  { static_cast<auto_init_system_holder<System>&>(*this)->get_system(); }
};

// -----------------------------------------
/* delayed automatic initialization */
// loads the libraries, initializes the systems and makes them accessable (delayed)
template <class Config = default_config>
struct delayed_auto_init
{
public: // types
	typedef typename inherit_linear
	<
		Config, 
		typename lambda< auto_init_system_holder<_> >::type
	>::type Systems;

public: // methods
	void initialize()
	{
		m_config.reset(new Config());
		m_systems.reset(new Systems());
	}

	template <class System>
	System & get_system()
	{ static_cast<auto_init_system_holder<System>*>(m_systems.get())->get_system(); }

private: // data
	boost::shared_ptr<Config> m_config;
	boost::shared_ptr<Systems> m_systems;
};

// -----------------------------------------
/* semi automatic initialization */
namespace // unnamed
{
	template <class System>
	struct semi_init_system_holder_head
	{
		virtual void load_systems() {}
	};

	template <class System>
	struct semi_init_system_holder
	{
		virtual void load_system() 
		{ m_system = create<System>(); Base::load_system(); }
		// ############################################################### work on here tomorrow
		// WE NEED BASE HERE => LINEAR INHERITANCE ... SOLVED THIS, SEE ABSTRACT FACTORY SAMPLE

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};

} // namespace unnamed

template <class Config = default_config>
struct semi_auto_init : 
	private typename inherit_linear
	<
		Config, 
		typename lambda< semi_init_system_holder<_> >::type,
		semi_init_system_holder_head
	>::type
{
public: // methods
		semi_auto_init()
		{
			load_libraries();
			load_systems();
		}

		virtual void load_libraries() = 0;

		template <class System>
		System & get_system()
    static_cast<semi_init_system_holder<System>&>(*this)->get_system(); }
};

// -----------------------------------------
/* manual initialization */
namespace // unnamed
{
	template <class System>
	struct manual_init_system_holder
	{
		void set_system(shared_ptr<System> system)
		{ m_system.reset(system); }

		shared_ptr<System> get_system()
		{ return m_system; }

		shared_ptr<System> m_system;
	};
} // namspace unnamed

template <class Systems>
struct manual_init : inherit_linear<Systems, lambda< manual_init_system_holder<_> >::type
{
public: // methods
  template <class System>
  System & get_system()
  { static_cast<manual_init_system_holder<System>&>(*this)->get_system(); }

  template <class System>
  void load_system(const char * id)
  { get_system<System>().set_system(create<System>(id)); }
};

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_INITIALIZATION_H_
