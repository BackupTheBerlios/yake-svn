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

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/native/yakeNative.h>

#include <cassert>
#include <cstdio>
#include <locale>
#include <iostream>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace native
{

static class LogConsole
{
public:
	LogConsole()
	{
	}

	~LogConsole()
	{
	}

	void Print( const char* text )
	{
		std::cout << text;
	}

private:
    static int niftyCounter_;
    static bool consoleApp_;
} logConsole_g;

YAKE_BASE_NATIVE_API void debug_Log( const char* what, DebugLog::Severity eSeverity, const char* source )
{
	static char szBuffer[ 1024 ];

	sprintf( szBuffer, "%s: %s", source, what );

	logConsole_g.Print( szBuffer );
	logConsole_g.Print( "\n" );
}

YAKE_BASE_NATIVE_API void debug_Print( const char* string )
{
	logConsole_g.Print( string );
}

YAKE_BASE_NATIVE_API bool debug_AssertFailed( 	const char* pszMessage,
						const char* pszCondition,
						const char* pszFile,
						int nLine,
						bool& rbIgnoreAlways )
{
}

} // native
} // base
} // yake
