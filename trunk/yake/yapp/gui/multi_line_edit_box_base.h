#ifndef _MULTI_LINE_EDIT_BOX_H_
#define _MULTI_LINE_EDIT_BOX_H_

#include <reflection.h>
#include <bind_lua/bind_lua.h>
#undef free // todo: find conflict with tnl and move to bind_network.h
#include <bind_network/bind_network.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "config.h"
#include "control_base.h"

#include "cegui_util.h"

namespace yake
{
namespace gui
{

class multi_line_edit_box_base : public control_base
{
public: // types
	static struct defaults
	{
		static struct size { static float height, width; };
		static struct position { static float x, y; };
		static const char * text;
	};

  struct info : info_base<info, multi_line_edit_box_base>
  {
		typedef info_base<info, multi_line_edit_box_base> base;
		info() {}
    info(const char * text) : m_text(text) {}
		info & text(const char * text) { m_text = text; return *this; }
		info & text(const std::string & text) { m_text = text; return *this; }
		std::string m_text;
  };

	struct cegui_widget_info
	{
		typedef CEGUI::MultiLineEditbox widget;

		static std::string url()
		{ 
			static unsigned int current_id = 0;
			return std::string("push_multi_line_edit_box" + boost::lexical_cast<std::string>(current_id++)); 
		}

		static const char * class_name()
		{ return "TaharezLook/MultiLineEditbox"; }
	};

public: // constructors
  multi_line_edit_box_base() 
		: m_impl(cegui::create_widget<cegui_widget_info>())
	{
		// defaults
		m_impl.setSize(CEGUI::Size(defaults::size::width, defaults::size::height));
		m_impl.setPosition(CEGUI::Point(defaults::position::x, defaults::position::y));
		m_impl.setText(defaults::text);
	}

  multi_line_edit_box_base(const info & this_info) 
		: m_impl(cegui::create_widget<cegui_widget_info>())
	{
		// if the user has set a pointer then bind user pointer to this
		if(this_info.m_bind_to_this) *const_cast<info &>(this_info).m_bind_to_this = this;

		// set properties
		m_impl.setText(this_info.m_text);
		m_impl.setSize(CEGUI::Size(this_info.m_w, this_info.m_h));
		m_impl.setPosition(CEGUI::Point(this_info.m_x, this_info.m_y));  
	}

public: // accessors
	const char * get_text() const
	{	return m_impl.getText().c_str(); }

	void set_text(const std::string & text)
	{	m_impl.setText(text); }

	virtual CEGUI::Window & getImpl() const                                              
	{ return m_impl; }

private: // data
	CEGUI::MultiLineEditbox & m_impl;

public:	// reflection
	GUI_CLASS(multi_line_edit_box_base, control_base);
};

typedef multi_line_edit_box_base::info multi_line_edit_box;

} // namespace gui
} // namespace yake

#endif // _MULTI_LINE_EDIT_BOX_H_