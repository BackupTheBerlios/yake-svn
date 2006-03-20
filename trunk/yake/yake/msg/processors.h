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
#ifndef YAKE_MSG_PROCESSORS_H
#define YAKE_MSG_PROCESSORS_H

#include "yake/msg/prerequisites.h"

MSG_NAMESPACE_BEGIN

	/** Processes messages immediately when they are possed. */
	struct ImmediateProcessor : public detail::msg_listener_mgr
	{
		typedef detail::listener_map listener_map;
		typedef detail::listener_list listener_list;

		void onPost(detail::message_base* msg)
		{
			YAKE_ASSERT(msg);
			YAKE_ASSERT( !isProcessing() );
			setProcessing(true);
			this->process(msg);
			setProcessing(false);
		}
	};
	/** Queues messages in a single queue and processes them when processMessages() is called.
		@Remarks It is not possible post messages while processing is in progress! See DoubleQueuedProcessor for a processor which can do that. processMessages() must not be called when processing is already in progress.
		@see ImmediateProcessor
		@see DoubleQueuedProcessor
	*/
	struct SingleQueuedProcessor : public detail::msg_listener_mgr
	{
		typedef detail::listener_map listener_map;
		typedef detail::listener_list listener_list;

		SingleQueuedProcessor()
		{}
		void onPost(detail::message_base* msg)
		{
			YAKE_ASSERT(!isProcessing());
			YAKE_ASSERT(msg);
			q_.push_back(msg); //@todo sort by msg type!?
		}
		void processMessages()
		{
#ifdef METHOD2
			while (!q_.empty())
			{
				YAKE_ASSERT( !isProcessing() );
				setProcessing(true);

				detail::message_base* msg = q_.front();
				q_.pop_front();
				YAKE_ASSERT( msg );
				this->process(msg);

				setProcessing(false);
			}
#else
			YAKE_ASSERT( !isProcessing() );
			setProcessing(true);
			for (msg_q::const_iterator it = q_.begin(); it != q_.end(); ++it)
			{
				detail::message_base* msg = *it;
				YAKE_ASSERT( msg );
				this->process(msg);
			}
			q_.clear();
			setProcessing(false);
#endif
		}
	private:
		typedef std::deque<detail::message_base*> msg_q;
		msg_q		q_;
		bool		isProcessing_;
	};
	/** Queues messages in a single queue and processes them when processMessages() is called.
		@Remarks It is possible post messages while processing is in progress! processMessages() must not be called when processing is already in progress.
		@see ImmediateProcessor
		@see SingleQueuedProcessor
	*/
	struct DoubleQueuedProcessor : public detail::msg_listener_mgr
	{
		typedef detail::listener_map listener_map;
		typedef detail::listener_list listener_list;

		DoubleQueuedProcessor() : q_(&q1_) {}
		void onPost(detail::message_base* msg)
		{
			YAKE_ASSERT(msg);
			q_->push_back(msg); //@todo sort by msg type!?
		}
		void processMessages()
		{
			YAKE_ASSERT( !isProcessing() );
			msg_q* procQ = q_;
			q_ = (q_ == &q1_) ? &q2_ : &q1_;
			while (!procQ->empty())
			{
				setProcessing(true);
				detail::message_base* msg = procQ->front();
				procQ->pop_front();
				YAKE_ASSERT( msg );
				this->process(msg);
				setProcessing(false); // this may trigger some listener disconnections!
			}
		}
	private:
		typedef std::deque<detail::message_base*> msg_q;
		msg_q		q1_,q2_;
		msg_q*		q_;
		bool		isProcessing_;
	};

MSG_NAMESPACE_END

#endif
