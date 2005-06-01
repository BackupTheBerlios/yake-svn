/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004, 2005 The YAKE Team
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
#ifndef YAKE_MSG_MESSAGEMANAGER_H
#define YAKE_MSG_MESSAGEMANAGER_H

#include <yapp/msg/yakeCommon.h>

namespace yake {
namespace msg {

	/** Manages creation, destruction, posting and dispatching of messages as well
		as message handler registration.
		@todo fix mem leaks
	*/
	class YAKE_MSG_API MessageManager
	{
		MessageManager(const MessageManager&);
		MessageManager& operator = (const MessageManager&);
	public:
		MessageManager();
		virtual ~MessageManager();
		
		typedef boost::function<bool(const Message&,const std::ostream&)> MessageSerNetFn;
		void regMessageSerializerForNetworking(const MessageId& id, const MessageSerNetFn& fn);

		/** @defgroup Message handler registration */
		//@{
		/** Add a message handler.
			@param id Message id to register this handler for.
						If id is MSGID_Any then the message handler is registered for all messages.
						If id is invalid (or even MSGID_Invalid) then nothing is done.
			@param source If non-0 this specifies a specific message source. If source is 0 then all sources are used.
		*/
		void addMessageHandler(const MessageId& id, const MessageHandlerFn& fn, const void* source = 0);
		/** @copydoc addMessageHandler(const MessageId& id, const MessageHandlerFn& fn, const void* source = 0)
		*/
		void addMessageHandler(const MessageId& id, const MessageHandler* pHandler, const void* source = 0);

		/** Add a message handler for all messages of a specific source (if source is non-0) or of all sources.
		*/
		void addMessageHandler(const void* source, const MessageHandlerFn& fn);

		/** @copydoc addMessageHandler(const void* source, const MessageHandlerFn& fn)
		*/
		void addMessageHandler(const void* source, const MessageHandler* pHandler);
		//@}

		/** Creates a message instance for the given message id and message source.
			This message can then either be posted or it can be destroyed using destroyMessage.
			If a message is neither posted nor explicitely destroyed when the manager is destroyed,
			then the manager will take care of destruction.
			@see postMessage
			@see destroyMessage
		*/
		Message* createMessage(const MessageId& id, const void* source = 0);

		/** Destroys the given manager.
			@Remarks The message must have been created using this manager!
		*/
		void destroyMessage(Message* pMessage);

		/** Posts a message instance.
			@Remarks Messages are not instantly processed. They are queued until execute is called.
		*/
		void postMessage(Message* pMessage);

		/** Processes all mqueued (read: posted) messages.
			@Remarks It is safe to post messages while the manager is executing them.
					These messages will be added to a cache which will be executed when
					execute() is called the next time.
		*/
		void execute();
	private:
		MessageQ			mQ[2];
		MessageQ*			mMainQ;
		MessageQ*			mCacheQ;
		bool				mIsExecuting;

		typedef std::deque<Message*> MessageList;
		MessageList			mCreatedMessages;

		typedef std::deque<MessageHandlerFn> HandlerFnList;
		typedef std::map<const void*,HandlerFnList> SourceHandlerFnList;
		struct MessageIdEntry
		{
			HandlerFnList			handlerFns;
			SourceHandlerFnList		sourceHandlerFns;
		};
		typedef std::map<MessageId,MessageIdEntry> IdEntryMap;
		IdEntryMap			mIdEntries;
		SourceHandlerFnList	mSourceHandlerFns;
	private:
		void prepareExecute();
		void finishExecute();
		void fireHandlersForMessage(const MessageId& id, const Message& msg);
		void fireHandlerFnList(const HandlerFnList& handlerFns, const Message& msg);
	};

} // namespace msg
} // namespace yake

#endif
