#ifndef _YAKE_SAMPLES_COMMON_INITIALIZATION_H_
#define _YAKE_SAMPLES_COMMON_INITIALIZATION_H_

// stl
#include <vector>
#include <string>
#include <iostream>
// boost
#include <boost/shared_ptr.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/reverse.hpp>
// yake
#include <yake/base/mpl/inherit_linear.h>
#include <yake/base/mpl/inherit_multiple.h>
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

template <class System>
boost::shared_ptr<System> create(const char * id)
{ return boost::shared_ptr<System>(new System()); }

// -----------------------------------------
/* automatic initialization */
namespace // unnamed
{
	// holds a reference to a system and makes it accessable
	template <class System, class Parent>
	struct auto_init_system_holder
	{
		auto_init_system_holder()
			: m_system(create<System>())
		{}

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};
} // namespace unnamed

// loads the libraries, initializes the systems and makes them accessable
template <class Config = default_config>
struct auto_init : 
	private load_libraries<Config>, 
	private inherit_multiple
	<
		typename boost::mpl::reverse<Config>::type, // note: correcting initialization order, otherwise graphics system would be initialized after gui
		typename lambda< auto_init_system_holder<_, auto_init> >::type
	>::type
{
  template <class System>
  boost::shared_ptr<System> get_system()
  { return static_cast<auto_init_system_holder<System, auto_init>*>(this)->get_system(); }
};

// -----------------------------------------
/* delayed automatic initialization */
// loads the libraries, initializes the systems and makes them accessable (delayed)
template <class Config = default_config>
struct delayed_auto_init
{
public: // types
	typedef typename inherit_multiple
	<
		typename boost::mpl::reverse<Config>::type, // note: correcting initialization order, otherwise graphics system would be initialized after gui
		typename lambda< auto_init_system_holder<_, delayed_auto_init> >::type
	>::type Systems;

public: // methods
	void initialize()
	{
		m_config.reset(new Config());
		m_systems.reset(new Systems());
	}

	template <class System>
	boost::shared_ptr<System> get_system()
	{ return static_cast<auto_init_system_holder<System, delayed_auto_init>*>(m_systems.get())->get_system(); }

private: // data
	boost::shared_ptr<Config> m_config;
	boost::shared_ptr<Systems> m_systems;
};

// -----------------------------------------
/* semi automatic initialization */
namespace // unnamed
{
	struct semi_init_system_holder_root
	{
		virtual void load_systems() {}
	};

	template <class Base, class System>
	struct semi_init_system_holder : Base
	{
		void load_systems() 
		{ m_system = create<System>(); Base::load_systems(); }

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};
} // namespace unnamed

template <class Parent, class Config = default_config>
struct semi_auto_init : 
	private inherit_linear
	<
		Config, 
		typename lambda< semi_init_system_holder<_, _> >::type,
		semi_init_system_holder_root
	>::type
{
		semi_auto_init()
		{
			static_cast<Parent*>(this)->load_libraries();
			load_systems();
		}

		template <class System>
		boost::shared_ptr<System> get_system()
		{
			// create list of bases
			typedef boost::mpl::find<Config, System>::type last;
			typedef boost::mpl::iterator_range<typename boost::mpl::begin<Config>::type, next<last>::type>::type bases;
			// cast to base
			inherit_linear
			< 
				Config, 
				typename lambda< semi_init_system_holder<_, _> >::type,
				semi_init_system_holder_root
			>::type & base = *this;
			// get system
			return base.get_system();
		}
};

// -----------------------------------------
/* manual initialization */
namespace // unnamed
{
	template <class System>
	struct manual_init_system_holder
	{
		void set_system(boost::shared_ptr<System> system)
		{ m_system = system; }

		boost::shared_ptr<System> get_system()
		{ return m_system; }

		boost::shared_ptr<System> m_system;
	};
} // namspace unnamed

template <class Systems>
struct manual_init : private inherit_multiple<Systems, typename lambda< manual_init_system_holder<_> >::type >::type
{
  template <class System>
  boost::shared_ptr<System> get_system()
  { return static_cast<manual_init_system_holder<System>*>(this)->get_system(); }

  template <class System>
  void load_system(const char * id)
  { return static_cast<manual_init_system_holder<System>*>(this)->set_system(create<System>(id)); }
};

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_INITIALIZATION_H_
