#ifndef _YAKE_SAMPLES_COMMON_INITIALIZATION_H_
#define _YAKE_SAMPLES_COMMON_INITIALIZATION_H_

// stl
#include <vector>
#include <string>
// boost
#include <boost/shared_ptr.hpp>
#include <boost/mpl/push_front.hpp>
// yake
#include <yake/base/mpl/yakeInheritLinear.h>
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

// -----------------------------------------
/* automatic initialization */
namespace // unnamed
{
	// holds a reference to a system and makes it accessable
	template <class System>
	struct auto_init_system_holder
	{
		auto_initialization_system_holder()
			: m_system(create<System>())
		{}

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};


	// note: you cannot use gui without a renderer (todo: for win32 gui fake renderer needed?)
	template <>
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
	};
} // namespace unnamed

// loads the libraries, initializes the systems and makes them accessable
template <class Config = default_config>
struct auto_init : 
	load_libraries<Config>, 
	typename inherit_linear<typename Config::systems, lambda< auto_init_system_holder<_> >::type >::type
{
public: // types
	typedef auto_init_system_holder holder;

public: // methods
  template <class System>
  System & get_system()
  { static_cast<holder<System>&>(*this)->get_system(); }
};

// -----------------------------------------
/* delayed automatic initialization */
// loads the libraries, initializes the systems and makes them accessable (delayed)
template <class Config = default_config>
struct delayed_auto_init
{
public: // types
	typedef auto_init_system_holder holder;
	typedef typename inherit_linear
	<
		typename Config::systems, 
		lambda< holder<_> >::type
	>::type Systems;

public: // methods
	void initialize()
	{
		m_config.reset(new Config());
		m_systems.reset(new Systems());
	}

	template <class System>
	System & get_system()
	{ static_cast<holder<System>*>(m_systems.get())->get_system(); }

private: // data
	boost::shared_ptr<Config> m_config;
	boost::shared_ptr<Systems> m_systems;
};

// -----------------------------------------
/* semi automatic initialization */
namespace // unnamed
{
	struct head
	{
		virtual void load_systems() {}
	};

	template <class Base>
	struct semi_init_system_holder : Base
	{
		virtual void load_system() 
		{ m_system = create<System>(); Base::load_system(); }

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};

} // namespace unnamed

template <class Config = default_config>
struct semi_auto_init : 
	private typename inherit_linear
	<
		typename push_front<Config, head>::type, 
		typename lambda< semi_init_system_holder<_> >::type 
	>::type
{
public: // types
	typedef semi_init_system_holder holder;

public: // methods
		semi_auto_init()
		{
			load_libraries();
			load_systems();
		}

		virtual void load_libraries() = 0;

		template <class System>
		System & get_system()
    static_cast<holder<System>&>(*this)->get_system(); }
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
public: // types
	typedef manual_init_system_holder holder;

public: // methods
  template <class System>
  System & get_system()
  { static_cast<holder<System>&>(*this)->get_system(); }

  template <class System>
  void load_system(const char * id)
  { get_system<System>().set_system(create<System>(id)); }
};

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_INITIALIZATION_H_
