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
#include <yapp/base/yappPCH.h>
#include <yapp/base/event/yakeMessageInstance.h>

namespace yake {
namespace event {

	MessageInstance::MessageInstance(const MessageId id, ParamHolder* pParams, const void* pOrigin, bool bOwnParamObject) :
						mId(id),
						mParams(pParams),
						mOwnsParams(bOwnParamObject),
						mOrigin(const_cast<void*>(pOrigin))
	{
	}
	MessageInstance::~MessageInstance()
	{
		reset();
	}
	const void* MessageInstance::origin() const
	{
		return mOrigin;
	}
	MessageId MessageInstance::id() const
	{
		return mId;
	}
	ParamHolder* MessageInstance::params() const
	{
		return mParams;
	}
	void MessageInstance::reset()
	{
		if (mOwnsParams)
			YAKE_SAFE_DELETE( mParams );
		mId = 0;
		mOrigin = 0;
	}

}
}
