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
#ifndef _YAPP_GUI_SYSTEM_H_
#define _YAPP_GUI_SYSTEM_H_

// stl
#include <typeinfo>
// boost
#include <boost/shared_ptr.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/lambda.hpp>
// yake
#include <yake/base/mpl/abstract_factory_pars.h>
#include <yake/base/templates/yakeRegistry.h>
// yapp
#include <yapp/gui/config.h>
#include <yapp/gui/widgets.h>

namespace yake
{
namespace gui 
{

/* create a list holding the constructor arguments for the abstract factory, currently
   only the info class is passed to the widget constructor, so each list holds one argument (the info class).
	 result:
		list
		<
			list<widget1::info &>,
			list<widget2::info &>,
			...
		>
*/
namespace // unnamed
{
	template <typename Widget>
	struct get_info_class_list
	{
		typedef boost::mpl::list<typename Widget::info &> type;
	};
} // namespace unnamed

template <typename Widgets>
struct get_widget_infos_lists 
	: boost::mpl::transform
		< 
			Widgets, 
			typename boost::mpl::lambda< get_info_class_list<boost::mpl::_1> >::type 
		>
{};

struct YAPP_GUI_API gui_system_base 
	: yake::base::mpl::abstract_factory
		<
			available_widgets, // products
			yake::base::mpl::abstract_factory_unit_pars, // creator
			typename get_widget_infos_lists<available_widgets>::type // constructor args
		>
{
	// virtual destructor
  virtual ~gui_system_base() {}

	// todo: meta: not needed, because typeid(ptr) returns final type of ptr
	// returns type info of the concrete type used by renderer adapters
	//virtual const std::type_info & get_type_info() = 0;

	// class registry for concrete backends
	YAKE_DECLARE_REGISTRY_0(gui_system_base, yake::base::String);
};

void set_gui_system(boost::shared_ptr<gui_system_base> system);
boost::shared_ptr<gui_system_base> get_gui_system();

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_SYSTEM_H_