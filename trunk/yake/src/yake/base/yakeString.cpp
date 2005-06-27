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
#include <yake/base/yakeString.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

	//-----------------------------------------------------
	// The following implementation is based on Ogre::String.

	//-----------------------------------------------------
	void StringUtil::trim(String& rString, bool left, bool right)
	{
		size_t lspaces, rspaces, len = rString.length(), i;

		lspaces = rspaces = 0;

		if( left )
		{
			// Find spaces / tabs on the left
			for( i = 0;
				i < len && ( rString[i] == ' ' || rString[i] == '\t' || rString[i] == '\r');
				++lspaces, ++i );
		}

		if( right && lspaces < len )
		{
			// Find spaces / tabs on the right
			for( i = len - 1;
				i >= 0 && ( rString[i] == ' ' || rString[i] == '\t' || rString[i] == '\r');
				rspaces++, i-- );
		}

		rString = rString.substr(lspaces, len-lspaces-rspaces);
	}

	//-----------------------------------------------------------------------
	std::vector<String> StringUtil::split(const String& rString, const String& delims, unsigned int maxSplits)
	{
		// static unsigned dl;
		std::vector<String> ret;
		unsigned int numSplits = 0;

		// Use STL methods 
		size_t start, pos;
		start = 0;
		do 
		{
			pos = rString.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				start = pos + 1;
			}
			else if (pos == rString.npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back( rString.substr(start) );
			}
			else
			{
				// Copy up to delimiter
				ret.push_back( rString.substr(start, pos - start) );
				start = pos + 1;
			}
			// parse up to next real data
			start = rString.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != rString.npos);

		return ret;
	}

	//-----------------------------------------------------------------------
	String StringUtil::toLowerCase(const String& rString) 
	{
		String out( rString );
		std::transform(
			out.begin(),
			out.end(),
			out.begin(),
			static_cast<int(*)(int)>(::tolower) );

		return out;
	}

	//-----------------------------------------------------------------------
	String StringUtil::toUpperCase(const String& rString) 
	{
		String out( rString );
		std::transform(
			out.begin(),
			out.end(),
			out.begin(),
			static_cast<int(*)(int)>(::toupper) );

		return out;
	}

	//-----------------------------------------------------------------------
	real StringUtil::toReal(const String& rString)
	{
		return static_cast<real>(atof( rString.c_str() ));
	}

	//-----------------------------------------------------------------------
	size_t StringUtil::toSizeT( const String& rString )
	{
		return static_cast<size_t>(atoi( rString.c_str() ));
	}

	//-----------------------------------------------------------------------
	int32 StringUtil::toInt32( const String& rString )
	{
		return static_cast<int32>(atoi( rString.c_str() ));
	}

} // yake
