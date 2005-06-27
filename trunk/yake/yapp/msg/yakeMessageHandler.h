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
#ifndef YAKE_MSG_MESSAGEHANDLER_H
#define YAKE_MSG_MESSAGEHANDLER_H

#include <yapp/msg/yakeCommon.h>

namespace yake {
namespace msg {

	/** A functor which represents a message handler.
		@see MessageHandler
	*/
	typedef boost::function<ResultCode(const Message&)> MessageHandlerFn;

	/** A message handler object.
		@see MessageHandler
		@see MessageManager
	*/
	struct MessageHandler
	{
		friend class MessageManager;
		virtual ~MessageHandler() {}

		/** Override this in derived classes to handle messages. */
		virtual ResultCode onMessage(const Message& msg) = 0;
	private:
		void doMessage(const Message& msg)
		{
			onMessage( msg );
		}
	};

} // namespace msg
} // namespace yake

#endif
