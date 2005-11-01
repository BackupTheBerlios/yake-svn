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
#ifndef _YAPP_GUI_CONTROL_BASE_H_
#define _YAPP_GUI_CONTROL_BASE_H_

#include <CEGUI.h>
#include <yake/reflection/reflection.h>

namespace yake
{
namespace gui
{

// todo: generic ... not bound to cegui
class control_base
{
public: // types
  template<typename ConcreteInfo, typename ConcreteControl> 
	struct info_base 
	{
		info_base() 
			: m_x(ConcreteControl::defaults::position::x),
				m_y(ConcreteControl::defaults::position::y),
				m_w(ConcreteControl::defaults::size::width),
				m_h(ConcreteControl::defaults::size::height),
				m_bind_to_this(NULL),
				m_abs_pos(false),
				m_visible(true)
		{}

		ConcreteInfo & pos(float x, float y) 
		{ m_x=x; m_y=y; return static_cast<ConcreteInfo&>(*this); }

		ConcreteInfo & absolute_position(float x, float y) 
		{ m_x=x; m_y=y; m_abs_pos=true; return static_cast<ConcreteInfo&>(*this); }

		ConcreteInfo & size(float width, float height) 
		{ m_w=width; m_h=height; return static_cast<ConcreteInfo&>(*this); }

		ConcreteInfo & visible(bool visible) 
		{ m_visible=visible; return static_cast<ConcreteInfo&>(*this); }
	
		ConcreteInfo & bind(ConcreteControl *& bind_to_this) 
		{ m_bind_to_this = &bind_to_this; return static_cast<ConcreteInfo&>(*this); }

		float m_x, m_y, m_w, m_h;
		bool m_abs_pos, m_visible;
		ConcreteControl ** m_bind_to_this;
	};

public: // construction and destruction
  virtual ~control_base() {}

public: // implementation-specific accessors  
  virtual CEGUI::Window & getImpl() const = 0; // todo: remove

	CLASS(control_base, reflection::NullClass);
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONTROL_BASE_H_