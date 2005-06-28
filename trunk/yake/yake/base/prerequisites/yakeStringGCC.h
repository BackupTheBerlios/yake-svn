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
#ifndef YAKE_BASE_PLATTFORM_STRINGGCC_H
#define YAKE_BASE_PLATTFORM_STRINGGCC_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yakePrerequisites.h>
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{

typedef std::string YAKE_BASE_STRING;

} // yake

#ifndef GNU_STD_EXTENSION_HASH_STRING
#define GNU_STD_EXTENSION_HASH_STRING
namespace __gnu_cxx
{

template <> struct hash< yake::YAKE_BASE_STRING >
{
size_t operator()( const yake::YAKE_BASE_STRING _stringBase ) const 
{ 
	/* This is the PRO-STL way, but it seems to cause problems with VC7.1
	* and in some other cases (although I can't recreate it)
	* hash<const char*> H;
	* return H(_stringBase.c_str());
	*/
	/** This is our custom way */
	register size_t ret = 0;
	for( yake::YAKE_BASE_STRING::const_iterator it = _stringBase.begin(); it != _stringBase.end(); ++it )
	ret = 5 * ret + *it;
	return ret;
}
};

} // __gnu_cxx
#endif //GNU_STD_EXTENSION_HASH_STRING

typedef ::__gnu_cxx::hash < yake::YAKE_BASE_STRING > _StringHash;


#endif
