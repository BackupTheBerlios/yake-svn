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
*/
#ifndef YAKE_BASE_PREQUISITES_STRINGVC8_H
#define YAKE_BASE_PREQUISITES_STRINGVC8_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "../yakePrerequisites.h"
#endif

// todo

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

typedef std::string YAKE_BASE_STRING;

} // base
} // yake

namespace std
{
/*	template<> size_t hash_compare < yake::base::YAKE_BASE_STRING, std::less< yake::base::YAKE_BASE_STRING > >::operator ()( const yake::base::YAKE_BASE_STRING& rStringBase ) const
	{
		register size_t ret = 0;
		for( yake::base::YAKE_BASE_STRING::const_iterator it = rStringBase.begin(); it != rStringBase.end(); ++it )
			ret = 5 * ret + *it;

		return ret;
	}*/
} // std

namespace yake
{
namespace base
{

	

//#if _DEFINE_DEPRECATED_HASH_CLASSES
//		typedef std::hash< YAKE_BASE_STRING, std::less< YAKE_BASE_STRING > > _StringHash;
//#else
	typedef stdext::hash_compare< YAKE_BASE_STRING, std::less< YAKE_BASE_STRING > > _StringHash;
//#endif

//#undef YAKE_BASE_STRING

} // base
} // yake

#endif // YAKE_BASE_PREQUISITES_STRINGVC8_H
