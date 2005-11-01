#ifndef _YAPP_PLUGINS_GUI_CEGUI_WIDGETS_BUTTON_H_
#define _YAPP_PLUGINS_GUI_CEGUI_WIDGETS_BUTTON_H_

#include <yapp/gui/widgets/button_base.h>

namespace yake
{
namespace gui
{
namespace cegui
{

class button : public button_base
{
public: // constructors
	button() 
		: m_impl(cegui::create_widget<cegui_widget_info>()),
			clicked(m_impl, CEGUI::String(CEGUI::PushButton::EventClicked).c_str(), *this),
			m_pos(defaults::position::x, defaults::position::y, boost::bind(set_pos, this, _1))
	{
		// defaults
		m_impl.setSize(CEGUI::Size(defaults::size::width, defaults::size::height));
		m_impl.setPosition(CEGUI::Point(defaults::position::x, defaults::position::y));
		m_impl.setText(defaults::caption);
	}

  button(const info & this_info) 
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
	GUI_CLASS(button, button_base);

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

  YAKE_DEFINE_CEGUI_WIDGET_INFO(CEGUI::PushButton, "push_button_url", "TaharezLook/Button"); // todo: "TaharezLook" should depend on the current factory/layout
};

typedef button_base::info button;

} // namespace cegui
} // namespace gui
} // namespace yake

#endif // _YAPP_PLUGINS_GUI_CEGUI_WIDGETS_BUTTON_H_