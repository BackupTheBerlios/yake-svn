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
#include <yapp/base/yappPCH.h>
#include <yake/base/yakeParamHolder.h>
#include <yapp/base/event/yakeMessageManager.h>
#include <yapp/base/event/yakeMessageInstance.h>
#include <yapp/base/event/yakeMessageListener.h>

namespace yake {
namespace event {

#define MSGMGR_COUT( x ) \
	std::cout << x;

#define MSGMGR_COUT_LINE( x ) \
	std::cout << x << std::endl;

	MessageManager::MessageManager() : mExecuting(false), mExecQ(0), mCacheQ(0)
	{
		mCacheQ = new MsgInstanceList();
		YAKE_ASSERT( mCacheQ );
		mExecQ = new MsgInstanceList();
		YAKE_ASSERT( mExecQ );
	}
	MessageManager::~MessageManager()
	{
		YAKE_SAFE_DELETE( mExecQ );
		YAKE_SAFE_DELETE( mCacheQ );
		mMsgIds.clear();
	}
	void MessageManager::postMessage(
							const MessageId id,
							ParamHolder* pParams, 
							const void* pOrigin /* = 0 */, 
							bool executeImmediately /* = false  */)
	{
		YAKE_ASSERT( mCacheQ );
		MSGMGR_COUT_LINE( "MsgMgr::postMessage() id=" << id );
		//FIXME: use an auto-adjusting pool!
		MessageInstance* pI = new MessageInstance(id, pParams, pOrigin, /*FIXME:*/false);
		if (executeImmediately)
		{
			MSGMGR_COUT_LINE( "  execute immediately" );
			executeMsgInstance( *pI );
			YAKE_SAFE_DELETE( pI );
		}
		else
			mCacheQ->push_back( SharedPtr<MessageInstance>(pI) );
	}
	void MessageManager::postQueue( MessageQueue* pQueue )
	{
		YAKE_ASSERT( mCacheQ );
		YAKE_ASSERT( pQueue ).debug("need a valid ptr!");
		if (!pQueue)
			return;
		MsgInstanceListConstIterator it = pQueue->getMsgInstanceIterator();
		while (it.hasMoreElements())
		{
			mCacheQ->push_back( it.getNext() );
		}
		pQueue->clear();
	}
	void MessageManager::executeQ()
	{
		MSGMGR_COUT_LINE( "MsgMgr::executeQ()" );
		YAKE_ASSERT( mCacheQ );
		YAKE_ASSERT( mExecQ );
		if (!mExecuting)
		{
			mExecuting = true;
			// switch queues
			std::swap( mCacheQ, mExecQ );
		}
		else
		{
			YAKE_ASSERT( !mExecuting ).debug("Q is already in the process of being executed!");
			if (!mExecuting)
				return;
		}
		VectorIterator<MsgInstanceList> it( mExecQ->begin(), mExecQ->end() );
		while (it.hasMoreElements())
			executeMsgInstance( *(it.getNext()).get() );

		mExecQ->clear();

		mExecuting = false;
	}
	void MessageManager::clearQ()
	{
		YAKE_ASSERT( mCacheQ );
		mCacheQ->clear(); 
	}
	void MessageManager::executeMsgInstance( MessageInstance & msg )
	{
		MSGMGR_COUT_LINE( "MsgMgr::executeMsgInstance() id=" << msg.id() );
		SharedPtr<MsgIdEntry>& e = mMsgIds[ msg.id() ];
		if (e.get() == 0)
		{
			MSGMGR_COUT_LINE( "  no listeners" );
			return;
		}
		if (!msg.origin())
		{
			MSGMGR_COUT_LINE( "  using 'no origin' list" );
			executeMsgInstanceToListenerList( msg, e->mListeners );
		}
		else
		{
			MSGMGR_COUT_LINE( "  finding listeners for origin (" << msg.origin() << ")" );
			OriginList::const_iterator itFind = e->mOrigins.find( msg.origin() );
			if (itFind != e->mOrigins.end() )
			{
				SharedPtr<ListenerList> & pList = e->mOrigins[ msg.origin() ];
				executeMsgInstanceToListenerList( msg, *pList.get() );
			}
			else
				MSGMGR_COUT_LINE( "  no listeners" );
		}
	}
	void MessageManager::executeMsgInstanceToListenerList( MessageInstance & msg, ListenerList & list )
	{
		MSGMGR_COUT_LINE( "  sending to listener list" );
		VectorIterator<ListenerList> it( list.begin(), list.end() );
		while (it.hasMoreElements())
		{
			MessageListener* pListener = it.getNext();
			YAKE_ASSERT( pListener );
			MsgResultCode retCode = pListener->onMessage( msg.id(), msg.params() );
			if (kMsgHandledExclusively == retCode)
				return;
		}
	}
	void MessageManager::connect( MessageListener* pListener, const MessageId id, const void* pOrigin )
	{
		YAKE_ASSERT( pListener );

		// get entry for message id, if it doesn't exist create one.
		SharedPtr<MsgIdEntry> & e = mMsgIds[ id ];
		if (e.get() == 0)
			e.reset( new MsgIdEntry() );

		// either add to default list (all messages no matter which origin)
		if (!pOrigin)
			e->mListeners.push_back( pListener );
		else
		{
			SharedPtr<ListenerList> & pListenerList = e->mOrigins[ const_cast<void*>(pOrigin) ];
			if (pListenerList.get() == 0)
				pListenerList.reset( new ListenerList() );
			pListenerList->push_back( pListener );
		}
	}
	void MessageManager::disconnect( MessageListener* pListener, const void* pOrigin )
	{
		MSGMGR_COUT_LINE( "MsgMgr::disconnect(listener,origin)" );
	}
	void MessageManager::disconnect( MessageId id, const void* pOrigin )
	{
		MSGMGR_COUT_LINE( "MsgMgr::disconnect(id,origin)" );
		// get entry for message id.
		MsgIdList::iterator itFind = mMsgIds.find( id );
		YAKE_ASSERT( itFind != mMsgIds.end() );
		if (itFind == mMsgIds.end())
		{
			MSGMGR_COUT_LINE( "  no entry for id found" );
			return;
		}
		SharedPtr<MsgIdEntry> & e = itFind->second;
		if (!pOrigin) // disconnect everything
		{
			MSGMGR_COUT_LINE( "  disconnecting all listeners for id" );
			e->mListeners.clear();
			e->mOrigins.clear();
		}
		else // disconnect only for specified origin
		{
			MSGMGR_COUT_LINE( "  looking for listeners for id and origin" );
			OriginList::iterator itFindOriginEntry = e->mOrigins.find( pOrigin );
			if (itFindOriginEntry != e->mOrigins.end())
			{
				MSGMGR_COUT_LINE( "  disconnecting listeners" );
				e->mOrigins.erase( itFindOriginEntry );
			}
			else
				MSGMGR_COUT_LINE( "  no listeners found" );
		}
	}

}
}
