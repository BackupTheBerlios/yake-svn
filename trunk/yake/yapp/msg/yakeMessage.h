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
#ifndef YAKE_MSG_MESSAGE_H
#define YAKE_MSG_MESSAGE_H

#include <yapp/msg/yakeCommon.h>

namespace yake {
namespace msg {

	/** Represents a message instance.
	*/
	class YAKE_MSG_API Message
	{
		friend class MessageManager;
	protected:
		bool			mCanBeDeleted;
		MessageId		mId;
		const void*		mSource;
	private:
		Message(const Message&);
		Message& operator = (const Message&);
	public:
		Message(const MessageId& id, const void* source = 0, const bool canBeDeleted = true) : mId(id), mSource(source), mCanBeDeleted(canBeDeleted)
		{}
		Message(const bool canBeDeleted = true) : mId(MSGID_Invalid), mSource(0), mCanBeDeleted(canBeDeleted)
		{}
		virtual ~Message() {}
		bool canBeDeleted() const
		{
			return mCanBeDeleted;
		}
		const void* getSource() const
		{ return mSource; }
		void setSource(const void* source)
		{ mSource = source; }
		MessageId id() const
		{ return mId; }
	};

} // namespace msg
} // namespace yake

#endif
