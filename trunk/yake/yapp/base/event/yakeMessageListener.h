/**------------------------------------------------------------------------------------
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
#ifndef YAKE_YAPP_BASE_EVENT_MESSAGELISTENER_H
#define YAKE_YAPP_BASE_EVENT_MESSAGELISTENER_H

namespace yapp {
namespace event {

	/** Base class for message listeners.
		A listener has to override the onMessage() method to do something worthwhile.
		A listener can connect to messages in a variety of ways through MessageManager objects.
		@see MessageManager
		@see MessageId
		@see MessageInstance
	*/
	class YAPP_BASE_API MessageListener
	{
	public:
		/** React on messages.
			Return kMsgHandled if message has been handled and other listeners may use this message, too.
			Return kMsgHandledExclusively if other listeners shouldn't receive this message.
			Return kMsgUnhandled if message wasn't handled by this object.
			Return any of the other kMsgXXXX constants as needed (error, corrupt message, ...)
		*/
		virtual MsgResultCode onMessage( const MessageId id, ParamHolder* pParams );
	};

	inline MsgResultCode MessageListener::onMessage( const MessageId id, ParamHolder* pParams )
	{
		return kMsgUnhandled;
	}

}
}
#endif
