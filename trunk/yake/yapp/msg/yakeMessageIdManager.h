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
#ifndef YAKE_MSG_MESSAGEIDMANAGER_H
#define YAKE_MSG_MESSAGEIDMANAGER_H

#include <yapp/msg/yakeCommon.h>

namespace yake {
namespace msg {

	/** Manages creation and conversion of message identifiers, both strings and numeric.
		It also defines special IDs representing "invalid" or "any" message ids.
		@todo Make non-singleton.
	*/
	struct YAKE_MSG_API MessageIdManager
	{
	private:
		MessageIdManager();
		MessageIdManager(const MessageIdManager&);
	public:
		/** Registers a new message for the given name. The new id is returned.
		*/
		static MessageId newId(const String& name);

		/** Returns the message id that represents invalid message ids.
			This id is often used when message ids have to be returned.
		*/
		static const MessageId& idInvalid();

		/** Returns the message id that represents any other message id.
			It is used, for example, for message handlers that want to subscribe to
			all registered messages.
		*/
		static const MessageId& idAny();

		String toString(const MessageId& id) const;
		MessageId fromString(const String& msgName) const;
	private:
		typedef std::map<String,uint32> StringIdMap;
		typedef std::map<uint32,String> IdStringMap;
	};

} // namespace msg
} // namespace yake

#endif
