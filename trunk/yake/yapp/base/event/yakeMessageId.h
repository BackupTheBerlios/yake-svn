/**------------------------------------------------------------------------------------
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
#ifndef YAKE_YAPP_BASE_EVENT_MESSAGEID_H
#define YAKE_YAPP_BASE_EVENT_MESSAGEID_H

namespace yake {
namespace event {

	typedef YAPP_BASE_API  yake::uint32 MessageId;
	/*
	class MessageId
	{
	public:
		String asString() const;
	};*/

	/** Simple class used for managing message ids. This can (and most likely will)
		be replaced with a proper manager that is also able to handle rematching
		ids when using serialization and persistence and similar concepts.
		@remarks This is a very temporary concept and definitely subject to change!
	*/
	class YAPP_BASE_API  MessageIdManager
	{
	public:
		/** Create a new unique message id. */
		static MessageId createId();
	private:
		static MessageId msLastId;
	};

}
}
#endif
