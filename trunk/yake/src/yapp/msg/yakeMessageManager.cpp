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
#include <yapp/msg/yakePCH.h>
#include <yapp/msg/yakeMsg.h>

namespace yake {
namespace msg {

	void MessageManager::addMessageHandler(const MessageId& id, const MessageHandlerFn& fn, const void* source)
	{
		if (mIdEntries.find( id ) == mIdEntries.end())
			mIdEntries[ id ] = MessageIdEntry();
		MessageIdEntry& msgIdEntry = mIdEntries[ id ];
		if (source)
		{
			if (msgIdEntry.sourceHandlerFns.find(source) == msgIdEntry.sourceHandlerFns.end())
				msgIdEntry.sourceHandlerFns[ source ] = HandlerFnList();
			msgIdEntry.sourceHandlerFns[ source ].push_back( fn );
		}
		else
		{
			msgIdEntry.handlerFns.push_back( fn );
		}
		YAKE_ASSERT( mIdEntries.find( id ) != mIdEntries.end() );
	}
	void MessageManager::addMessageHandler(const void* source, const MessageHandlerFn& fn)
	{
		mSourceHandlerFns[ source ].push_back( fn );
	}
	void MessageManager::fireHandlersForMessage(const MessageId& id, const Message& msg)
	{
		// We cache the handlers. While this has a performance impact it allows
		// any handlers to detach/attach themselves as necessary. Otherwise this
		// could destroy our iterators etc.
		HandlerFnList	cachedHandlerFns;

		// try "all messages with source"
		if (msg.getSource())
		{
			SourceHandlerFnList::const_iterator it = mSourceHandlerFns.find( msg.getSource() );
			if (it != mSourceHandlerFns.end())
			{
				const HandlerFnList& handlerFns = (*it).second;
				if (!handlerFns.empty())
					std::copy( handlerFns.begin(), handlerFns.end(), std::back_inserter( cachedHandlerFns) );
			}
		}
		// try "specific message id..."
		if (mIdEntries.find( id ) == mIdEntries.end())
			return;
		const MessageIdEntry& msgIdEntry = mIdEntries[ id ];

		// ..."with source"
		if (msg.getSource())
		{
			SourceHandlerFnList::const_iterator it = msgIdEntry.sourceHandlerFns.find( msg.getSource() );
			if (it != msgIdEntry.sourceHandlerFns.end())
			{
				const HandlerFnList& handlerFns = (*it).second;
				if (!handlerFns.empty())
					std::copy( handlerFns.begin(), handlerFns.end(), std::back_inserter( cachedHandlerFns) );
			}
		}
		
		// ..."without source"
		if (!msgIdEntry.handlerFns.empty())
			std::copy( msgIdEntry.handlerFns.begin(), msgIdEntry.handlerFns.end(), std::back_inserter( cachedHandlerFns) );
		
		fireHandlerFnList( cachedHandlerFns, msg );
	}
	void MessageManager::fireHandlerFnList(const HandlerFnList& handlerFns, const Message& msg) 
	{
		const HandlerFnList::const_iterator itEnd = handlerFns.end();
		for (HandlerFnList::const_iterator itFn = handlerFns.begin(); itFn != itEnd; ++itFn)
		{
			ResultCode code = (*itFn)(msg);

			// Actually the code isn't intended to be used here.
			// That's because all primary handlers should've a chance of handling this messaging.
			//if (code == kHandled)
			//	break;
		}
	}
	MessageManager::MessageManager() :
		mIsExecuting(false)
	{
		// valid if not executing:
		mMainQ = &mQ[0];
		mCacheQ = &mQ[1];
	}
	MessageManager::~MessageManager()
	{
		while (!mMainQ->empty())
			destroyMessage( mMainQ->pop() );

		while (!mCacheQ->empty())
			destroyMessage( mCacheQ->pop() );

		DequeIterator<MessageList> itMsg(mCreatedMessages);
		while (itMsg.hasMoreElements())
		{
			destroyMessage( itMsg.getNext() );
		}
		mCreatedMessages.clear();
	}
	Message* MessageManager::createMessage(const MessageId& id, const void* source /*= 0*/)
	{
		Message* pMsg = new Message(id,source);
		mCreatedMessages.push_back( pMsg );
		return pMsg;
	}
	void MessageManager::destroyMessage(Message* pMessage)
	{
		MessageList::iterator itFind = std::find( mCreatedMessages.begin(), mCreatedMessages.end(), pMessage );
		if (itFind != mCreatedMessages.end())
		{
			mCreatedMessages.erase( itFind );
		}
		if (pMessage->canBeDeleted())
			delete pMessage;
	}
	void MessageManager::postMessage(Message* pMessage)
	{
		YAKE_ASSERT( mCacheQ );
		YAKE_ASSERT( pMessage ).warning("valid message pointer expected");
		if (!pMessage)
			return;
		mCacheQ->push( pMessage );
	}
	void MessageManager::execute()
	{
		YAKE_ASSERT( !mIsExecuting ).warning("Already executing!");
		if (mIsExecuting)
			return;
		prepareExecute();
		while (!mMainQ->empty())
		{
			Message* pMessage = mMainQ->pop();
			YAKE_ASSERT( pMessage ).warning("invalid message pointer - skipped");
			if (pMessage)
			{
				std::cout << "EXECUTING MSG: " << pMessage->id() << "\n";
				fireHandlersForMessage( pMessage->id(), *pMessage );
				fireHandlersForMessage( MSGID_Any, *pMessage );
				destroyMessage( pMessage );
			}
		}
		finishExecute();
	}
	void MessageManager::prepareExecute()
	{
		mIsExecuting = true;
		std::swap( mMainQ, mCacheQ );
	}
	void MessageManager::finishExecute()
	{
		mIsExecuting = false;
	}

} // namespace msg
} // namespace yake
