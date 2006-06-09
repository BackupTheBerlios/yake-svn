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
#ifndef YAKE_BASE_PREREQUISITES_H
#define YAKE_BASE_PREREQUISITES_H

//============================================================================
//    IMPLEMENTATION HEADERS
//===========================================================================
#include <yake/base/yakePlatform.h>
#include <assert.h>
#include <ctime>
#include <math.h>
#include <cmath>
#if (YAKE_PLATFORM == PLATFORM_WIN32) && (YAKE_COMPILER == COMPILER_GNUC)
	#ifdef min
		   #undef min
	#endif
	#ifdef max
		   #undef max
	#endif
#endif
#include <limits>
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <stdexcept>
#include <locale>
#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <set>
#include <iostream>
#include <sstream>

#ifdef __SGI_STL
#	include <iterator>
#endif

// Include platform specific configuration files
#if (YAKE_PLATFORM == PLATFORM_WIN32) && (YAKE_COMPILER == COMPILER_MSVC)
#	include <yake/base/prerequisites/yakePrerequisitesVC.h>
#elif (YAKE_PLATFORM == PLATFORM_WIN32) && (YAKE_COMPILER == COMPILER_GNUC)
#	include <yake/base/prerequisites/yakePrerequisitesGCCWin.h>
#elif (YAKE_PLATFORM == PLATFORM_LINUX) && (YAKE_COMPILER == COMPILER_GNUC)
#	include <yake/base/prerequisites/yakePrerequisitesGCC.h>
#else
#	error("Yake: No configuration file set for the selected platform/compiler!")
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
// Yake
#if defined( YAKE_BASE_EXPORTS )
#	define YAKE_BASE_API DLLEXPORT
#else
#	define YAKE_BASE_API DLLIMPORT
#endif

#define _T(X) X

#if defined(_DEBUG) || defined(DEBUG)
#	define YAKE_DEBUG
#endif
//#define YAKE_UNICODE

// STL
#if defined(YAKE_DEBUG_BUILD)
//# define _STLP_DEBUG 1
#endif
#define _STLP_USE_DYNAMIC_LIB

// Boost / TTL
#if (YAKE_PLATFORM == PLATFORM_WIN32) && (YAKE_COMPILER == COMPILER_GNUC)
	#define YAKE_SIGNALS_USE_BOOST
#else
	#define YAKE_SIGNALS_USE_BOOST
	//#define YAKE_SIGNALS_USE_TTL
#endif

// Helper macros
// todo: remove, deprecated, use safe_delete(_array)
#define YAKE_SAFE_RELEASE(ptr)              if(ptr) { ptr->release(); ptr = 0; }
#define YAKE_SAFE_DELETE(ptr)               if(ptr) { delete ptr; ptr = 0; }
#define YAKE_SAFE_DELETE_ARRAY(ptr_array)   if(ptr_array) { delete [] ptr_array; ptr_array = 0; }

#define YAKE_DECLARE_CLASS(name)            static const char * yake_private_currentClass() { return #name; }
#define YAKE_DECLARE_GLOBAL                 YAKE_DECLARE_CLASS( global )
#define YAKE_DECLARE_FUNCTION(name)         static const char * yake_private_currentFunction = #name "()";

#endif // YAKE_BASE_PREREQUISITES_H
