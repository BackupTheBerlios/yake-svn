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
#include <limits>
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include <string>
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
//#define YAKE_SIGNALS_USE_BOOST
#define YAKE_SIGNALS_USE_TTL

// Helper macros
#define YAKE_SAFE_RELEASE( x )			if( x ) { x->release(); x = 0; }
#define YAKE_SAFE_DELETE( x )				if( x ) { delete x; x = 0; }
#define YAKE_SAFE_DELETE_ARRAY( x ) if( x ) { delete [] x; x = 0; }

#define YAKE_DECLARE_CLASS( name )           static const char* yake_private_currentClass() { return ## #name; }
#define YAKE_DECLARE_GLOBAL								   YAKE_DECLARE_CLASS( global )
#define YAKE_DECLARE_FUNCTION( name )        static const char* yake_private_currentFunction = ## #name ## "()";

#define YAKE_THIS_CLASS                      yake_private_currentClass()
#define YAKE_THIS_FUNCTION                   yake_private_currentFunction
#define YAKE_HERE                            YAKE_THIS_CLASS + yake::base::String( "::" ) + YAKE_THIS_FUNCTION
#define YAKE_NOCLASS_HERE                    "::" + yake::base::String( YAKE_THIS_FUNCTION )

#endif // YAKE_BASE_PREREQUISITES_H
