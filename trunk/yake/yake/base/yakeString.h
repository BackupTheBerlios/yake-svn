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
#ifndef YAKE_BASE_STRING_H
#define YAKE_BASE_STRING_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

//typedef std::string YAKE_BASE_STRING;

	typedef YAKE_BASE_API YAKE_BASE_STRING String;
	typedef YAKE_BASE_API std::stringstream StrStream;

	template<typename T>
	String& operator << (String & lhs, const T & rhs)
	{
		StrStream ss;
		ss << rhs;
		lhs += ss.str();
		return lhs;
	}

	class YAKE_BASE_API StringUtil
	{
	public:
		static real parseReal( const String& rString )
		{
			return (real) atof( rString.c_str() );
		}
		static void trim(String& rString, bool left = true, bool right = true);
		static std::vector< String > split( const String& rString, const String& delims = "\t\n", unsigned int maxSplits = 0);
		static String toLowerCase( const String& rString );
		static String toUpperCase( const String& rString );
		static real toReal( const String& rString );
	};

#undef YAKE_BASE_STRING

} // base
} // yake

#endif // YAKE_BASE_STRING_H
