#ifndef _STATIC_TEXT_BASE_H_
#define _STATIC_TEXT_BASE_H_

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

class static_text_base : public control_base
{
public: // types
	static struct defaults
	{
		static struct size { static float height, width; };
		static struct position { static float x, y; };
		static const char * text;
		static const bool bg_enabled = true;
		static const bool frame_enabled = true;
	};

  // construction
  struct info : info_base<info, static_text_base>
  {
		typedef info_base<info, static_text_base> base;
		info() 
			: m_text(static_text_base::defaults::text),
				m_bg(static_text_base::defaults::bg_enabled),
				m_frame(static_text_base::defaults::frame_enabled)
		{}
    info(const char * text) 
			: m_text(text),
				m_bg(static_text_base::defaults::bg_enabled),
				m_frame(static_text_base::defaults::frame_enabled)
		{}
		info & bg_enabled(bool enable)	{ m_bg = enable; return *this; }
		info & frame_enabled(bool enable) { m_frame = enable; return *this; }
		info & text(const char * text) { m_text = text; return *this; }
		info & text(const std::string & text) { m_text = text; return *this; }
		std::string m_text;
		bool m_bg, m_frame;
  };

	struct cegui_widget_info
	{
		typedef CEGUI::StaticText widget;

		static std::string url()
		{ 
			static unsigned int current_id = 0;
			return std::string("push_static_text" + current_id++); 
		}

		static const char * class_name()
		{ return "TaharezLook/StaticText"; }
	};

public: // constructors
	static_text_base() 
		: m_impl(cegui::create_widget<cegui_widget_info>())
	{
		// defaults
		m_impl.setSize(CEGUI::Size(defaults::size::width, defaults::size::height));
		m_impl.setPosition(CEGUI::Point(defaults::position::x, defaults::position::y));
		m_impl.setText(defaults::text);
	}

  static_text_base(const info & this_info) 
		: m_impl(cegui::create_widget<cegui_widget_info>())
	{
		m_impl.setText(this_info.m_text);
		m_impl.setSize(CEGUI::Size(this_info.m_w, this_info.m_h));
		m_impl.setPosition(CEGUI::Point(this_info.m_x, this_info.m_y));  
		m_impl.setFrameEnabled(this_info.m_frame);
		m_impl.setBackgroundEnabled(this_info.m_bg);
	}

public: // accessors
	virtual CEGUI::Window & getImpl() const                                               
	{ return m_impl; }

private: // data
	CEGUI::StaticText & m_impl;

public:	// reflection
	GUI_CLASS(static_text_base, control_base);
};

typedef static_text_base::info static_text;

} // namespace gui
} // namespace yake

#endif // _STATIC_TEXT_BASE_H_