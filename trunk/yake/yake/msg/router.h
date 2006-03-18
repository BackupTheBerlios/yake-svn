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

	/** Generic customizable message router which represents a central point for listener management, message posting and message processing.
		
		Example:
		@code
			using namespace msg;

			router< ImmediateProcessor > myrouter;

			myrouter.post(makeMessage(1)); // int(1) is the message
			myrouter.postMessage(1); // the same - but using an alternative posting method

			myrouter.postMessage(1,Source(12)); // posting message int(1) from source 12
		@endcode

		For more elaborate examples see the source distribution.

		@see detail::msg_listener_mgr
		@todo merge documentation of listener_mgr into router's docs.
	*/
	template<typename T_processor>
	struct router : public T_processor
	{
	private:
		// disallow:
		router(const router&);
		router& operator=(const router&);
	public:
		/** @todo add docs */
		typedef T_processor processor_type;

		/** Construct a router. */
		router();

		template<typename T>
		void postMessage(const T& v, source_type source = kNullSource)
		{ this->post( MSG_NAMESPACE::makeMessage(v,source) ); }

		/** Post a message. Processing is controlled by T_processor.
			@see postMessage()
		*/
		void post(detail::message_base* msg)
		{
			this->onPost(msg);
		}

		/** Operator for listener registration (with source identifier 0). */
		router& operator << (detail::listener_base*);
		/** Operator for listener registration with user supplied source identifier. */
		router& operator << (const std::pair<detail::listener_base*,source_type>&);

	private:
		typedef detail::listener_list listener_list;
		typedef detail::listener_map listener_map;
	};
	template<typename T_processor>
	router<T_processor>::router()
	{}
	template<typename T_processor>
	router<T_processor>& router<T_processor>::operator << (detail::listener_base* l)
	{
		YAKE_ASSERT(l);
		if (!l)
			return *this;
		this->add(l);
		return *this;
	}
	template<typename T_processor>
	router<T_processor>& router<T_processor>::operator << (const std::pair<detail::listener_base*,source_type>& l)
	{
		YAKE_ASSERT(l.first);
		if (!l.first)
			return *this;
		this->add(l.first,l.second);
		return *this;
	}
MSG_NAMESPACE_END

#endif
