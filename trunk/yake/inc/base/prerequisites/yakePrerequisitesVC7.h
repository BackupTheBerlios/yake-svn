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
#ifndef YAKE_BASE_PREREQUISITES_PREREQUISITESVC7_H
#define YAKE_BASE_PREREQUISITES_PREREQUISITESVC7_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Preprocessor check
#if (YAKE_PLATFORM != PLATFORM_WIN32) || ( YAKE_COMP_VER < 1300 )
#	error "Yake.Core.Prerequisites.Vc7: This file should only be included when building using Microsoft Visual C++ 7.0 on the Win32 Platform."
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
// Disable several useless warnings.
#pragma warning( disable : 4514 )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4284 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4512 )
#pragma warning( disable : 4150 )
#pragma warning( disable : 4201 )

// Fixed-size numeric types.
// Note: This naming convetion is equivalent to the C99 one. Once C99 will be supported by the compiler vendors,
// we can just remove the following typedefs and add an #include <cstdint>.
typedef signed __int8           int8;
typedef unsigned __int8         uint8;
typedef signed __int16          int16;
typedef unsigned __int16        uint16;
typedef signed __int32          int32;
typedef unsigned __int32        uint32;
typedef signed __int64          int64;
typedef unsigned __int64        uint64;
typedef float										real;

// Stl workarounds
#ifdef _STLP_HASH_MAP
#	define HashMap ::std::hash_map
#else
#	define HashMap ::stdext::hash_map
#endif

#endif // YAKE_BASE_PREREQUISITES_PREREQUISITESVC7_H
