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
		struct msg_listener_mgr;
	} // namespace detail

	/** Represents the connection between a listener  and the router and can be disconnected via a member method.
		@see ScopedListenerConnection
		@todo Handle disconnect()'s when having more than one copy of the connection object.
		@Remarks STILL VERY MUCH EXPERIMENTAL! NOT SUPPORTED BY ALL ROUTERS! CAN EXPLODE WHEN USED WRONGLY!
	*/
	struct ListenerConnection
	{
	private:
		ListenerConnection();
	public:
		ListenerConnection(detail::msg_listener_mgr* mgr,
							detail::listener_base* listener,
							source_type source,
							target_type target) :
			mgr_(mgr),
			listener_(listener),
			source_(source),
			target_(target)
		{
		}
		ListenerConnection(const ListenerConnection& other) :
			mgr_(other.mgr_),
			listener_(other.listener_),
			source_(other.source_),
			target_(other.target_)
		{
		}
		ListenerConnection& operator=(const ListenerConnection& rhs)
		{
			if (&rhs == this)
				return *this;
			mgr_ = rhs.mgr_;
			listener_ = rhs.listener_;
			source_ = rhs.source_;
			target_ = rhs.target_;
			return *this;
		}
		/** Disconnects the listener from the message router.
		*/
		void disconnect();

		bool empty() const
		{ 
			return (listener_ != 0); 
		}
		detail::listener_base* listener() const
		{
			return listener_;
		}
		source_type source() const
		{ 
			return source_;
		}
		target_type target() const
		{
			return target_;
		}
	private:
		detail::msg_listener_mgr*	mgr_;
		detail::listener_base*		listener_;
		source_type					source_;
		target_type					target_;
	};

	namespace detail {
		typedef std::deque<boost::shared_ptr<detail::listener_base> > listener_list;
		typedef std::map<std::string,listener_list> listener_map;

		/** Internal base class providing listener management and
			a hook for derived routers/processors.
		*/
		struct msg_listener_mgr
		{
			msg_listener_mgr() : isProcessing_(false)
			{}
			void remove(const ListenerConnection& conn)
			{
				if (isProcessing())
					connsToRemove_.push_back(conn);
				_doRemove(conn);
			}
			ListenerConnection add(detail::listener_base* l, source_type source = kNoSource, target_type tgt = kNoTarget)
			{
				YAKE_ASSERT(l);
				if (!l)
					return ListenerConnection(this,0,source,tgt);
				listener_map& listeners = sources_[source][tgt];
				listeners[l->msgTypeName()].push_back( boost::shared_ptr<detail::listener_base>(l) );
				return ListenerConnection(this,l,source,tgt);
			}
			void process(detail::message_base* msg)
			{
				YAKE_ASSERT( msg );

				const source_type source = msg->source();
				const target_type target = msg->target();
				YAKE_ASSERT( source != kAnySource ); // should be kNoSource if there's no source.
				YAKE_ASSERT( target != kAnyTarget ); // should be kBroadcastTarget.

				// find 'kAnySource' source
				source_map::const_iterator itSource = sources_.find(kAnySource);
				if (itSource != sources_.end())
				{
					// find listeners for specified target
					target_map::const_iterator itTarget = itSource->second.find(msg->target());
					if (itTarget != itSource->second.end())
						_processListeners( msg, itTarget->second );

					// find listeners for special 'kAnyTarget'
					itTarget = itSource->second.find(kAnyTarget);
					if (itTarget != itSource->second.end())
						_processListeners( msg, itTarget->second );
				}

				// find source
				itSource = sources_.find(source);
				if (itSource == sources_.end())
					return;

				const target_map& targets = itSource->second;

				// process listeners that have subscribed to all messages of this source
				target_map::const_iterator itAnyTargetListeners = targets.find(kAnyTarget);
				if (itAnyTargetListeners != targets.end())
					_processListeners( msg, itAnyTargetListeners->second );

				// broadcast or targetted message?
				if (target == kBroadcastTarget)
				{
					// broadcasting - iterate over all targets:
					target_map::const_iterator itEnd = targets.end();
					for (target_map::const_iterator itTarget = targets.begin(); itTarget != itEnd; ++itTarget)
					{
						if (itTarget->first == kAnyTarget) // We already have processed them above.
							continue;
						_processListeners( msg, itTarget->second );
					}
				}
				else
				{
					// targetted message - find target:
					target_map::const_iterator itTarget = targets.find(msg->target());
					if (itTarget == targets.end())
						return;
					// find message type & process message if found.
					_processListeners( msg, itTarget->second );
				}
				// clean up
				MSG_NAMESPACE::destroyMessage(msg);
			}
			void clear()
			{
				sources_.clear();
			}
		private:
			void _doRemoveList()
			{
				ConstVectorIterator< conn_list > it( connsToRemove_ );
				while (it.hasMoreElements())
					_doRemove( it.getNext() );
				connsToRemove_.clear();
			}
			void _doRemove(const ListenerConnection& conn)
			{
				_doRemove(conn.listener(),conn.source(),conn.target());
			}
			void _doRemove(detail::listener_base* l, source_type source = kNoSource, target_type tgt = kNoTarget)
			{
				source_map::iterator itSource = sources_.find( source );
				if (itSource == sources_.end())
					return; //@todo except?
				target_map& targets = itSource->second;
				target_map::iterator itTarget = targets.find( tgt );
				if (itTarget == targets.end())
					return;
				listener_map& listeners = itTarget->second;
				listener_map::iterator itL = listeners.find( l->msgTypeName() );
				if (itL != listeners.end())
					listeners.erase( itL );
			}
			void _processListeners(detail::message_base* msg, const listener_map& listeners)
			{
				// find message type
				listener_map::const_iterator itMsgType = listeners.find(msg->msgTypeName());
				if (itMsgType == listeners.end())
					return;
				// find listeners
				const listener_list& l = itMsgType->second;
				for (listener_list::const_iterator it = l.begin(); it != l.end(); ++it)
					(*it)->execute(*msg);
			}
		protected:
			void setProcessing(const bool yes)
			{
				isProcessing_ = yes;
				if (!isProcessing_)
					_doRemoveList();
			}
			bool isProcessing() const
			{
				return isProcessing_;
			}
		private:
			typedef MSG_NAMESPACE::detail::listener_list listener_list;
			typedef MSG_NAMESPACE::detail::listener_map listener_map;
			typedef AssocVector<target_type,listener_map> target_map;
			typedef AssocVector<source_type,target_map> source_map;
			source_map		sources_;
			//listener_map	listeners_;
			bool			isProcessing_;

			typedef std::deque<ListenerConnection> conn_list;
			conn_list		connsToRemove_;
		};
	}

	inline void ListenerConnection::disconnect()
	{
		if (mgr_ && listener_)
			mgr_->remove( *this );
		listener_ = 0;
	}

	/** Wraps a ListenerConnection in RAII fashion.
		@see ListenerConnection
		@Remarks STILL VERY MUCH EXPERIMENTAL! NOT SUPPORTED BY ALL ROUTERS! CAN EXPLODE WHEN USED WRONGLY!
	*/
	struct ScopedListenerConnection
	{
	private:
		ScopedListenerConnection();
		ScopedListenerConnection(const ScopedListenerConnection&);
		ScopedListenerConnection& operator=(const ScopedListenerConnection&);
	public:
		/** Constructs a scoped listener connection object from a listener connection.
		*/
		ScopedListenerConnection(const ListenerConnection& conn) : conn_(conn)
		{}

		/** Destroys the scoped listener and disconnects the listener connection
			from the messaging router.
		*/
		~ScopedListenerConnection()
		{
			conn_.disconnect();
		}
	private:
		ListenerConnection	conn_;
	};

MSG_NAMESPACE_END

#endif
