/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef YAKE_MSG_MESSAGEQ_H
#define YAKE_MSG_MESSAGEQ_H

#include <yapp/msg/yakeCommon.h>

namespace yake {
namespace msg {

	/** A FIFO (first in, first out) queue of messages providing a simple and easy to use interface.
		\Remarks A MessageQ does *not* own the contained Messages. It only references them!
			The owner has to take of proper destruction!
	*/
	class YAKE_MSG_API MessageQ
	{
		YAKE_DECLARE_CLASS( MessageQ );

		MessageQ(const MessageQ&);
		MessageQ& operator = (const MessageQ&);
	public:
		MessageQ();
		~MessageQ();

		/** Pushes a message to the back of the queue. */
		void push( Message* pMessage );

		/** Retrieves the front most message from the queue.
			The messages is removed from the queue.
		*/
		Message* pop();

		/** Returns whether the queue contains any messages.
		*/
		bool empty() const;
	private:
		typedef std::queue< Message* > MessageQueue;
		MessageQueue	mQ;
	};


} // namespace msg
} // namespace yake

#endif
