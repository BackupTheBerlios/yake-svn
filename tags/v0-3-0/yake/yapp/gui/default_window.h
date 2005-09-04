/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef _YAPP_GUI_DEFAULT_WINDOW_H_
#define _YAPP_GUI_DEFAULT_WINDOW_H_

#include "window_base.h"
#include <yapp/plugins/gui_cegui/window.h>

// todo: gui system sits within yapp::gui
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

#endif // _YAPP_GUI_DEFAULT_WINDOW_H_