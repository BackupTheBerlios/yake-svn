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
#ifndef YAKE_BASE_STRING_H
#define YAKE_BASE_STRING_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
#include <yake/base/templates/yakeVector.h>
#include <yake/base/templates/yakeFastMap.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

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

	template<typename T>
	String operator << ( const String& lhs, const T& rhs)
	{
		StrStream ss;
		ss << rhs;
		return lhs + ss.str();
	}

	typedef Vector<String> StringVector;

	class YAKE_BASE_API StringUtil
	{
	public:
		static real parseReal( const String& rString )
		{
			return (real) atof( rString.c_str() );
		}
		static int32 parseInt( const String& rString )
		{
			return int32( atoi( rString.c_str() ) );
		}
		static bool parseBool( const String& rString )
		{
			String in = toLowerCase( rString );
			return (in == "yes" || in == "1" || in == "true");
		}
		static void trim(String& rString, bool left = true, bool right = true);
		static std::vector< String > split( const String& rString, const String& delims = "\t\n", unsigned int maxSplits = 0);
		static void split( StringVector& ret, const String& rString, const String& delims = "\t\n", unsigned int maxSplits = 0);
		static String toLowerCase( const String& rString );
		static String toUpperCase( const String& rString );
		static real toReal( const String& rString );
		static size_t toSizeT( const String& rString );
		static int32 toInt32( const String& rString );
		static uint32 toUInt32( const String& rString );
		// Thanks to OGRE!
	    static bool StringUtil::startsWith(const String& str, const String& pattern, bool lowerCase);
		// Thanks to OGRE!
	    static bool StringUtil::endsWith(const String& str, const String& pattern, bool lowerCase);
	};

	struct YAKE_BASE_API MakeStringVector
	{
		MakeStringVector & operator<<(const char * str)
		{
			if (str)
				m_strings.push_back(str); 
			return *this; 
		}
		operator StringVector()
		{ 
			return m_strings; 
		}
	private:
		StringVector m_strings;
	};
	typedef std::pair<String,String> StringPair;
	typedef Vector<StringPair> StringPairVector;
	struct YAKE_BASE_API MakeStringPairVector
	{
		MakeStringPairVector & operator<<(const StringPair& stringPair)
		{
			m_stringPairs.push_back( stringPair ); 
			return *this; 
		}
		operator StringPairVector()
		{ 
			return m_stringPairs; 
		}
	private:
		StringPairVector m_stringPairs;
	};

	typedef AssocVector<String,String> StringMap;
	struct YAKE_BASE_API MakeStringMap
	{
		MakeStringMap & operator<<(const StringPair& stringPair)
		{
			m_map.insert( stringPair ); 
			return *this; 
		}
		operator StringMap()
		{ 
			return m_map; 
		}
	private:
		StringMap m_map;
	};
	inline std::ostream& operator << (std::ostream& out, const StringMap& rhs)
	{
		out << "StringMap(";
		for (StringMap::const_iterator it = rhs.begin(); it != rhs.end(); ++it)
		{
			out << ((it == rhs.begin()) ? "" : ";") 
				<< it->first << "=" << it->second;
		}
		out << ")";
		return out;
	}

#undef YAKE_BASE_STRING

} // yake

#endif // YAKE_BASE_STRING_H
