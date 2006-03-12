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

	struct ImmediateProcessor : public detail::msg_listener_mgr
	{
		typedef detail::listener_map listener_map;
		typedef detail::listener_list listener_list;

		void onPost(message_base* msg)
		{
			assert(msg);
			this->process(msg);
		}
	};
	struct SingleQueuedProcessor : public detail::msg_listener_mgr
	{
		typedef detail::listener_map listener_map;
		typedef detail::listener_list listener_list;

		SingleQueuedProcessor() : isProcessing_(false)
		{}
		void onPost(message_base* msg)
		{
			assert(!isProcessing_);
			assert(msg);
			q_.push_back(msg); //@todo sort by msg type!?
		}
		void processMessages()
		{
			assert( !isProcessing_ );
			isProcessing_ = true;
			for (msg_q::const_iterator it = q_.begin(); it != q_.end(); ++it)
			{
				message_base* msg = *it;
				assert( msg );
				this->process(msg);
			}
			q_.clear();
			isProcessing_ = false;
		}
	private:
		typedef std::deque<message_base*> msg_q;
		msg_q		q_;
		bool		isProcessing_;
	};
	struct DoubleQueuedProcessor : public detail::msg_listener_mgr
	{
		typedef detail::listener_map listener_map;
		typedef detail::listener_list listener_list;

		DoubleQueuedProcessor() : q_(&q1_), isProcessing_(false) {}
		void onPost(message_base* msg)
		{
			assert(msg);
			q_->push_back(msg); //@todo sort by msg type!?
		}
		void processMessages()
		{
			assert( !isProcessing_ );
			isProcessing_ = true;
			msg_q* procQ = q_;
			q_ = (q_ == &q1_) ? &q2_ : &q1_;
			for (msg_q::const_iterator it = procQ->begin(); it != procQ->end(); ++it)
			{
				message_base* msg = *it;
				assert( msg );
				this->process(msg);
			}
			procQ->clear();
			isProcessing_ = false;
		}
	private:
		typedef std::deque<message_base*> msg_q;
		msg_q		q1_,q2_;
		msg_q*	q_;
		bool		isProcessing_;
	};

MSG_NAMESPACE_END

#endif
