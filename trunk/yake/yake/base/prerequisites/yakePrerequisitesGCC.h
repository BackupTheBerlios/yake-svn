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
#ifndef YAKE_BASE_PREREQUISITES_PREREQUISITESGCC_H
#define YAKE_BASE_PREREQUISITES_PREREQUISITESGCC_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// GCC specific headers
#include <wchar.h>

// Include platform specific configuration files
#if (YAKE_PLATFORM == PLATFORM_LINUX) && (YAKE_COMPILER == COMPILER_GNUC)
#	pragma message("Yake platform/compiler config: yake.base.prerequisites.GCC")
//#	include "yakePrerequisitesGCCWarnings.h"
//#	include <wchar.h>
#	include <ext/hash_map>
#ifdef __GNUC__
#    if __GNUC__ < 3
#       include <ext/stl_hash_fun.h>
#    elif __GNUC_MINOR__ < 4
#        include <ext/stl_hash_fun.h>
#    else
#        include <ext/hash_fun.h>
#    endif
#endif

#include "yakeStringGCC.h"
//#	include "yakePrerequisitesGCCTypes.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================

// Fixed-size numeric types.
// Note: This naming convetion is equivalent to the C99 one. Once C99 will be supported by the compiler vendors,
// we can just remove the following typedefs and add an #include <cstdint>.
// SK: put it into namespace because it collided with other libs:

namespace yake 
{
typedef signed char           int8;
typedef unsigned char         uint8;
typedef signed short          int16;
typedef unsigned short        uint16;
typedef signed int          int32;
typedef unsigned int        uint32;
typedef long long		int64;
typedef unsigned long long	uint64;
typedef float			real;
}

// Stl workarounds
// #ifdef _STLP_HASH_MAP
// #	define HashMap ::std::hash_map
// #else
// #	define HashMap ::stdext::hash_map
// #endif
 
#else
#	error("Yake: No configuration file set for the selected platform/compiler!")
#endif

// String specialization
#if !defined ( _STLP_HASH_FUN_H )
#	include "yakeStringGCC.h"
#else
#	include "yakeStringHashFun.h"
#endif



//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
#define YAKE_VSNPRINTF vsnprintf

#define DLLEXPORT			 
#define DLLIMPORT			 
#define LIBRARY_EXTENSION	_( "so" )

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
//#ifdef GCC_3_1
#  define HashMap ::__gnu_cxx::hash_map
//#endif

#endif // YAKE_BASE_PREREQUISITES_PREREQUISITESGCC_H
