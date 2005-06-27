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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/templates/yakeSingleton.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::templates::detail;

TrackerArray yake::base::templates::detail::pTrackerArray = 0;
unsigned int yake::base::templates::detail::elements = 0;

void AtExitFn()
{
		assert(yake::base::templates::detail::elements > 0 && yake::base::templates::detail::pTrackerArray != 0);
    // Pick the element at the top of the stack
    LifetimeTracker* pTop = yake::base::templates::detail::pTrackerArray[elements - 1];
    // Remove that object off the stack
    // Don't check errors - realloc with less memory 
    //     can't fail
    yake::base::templates::detail::pTrackerArray = static_cast<TrackerArray>(std::realloc(
        yake::base::templates::detail::pTrackerArray, 
					sizeof(*yake::base::templates::detail::pTrackerArray) * --elements));
    // Destroy the element
    delete pTop;
}
