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
#ifndef YAKE_YAPP_BASE_EVENT_MESSAGEMANAGER_H
#define YAKE_YAPP_BASE_EVENT_MESSAGEMANAGER_H

#include <yake/base/yakeParamHolder.h>
#include <yapp/base/event/yakeMessageInstance.h>
#include <yapp/base/event/yakeMessageQ.h>

using namespace yake::base;
using namespace yake::base::templates;

namespace yake {
namespace event {

	class MessageListener;
	/**
		@Remarks
			QUEUES, INSTANCES, EXECUTION AND PARALELL POSTING OF INSTANCES:
			The MessageManager uses two queues to allow for posting message instance
			while executing a queue.
			The two queues are called "cache" and "exec".
			Message instances are always added to the cache Q while execution works
			on the exec Q.
			When you call execute (and it's not already executing) then the two queue
			objects are exchanged and what previously was the cache Q becomes the
			exec Q.
			The exec Q is automatically emptied after execution.
			Message instances can be posted while the queue is executing, but they
			will not be processed within the same execution process. You have to call
			execute() again to process them.
			TODO This may become configurable in the future.
			Do not call execute() while an execution is in progress! This will fail!
	*/
	class YAPP_BASE_API MessageManager
	{
	public:
		MessageManager();
		virtual ~MessageManager();

		/** Create a message object.
			It may be taken from a pool and is managed by the creating MessageManager object.
		*/
		MessageInstance* createMessage();
		/** Posts a message instance.
			@see createMessage()
			@param pMsg a valid pointer to a message instance, could be created using createMessage()
			@param executeImmediately if set to yes then the message will immediately be executed, otherwise
				it will be queued.
		*/
		void postMessage( MessageInstance* pMsg, bool executeImmediately = false );
		/** Posts a message instance.
			@see createMessage()
			@param id a valid message id
			@param pParams optional parameters of the messages.
			@param pOrigin optional origin. useful for listeners that listen to a message from a certain origin.
			@param executeImmediately if set to yes then the message will immediately be executed, otherwise
				it will be queued. 
		*/
		void postMessage( const MessageId id, ParamHolder* pParams, const void* pOrigin = 0, bool executeImmediately = false );
		/** Posts all messages in the specified queue. Order of messages is kept.
			The queue is immediately cleared!
			This method can be used to batch messages somewhere else and then batch post them
			to a central manager. Useful in multi-threaded applications.
			@param pQueue valid pointer to a MessageQueue object
		*/
		void postQueue( MessageQueue* pQueue );

		typedef boost::function<MsgResultCode(const MessageId, ParamHolder*)> MessageListenerFn;;

		/** Connect a listener to messages of certain type (id). If pOrigin is set then
			the listener listens only to message instances from that origin.
		*/
		void connect( MessageListener* pListener, const MessageId id, const void* pOrigin = 0 );
		void connect( const MessageListenerFn& fn, const MessageId id, const void* pOrigin = 0 );

		/** Disconnect a listener from messages (no matter which id/type).
			If pOrigin is 0, then the listener will be disconnected from all message types,
			otherwise it will only be disconnected from message types that share the specified
			origin.
		*/
		void disconnect( MessageListener* pListener, const void* pOrigin = 0 );
		void disconnect( const MessageListenerFn& fn, const void* pOrigin = 0 );

		/** Disconnect a listener from a specific message type.
			If pOrigin is 0, then the listener will be disconnected from all message types,
			otherwise it will only be disconnected from message types that share the specified
			origin.
		*/
		void disconnect( MessageListener* pListener, const MessageId id, const void* pOrigin = 0 );
		void disconnect( const MessageListenerFn& fn, const MessageId id, const void* pOrigin = 0 );

		/** Disconnected all listeners for a specific message id.
			If pOrigin is specified (non-0) then only listeners sharing that origin will be removed,
			otherwise all isteners for the specified message id will be removed.
		*/
		void disconnect( const MessageId id, const void* pOrigin = 0 );

		/** Execute queued message instances.
			The order of posted instances is kept.
			Processed instances are removed from the queue.
		*/
		void executeQ();

		/** Removes any message instances from the queue. */
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

		MsgInstanceList*	mCacheQ;
		MsgInstanceList*	mExecQ;
		bool				mExecuting;
	private:
		void executeMsgInstance( MessageInstance & msg );
		void executeMsgInstanceToListenerList( MessageInstance & msg, ListenerList & list );
	};

}
}

#endif
