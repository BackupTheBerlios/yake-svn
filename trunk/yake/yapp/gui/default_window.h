#ifndef _DEFAULT_WINDOW_H_
#define _DEFAULT_WINDOW_H_

#include "window_base.h"
#include "cegui_window.h"

namespace yake
{
namespace gui 
{

// default_window is wrapping an default window_base implementation which allows us to
// to derive form it in lua (we can't derive from the concrete implementation) and we 
// don't have to hold a smart pointer to window_base (resp. the concrete type) in c++
class default_window : public window_base
{
public: // constructors
	default_window() {}
	default_window(const char * title) : m_impl(title) {}
  virtual ~default_window() {}

public: // methods
  virtual void run()
	{ m_impl.run(); }

public: // accessors
	virtual void set_title(const char * title)
	{ m_impl.set_title(title); }

	virtual const char * get_title() const
	{ return m_impl.get_title(); }

	virtual bool get_visible() const
	{ return m_impl.get_visible(); }

	virtual void set_visible(bool visible)
	{ m_impl.set_visible(visible); }

	virtual container_control_base & get_client_area()
	{ return m_impl.get_client_area(); }

	virtual const container_control_base::child_list & get_childs() const
	{ return m_impl.get_childs(); }

protected: // mutators
	virtual void apply(const info & this_info)
	{ m_impl.window_base::operator =(this_info); }

public: // mutators
	default_window & operator=(const info & this_info)
	{
		// set info
		apply(this_info);
		// chaining
		return *this;
	}

private: // data
	cegui::cegui_window m_impl; // todo: we should use the theme manager which has a default abstract factory
};

} // namespace gui
} // namespace yake

#endif // _DEFAULT_WINDOW_H_