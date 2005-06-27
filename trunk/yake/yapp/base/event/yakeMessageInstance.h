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
#ifndef YAKE_YAPP_BASE_EVENT_MESSAGEINSTANCE_H
#define YAKE_YAPP_BASE_EVENT_MESSAGEINSTANCE_H

#include <yapp/base/event/yakeMessageId.h>
#include <yake/base/yakeParamHolder.h>

using namespace yake::base;

namespace yake {
namespace event {

	enum MsgResultCode {
		kMsgUnhandled,
		kMsgHandled,
		kMsgHandledExclusively,
		kMsgCorrupt,
		kMsgError
	};

	/** A message instance. Objects of this type are used within message managers (-> MessageManager)
		but can be used by applications and other clients, too.
		It holds everything needed for a single instance of a message including the id, (optionally)
		the parameter list (key/value pairs), (optionally) the origin of the message.
		It can be selected whether the ParamHolder object is owned by the message instance. If this is
		the case then the message instance is responsible for destruction.
		Specifying parameters and origin is optional (set to 0 if not used).
		@see MessageManager
	*/
	class YAPP_BASE_API MessageInstance
	{
	public:
		MessageInstance( const MessageId id, ParamHolder* pParams = 0, const void* pOrigin = 0, bool bOwnParamObject = false );
		~MessageInstance();

		/** Return the message's id. */
		MessageId id() const;

		/** Return the parameter holder object containing key/value pairs for all parameters.
			The return value may be 0 if no parameters are assigned to this message.
		*/
		ParamHolder* params() const;

		/** Return the origin of this message.
			The return value may be 0 if no origin was assigned to the message.
		*/
		const void* origin() const;

		void reset();
	private:
		MessageId		mId;
		ParamHolder*	mParams;
		bool			mOwnsParams;
		void*			mOrigin;
	};

}
}

#endif
