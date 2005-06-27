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
#ifndef _YAKE_BASE_MPL_GET_TYPE_OR_NULL_H_
#define _YAKE_BASE_MPL_GET_TYPE_OR_NULL_H_

#include <boost/mpl/if.hpp>

#include "null_type.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================

namespace yake
{
namespace base
{
namespace mpl
{

// meta functions: returns given arg if there is one, otherwise it will return null
template <typename T = null_type>
struct get_type_or_null
{
  typedef T type;   
};

} // mpl
} // base
} // yake

#endif // _YAKE_BASE_MPL_GET_TYPE_OR_NULL_H_