/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#ifndef YAKE_MSG_ROUTER_H
#define YAKE_MSG_ROUTER_H

#include "yake/msg/prerequisites.h"

MSG_NAMESPACE_BEGIN

	template<typename T_processor>
	struct router : public T_processor
	{
		router();

		template<typename T>
		void postMessage(const T& v)
		{ this->post( MSG_NAMESPACE::makeMessage(v) ); }

		void post(message_base* msg)
		{
			this->onPost(msg);
		}


		router& operator << (listener_base*);

	private:
		// disallow:
		router(const router&);
		router& operator=(const router&);
		//...
	private:
		typedef detail::listener_list listener_list;
		typedef detail::listener_map listener_map;
	};
	template<typename T_processor>
	router<T_processor>::router()
	{}
	template<typename T_processor>
	router<T_processor>& router<T_processor>::operator << (listener_base* l)
	{
		assert(l);
		if (!l)
			return *this;
		this->add(l);
		return *this;
	}

MSG_NAMESPACE_END

#endif
