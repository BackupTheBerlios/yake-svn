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
#ifndef YAKE_BASE_PLATTFORM_STRINGGCC_H
#define YAKE_BASE_PLATTFORM_STRINGGCC_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

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

namespace __gnu_cxx
{

	template <> struct hash< Yake::Core::YAKE_BASE_STRING >
{
size_t operator()( const Yake::Core::YAKE_BASE_STRING _stringBase ) const 
{ 
	/* This is the PRO-STL way, but it seems to cause problems with VC7.1
	* and in some other cases (although I can't recreate it)
	* hash<const char*> H;
	* return H(_stringBase.c_str());
	*/
	/** This is our custom way */
	register size_t ret = 0;
	for( Yake::Core::YAKE_BASE_STRING::const_iterator it = _stringBase.begin(); it != _stringBase.end(); ++it )
	ret = 5 * ret + *it;
	return ret;
}
};

} // __gnu_cxx


namespace std
{
	template<> size_t hash_compare < yake::YAKE_BASE_STRING, std::less< yake::YAKE_BASE_STRING > >::operator ()( const yake::YAKE_BASE_STRING& _stringBase ) const
	{
		register size_t ret = 0;
		for( yake::YAKE_BASE_STRING::const_iterator it = _stringBase.begin(); it != _stringBase.end(); ++it )
			ret = 5 * ret + *it;

		return ret;
	}
}

typedef ::__gnu_cxx::hash< YAKE_BASE_STRING > _StringHash;

}

#undef YAKE_BASE_STRING

#endif
