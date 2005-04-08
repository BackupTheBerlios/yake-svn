#ifndef _YAPP_GUI_WINDOW_BASE_H_
#define _YAPP_GUI_WINDOW_BASE_H_

#include <vector>

#include "config.h"
#include "control_base.h"
#include "container_control_base.h"

namespace yake
{
namespace gui
{

class window_base
{
public:
	typedef std::vector<const window_base *> window_list; // todo: move to window manager
	static window_list windows; 

public: // types

	static struct defaults
	{
		static struct size { static float height, width; };
		static struct position { static float x, y; };
		static const char * title;
	};

	struct info : control_base::info_base<info, window_base>
  {
		typedef control_base::info_base<info, window_base> base;
		info() : m_title(window_base::defaults::title) {}
		info(const char * title) : m_title(title) {}
		info & sizing_enabled(bool enable_)	{ m_sizing = enable_; return *this; }
		info & title(const char * title_) { m_title = title_; return *this; }
		bool m_sizing;
		std::string m_title;
  };

public: // constructors
  virtual ~window_base() {}

public: // methods
  virtual void run() = 0;

public: // accessors
	virtual void set_title(const char * title) = 0;
	virtual const char * get_title() const = 0;

	virtual bool get_visible() const = 0;
	virtual void set_visible(bool visible) = 0;

	virtual container_control_base & get_client_area() = 0;
	virtual const container_control_base::child_list & get_childs() const = 0;

protected: // mutators
	virtual void apply(const info & this_info) = 0;

public: // mutators
	window_base & operator=(const info & this_info)
	{
		// set info
		apply(this_info);
		// chaining
		return *this;
	}

#ifndef YAKE_GUI_DISABLE_LUA
public: // luabind helpers
	template <typename ConcreteInfo>
	window_base & add(const ConcreteInfo & info) // 'const control_base::info_base<ConcreteInfo, ConcreteControl> & info' cannot bind this to lua
	{
		// add child to client area
		get_client_area() << info;
		// chaining
		return *this;
	}
#endif // YAKE_GUI_DISABLE_LUA
};

template<class ConcreteControl>
typename ConcreteControl::info create(const char * name)
{	return typename ConcreteControl::info(name); }

template<typename ConcreteInfo, typename ConcreteControl>
window_base & operator<<(window_base & window, const control_base::info_base<ConcreteInfo, ConcreteControl> & info)
{
  window.get_client_area() << info;
  return window;
}

template<typename ConcreteInfo, typename ConcreteControl>
window_base & operator<(window_base & window, const control_base::info_base<ConcreteInfo, ConcreteControl> & info) 
{ return window << info; }

template<typename ConcreteInfo, typename ConcreteControl>
window_base & operator^(window_base & window, const control_base::info_base<ConcreteInfo, ConcreteControl> & info)
{ return window << info; }

// todo: win manager, singleton widget factory should hold a list of created windows
const window_base::window_list & get_windows();

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_WINDOW_BASE_H_