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
#ifndef YAKE_YAPP_BASE_EVENT_MESSAGEMANAGER_H
#define YAKE_YAPP_BASE_EVENT_MESSAGEMANAGER_H

#include "yakeMessageInstance.h"

namespace yapp {
namespace event {

	class MessageListener;
	class YAPP_BASE_API  MessageManager
	{
	public:
		MessageManager();
		virtual ~MessageManager();

		/** Create a message object.
			It may be taken from a pool and is managed by the creating MessageManager object.
		*/
		MessageInstance* createMessage();
		void postMessage( MessageInstance* pMsg );
		void postMessage( const MessageId id, ParamHolder* pParams, const void* pOrigin = 0, bool executeImmediately = false );

		void connect( MessageListener* pListener, const MessageId id, const void* pOrigin = 0 );
		void disconnect( MessageListener* pListener, const void* pOrigin = 0 );
		void disconnect( MessageListener* pListener, const MessageId id, const void* pOrigin = 0 );
		void disconnect( const MessageId id, const void* pOrigin = 0 );

		void executeQ();
		void clearQ();
	private:
		typedef Vector< MessageListener* > ListenerList;
		typedef AssocVector< const void*, SharedPtr<ListenerList> > OriginList;
		struct MsgIdEntry
		{
			OriginList		mOrigins;
			ListenerList	mListeners; /// listeners for any origin
		};
		typedef AssocVector< MessageId, SharedPtr<MsgIdEntry> > MsgIdList;
		MsgIdList			mMsgIds;

		typedef Vector< SharedPtr<MessageInstance> > MsgInstanceList;
		MsgInstanceList		mQ;
	private:
		void executeMsgInstance( MessageInstance & msg );
		void executeMsgInstanceToListenerList( MessageInstance & msg, ListenerList & list );
	};

}
}

#endif
