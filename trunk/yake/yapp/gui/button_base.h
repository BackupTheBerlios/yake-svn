#ifndef _YAPP_GUI_BUTTON_BASE_H_
#define _YAPP_GUI_BUTTON_BASE_H_

#include <reflection.h>

#include <bind_lua/bind_lua.h>
#undef free // todo: find conflict with tnl and move to bind_network.h
#include <bind_network/bind_network.h> // todo: remove

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "config.h"
#include "control_base.h"
#include "point.h"

#include "cegui_event_adaptor.h"
#include "cegui_util.h"

namespace yake
{
namespace gui
{

class button_base : public control_base
{
public: // types

	// these default values are used by the InfoBase template
	static struct defaults
	{
		static struct size { static float height, width; };
		static struct position { static float x, y; };
		static const char * caption;
	};
	
	// construction
  struct info : info_base<info, button_base>
  {
		typedef info_base<info, button_base> base;
		info() : m_caption(button_base::defaults::caption) {}
    info(const char * caption) : m_caption(caption) {}
		info & caption(const char * caption) { m_caption = caption; return *this; }

		template<typename Function, typename Class>
		info & clicked(Function ptr, Class cls) 
		{
			m_clicked_handlers.push_back(
				new ce_clicked_event_adaptor::handler(
					boost::bind(ptr, cls, _1))); 
			return *this; 
		}

#ifndef YAKE_GUI_DISABLE_LUA
		info & clicked(const luabind::functor<void> & lua_function)
		{
      m_clicked_lua_handlers.push_back(lua_function);
			return *this;
		}
#endif // YAKE_GUI_DISABLE_LUA

		std::string m_caption;
		ce_clicked_event_adaptor::handlers m_clicked_handlers;
		ce_clicked_event_adaptor::lua_functor_list m_clicked_lua_handlers;
  };

	// todo: define this using a macro
	struct cegui_widget_info
	{
		typedef CEGUI::PushButton widget;

		static std::string url()
		{ 
			static unsigned int current_id = 0;
			return std::string("push_button_url" + boost::lexical_cast<std::string>(current_id++)); 
		}

		static const char * class_name()
		{ return "TaharezLook/Button"; }
	};

public: // constructors
	button_base() 
		: m_impl(cegui::create_widget<cegui_widget_info>()),
			clicked(m_impl, CEGUI::String(CEGUI::PushButton::EventClicked).c_str(), *this),
			m_pos(defaults::position::x, defaults::position::y, boost::bind(set_pos, this, _1))
	{
		// defaults
		m_impl.setSize(CEGUI::Size(defaults::size::width, defaults::size::height));
		m_impl.setPosition(CEGUI::Point(defaults::position::x, defaults::position::y));
		m_impl.setText(defaults::caption);
	}

  button_base(const info & this_info) 
		: m_impl(cegui::create_widget<cegui_widget_info>()),
			clicked(m_impl, CEGUI::String(CEGUI::PushButton::EventClicked).c_str(), *this),
			m_pos(this_info.m_x, this_info.m_y, boost::bind(set_pos, this, _1))
	{
		// if the user has set a pointer then bind user pointer to this
		if(this_info.m_bind_to_this) *const_cast<info &>(this_info).m_bind_to_this = this;

		// set properties
		m_impl.setText(this_info.m_caption);
		m_impl.setSize(CEGUI::Size(this_info.m_w, this_info.m_h));
		m_impl.setPosition((this_info.m_abs_pos ? CEGUI::Absolute : CEGUI::Relative), 
			CEGUI::Point(this_info.m_x, this_info.m_y));

		// set handlers
		clicked.attach_handlers(this_info.m_clicked_handlers);
		clicked.attach_handlers(this_info.m_clicked_lua_handlers);
	}

public: // methods
  // implementation-specific accessors
	virtual CEGUI::Window & getImpl() const	
	{ return m_impl; }

private: // data
	CEGUI::PushButton & m_impl;

public:	// reflection
	GUI_CLASS(button_base, control_base);

	// events
	GUI_CUSTOM_EVENT(public, ce_clicked_event_adaptor, clicked);

	// properties
	const char * get_caption() const { return m_impl.getText().c_str(); }
	void set_caption(const char * caption) { m_impl.setText(caption); }
	GUI_PROPERTY(const char *, caption);

	bool get_visible() const { return m_impl.isVisible(); }
	void set_visible(bool visible) { m_impl.setVisible(visible); }
	GUI_PROPERTY(bool, visible);

	// todo: abs posm careful! relative window size 0.5f 0.5f halves the abs pos of the (button) widget, we need to_abs_pos_parent or so
	const point & get_pos() const
	{
		const_cast<point &>(m_pos).x = m_impl.getXPosition();
		const_cast<point &>(m_pos).y = m_impl.getYPosition();
		return m_pos; 
	}
	void set_pos(const point & position) 
	{ 
		m_impl.setXPosition(position.x);  
		m_impl.setYPosition(position.y);  
	}
	GUI_PROPERTY(const point &, pos);

private:
	point m_pos;
};

typedef button_base::info button;

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_BUTTON_BASE_H_