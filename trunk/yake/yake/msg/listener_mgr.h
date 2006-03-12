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
#ifndef YAKE_MSG_LISTENER_MGR_H
#define YAKE_MSG_LISTENER_MGR_H

#include "yake/msg/prerequisites.h"

MSG_NAMESPACE_BEGIN

	namespace detail {
		typedef std::deque<boost::shared_ptr<listener_base> > listener_list;
		typedef std::map<std::string,listener_list> listener_map;

		struct msg_listener_mgr
		{
			bool add(listener_base* l)
			{
				assert(l);
				if (!l)
					return false;
				listeners_[l->msgTypeName()].push_back( boost::shared_ptr<listener_base>(l) );
				return true;
			}
			void process(message_base* msg)
			{
				assert( msg );
				listener_map::const_iterator itMsgType = listeners_.find(msg->msgTypeName());
				if (itMsgType == listeners_.end())
					return;
				const listener_list& l = itMsgType->second;
				for (listener_list::const_iterator it = l.begin(); it != l.end(); ++it)
					(*it)->execute(*msg);
				MSG_NAMESPACE::destroyMessage(msg);
			}
		private:
			typedef MSG_NAMESPACE::detail::listener_list listener_list;
			typedef MSG_NAMESPACE::detail::listener_map listener_map;
			listener_map	listeners_;
		};
	}

MSG_NAMESPACE_END

#endif
