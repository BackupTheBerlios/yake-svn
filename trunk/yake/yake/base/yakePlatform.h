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
#ifndef YAKE_BASE_PLATFORM_H
#define YAKE_BASE_PLATFORM_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Platform/compiler-related stuff to set
#define PLATFORM_WIN32 1
#define PLATFORM_LINUX 2
#define PLATFORM_APPLE 3

#define COMPILER_MSVC 1
#define COMPILER_GNUC 2
#define COMPILER_BORL 3

#define ENDIAN_LITTLE 1
#define ENDIAN_BIG 2

// Get compiler and version
#if defined( _MSC_VER )
#   define YAKE_COMPILER COMPILER_MSVC
	// version:
	// 1300 for vc7 (2002)
	// 1301 for vc7.1 (2003)
#   define YAKE_COMP_VER _MSC_VER

#elif defined( __GNUC__ )
#   define YAKE_COMPILER COMPILER_GNUC
	// version:
	// 2097 for gcc 2.97
	// 3002 for gcc 3.2.x
	// 3003 for gcc 3.3.x
#   define YAKE_COMP_VER (__GNUC__*1000+__GNUC_MINOR___)

#elif defined( __BORLANDC__ )
#   define YAKE_COMPILER COMPILER_BORL
#   define YAKE_COMP_VER __BCPLUSPLUS__

#else
#   pragma error "Yake: Unknown compiler. Bailing out!"
#endif

// Platform
#if defined( __WIN32__ ) || defined( _WIN32 )
#   define YAKE_PLATFORM PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define YAKE_PLATFORM PLATFORM_APPLE
#else
#   define YAKE_PLATFORM PLATFORM_LINUX
//#else
//#	error("Yake: Unknown platform. Bailing out!")
#endif

#endif // YAKE_BASE_PLATFORM_H
