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
#ifndef YAKE_BASE_NATIVE_NATIVE_H
#define YAKE_BASE_NATIVE_NATIVE_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakeProcessor.h>

#if defined( YAKE_BASE_EXPORTS )
#	define YAKE_BASE_NATIVE_API DLLEXPORT
#else
#	define YAKE_BASE_NATIVE_API DLLIMPORT
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace native
{

// Library Interface.
typedef const void * LibraryHandle;
YAKE_BASE_NATIVE_API LibraryHandle library_Load(const char * filemame);
YAKE_BASE_NATIVE_API void * library_GetSymbol(LibraryHandle, const char * function);
YAKE_BASE_NATIVE_API void library_Free(LibraryHandle);

// Debugging Interface.
struct DebugLog
{
	enum Severity
	{
		INFORMATIONS,
		WARNINGS,
		ERRORS
	};
};
YAKE_BASE_NATIVE_API bool debug_AssertFailed( const char* pszMessage, const char* pszCondition, const char* pszFile, int nLine, bool& rbIgnoreAlways );
YAKE_BASE_NATIVE_API void debug_Log( const char* what, DebugLog::Severity, const char* source );
YAKE_BASE_NATIVE_API void debug_Print( const char* string );

// Critical Section Interface.
typedef const void* CriticalSectionHandle;
YAKE_BASE_NATIVE_API CriticalSectionHandle criticalSection_Create();
YAKE_BASE_NATIVE_API void criticalSection_Enter( CriticalSectionHandle );
YAKE_BASE_NATIVE_API void criticalSection_Leave( CriticalSectionHandle );
YAKE_BASE_NATIVE_API void criticalSection_Delete( CriticalSectionHandle );

// Processor Information Interface.
YAKE_BASE_NATIVE_API void processor_RetrieveInfo( std::wstring& name, float& frequency, Processor::Info& );

// Timing Interface.
YAKE_BASE_NATIVE_API float getTime();

// Application Information Interface.
YAKE_BASE_NATIVE_API const char* getApplicationDir();

} // native
} // yake

#endif // YAKE_BASE_NATIVE_NATIVE_H
