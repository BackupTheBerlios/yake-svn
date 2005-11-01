#ifndef _YAPP_PLUGINS_CEGUI_WINDOW_H_
#define _YAPP_PLUGINS_CEGUI_WINDOW_H_

#include <boost/lexical_cast.hpp>

#include <yapp/gui/window_base.h>
#include "util.h"

namespace yake
{
namespace gui
{
namespace cegui
{

class cegui_window : public window_base
{
public: // constructors
	cegui_window()
		: m_window(cegui::create_widget<cegui_widget_info>()),
			m_client_area(m_window)
	{
		windows.push_back(this);

		// defaults
		m_window.setSize(CEGUI::Size(defaults::size::width, defaults::size::height));
		m_window.setPosition(CEGUI::Point(defaults::position::x, defaults::position::y));
		m_window.setText(defaults::title);
	}

  cegui_window(const char * title)                                                   
		: m_window(cegui::create_widget<cegui_widget_info>()),
			m_client_area(m_window) 
	{
		windows.push_back(this);

		// defaults
		m_window.setSize(CEGUI::Size(defaults::size::width, defaults::size::height));
		m_window.setPosition(CEGUI::Point(defaults::position::x, defaults::position::y));

		// user
		m_window.setText(title);
	}

	// todo: not needed?
  /*cegui_window(const info & this_info) 
		: m_window(cegui_util::create_widget<cegui_widget_info>()),
			m_client_area(m_window) 
	{
		windows.push_back(this);

		m_window.setText(this_info.m_title);
		m_window.setSize(CEGUI::Size(this_info.m_w, this_info.m_h));
		m_window.setPosition(CEGUI::Point(this_info.m_x, this_info.m_y));
		m_window.setSizingEnabled(this_info.m_sizing);
	}*/

	virtual ~cegui_window() 
	{
		// todo: temporary: luas' garbage collecting destroys the windows created by lua
		// but the window is still in the c++ window list, so when lua tries to iterate through
		// the windows, there would be a null pointer and it crashes

		// todo: window manager should handle this, take care about copy constructor, assignment operations etc.
		//       copy constructor and assignment operator as private, nobody should make a copy of a window!
		//       so we don't have to reference count, just unregister the window within the deconstructor!
		//       FIRST!!! => window manager
		windows.erase(std::find(windows.begin(), windows.end(), this));		
	}

public: // methods
  virtual void run()                                                                   
	{ cegui::get_root_widget().addChildWindow(&m_window); }

protected: // mutators
	virtual void apply(const info & this_info)
	{
		m_window.setText(this_info.m_title);
		m_window.setSize(CEGUI::Size(this_info.m_w, this_info.m_h));
		m_window.setPosition(CEGUI::Point(this_info.m_x, this_info.m_y));
		m_window.setSizingEnabled(this_info.m_sizing);
		m_window.setVisible(this_info.m_visible);
	}

public: // accessors
	virtual const char * get_title() const
	{ return m_window.getText().c_str(); }

	virtual void set_title(const char * title)
	{ m_window.setText(title); }

	virtual bool get_visible() const
	{ return m_window.isVisible(); }

	virtual void set_visible(bool visible)
	{ m_window.setVisible(visible); }

  virtual container_control_base & get_client_area()                                        
  { return m_client_area; }

	virtual const container_control_base::child_list & get_childs() const
	{ return m_client_area.get_childs(); }
  
private:
  struct client_area : public container_control_base
  {
    // construction
    client_area(CEGUI::FrameWindow & window) 
      : m_window(window)
		{}

	  virtual const child_list & get_childs() const
	  {	return m_childs; }

		virtual void remove(const control_base & child)
		{ m_childs.remove(&child); }

    // mutators
    virtual void add(const control_base & child)                                
		{
			m_childs.push_back(&child);

			CEGUI::Window & widget = const_cast<control_base&>(child).getImpl(); 
			m_window.addChildWindow(&widget);
			widget.show();
		}
 
		// implementaton-specific accessors
		virtual CEGUI::FrameWindow & getImpl() const                                        
		{ return m_window; }

    CEGUI::FrameWindow & m_window;
	  child_list m_childs;
	};

private: // data
  CEGUI::FrameWindow & m_window;
  client_area m_client_area;

  YAKE_DEFINE_CEGUI_WIDGET_INFO(CEGUI::FrameWindow, "window_url", "TaharezLook/FrameWindow");
};

} // namespace cegui
} // namespace gui
} // namespace yake

#endif // _YAPP_PLUGINS_CEGUI_WINDOW_H_