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
#ifndef _YAPP_GUI_POINT_H_
#define _YAPP_GUI_POINT_H_

#include <boost/function.hpp>

namespace yake
{
namespace gui
{

class button_base;

struct point 
{
	typedef boost::function<void (const point &)> on_change_functor;

	point() {}
	point(float this_x, float this_y) : x(this_x), y(this_y) {}

protected:
	friend button_base;
	point(float this_x, float this_y, on_change_functor functor)
		: x(this_x), y(this_y), on_change(functor) {}

public:
	void set_x(float this_x) { x = this_x; on_change(*this); }
	float get_x() const { return x; }

	void set_y(float this_y) { y = this_y; on_change(*this); }
	float get_y() const { return y; }

	float x, y;

	on_change_functor on_change;
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_POINT_H_