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
#ifndef YAPP_MODELLINK_H
#define YAPP_MODELLINK_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeUpdaters.h>
#include <yake/data/yakeData.h>

namespace yake {
namespace app {
namespace model {

	class YAPP_BASE_API ModelLink : public IObjectController
	{
	public:
		YAKE_DECLARE_CLASS( ModelLink );
		YAKE_DECLARE_REGISTRY_0( ModelLink, ::yake::base::String );
	};

	template< class SourceType >
	class ModelLinkController : public ModelLink
	{
	private:
		SourceType	* mSource;
	public:
		virtual void setSource( SourceType * source )
		{ mSource = source; }
		SourceType* getSource() const
		{ return mSource; }
	};

}
}
}

#endif