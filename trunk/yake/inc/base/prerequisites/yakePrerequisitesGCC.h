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
#ifndef YAKE_BASE_PREREQUISITES_PREREQUISITESVC_H
#define YAKE_BASE_PREREQUISITES_PREREQUISITESVC_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// VC specific headers
#include <wchar.h>
#include "yakeStringVC.h"

// Include platform specific configuration files
#if (YAKE_PLATFORM == PLATFORM_WIN32) && (YAKE_COMPILER == COMPILER_MSVC)
#	if (YAKE_COMP_VER == 1200)
#		include "yakePrerequisitesVC6.h"
#	elif (YAKE_COMP_VER == 1300)
#		include "yakePrerequisitesVC7.h"
#	elif (YAKE_COMP_VER >= 1300)
#		include "yakePrerequisitesVC71.h"
#	else
#		error("Yake: No configuration file for this compiler!")
#	endif
#else
#	error("Yake: No configuration file set for the selected platform/compiler!")
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
#define YAKE_VSNPRINTF _vsnprintf

#define DLLEXPORT			__declspec( dllexport )
#define DLLIMPORT			__declspec( dllimport )
#define LIBRARY_EXTENSION	_( "dll" )

#ifdef _DEBUG
#	define YAKE_DEBUG_BUILD
#endif

#ifdef YAKE_DEBUG_BUILD
#	define DEBUG_BREAK		__asm { int 3 }
#else
#	define DEBUG_BREAK      exit( 666 );
#endif

namespace yake
{
namespace base
{

// Aligns the specified pointer to machine boundaries (32-bit, on Win32).
#define ALIGN( number )         ( number & ( ~0x3 ) ) | ( ( ( number & 0x3 ) != 0 ) << 2 )

} // base
} // yake

// Stl workarounds
#ifdef GCC_3_1
#  define HashMap ::__gnu_cxx::hash_map
#endif

#endif // YAKE_BASE_PREREQUISITES_PREREQUISITESVC_H
