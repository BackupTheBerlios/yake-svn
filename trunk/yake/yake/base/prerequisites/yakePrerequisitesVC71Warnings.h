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
#ifndef YAKE_BASE_PREREQUISITES_PREREQUISITESVC71WARNINGS_H
#define YAKE_BASE_PREREQUISITES_PREREQUISITESVC71WARNINGS_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Preprocessor check
#if (YAKE_PLATFORM != PLATFORM_WIN32) || ( YAKE_COMP_VER < 1310 )
#	error "yake.base.prerequisites.vc71: This file should only be included when building using Microsoft Visual C++ 7.1 on the Win32 Platform."
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
// Disable several useless warnings.

// 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
// A base class or structure must be declared with the __declspec(dllexport) keyword for a function in a derived class to be exported
#pragma warning( disable : 4251 )

// Turn off 'warning C4355: 'this' : used in base member initializer list'
#pragma warning( disable : 4355 )

// Members of the <hash_map> and <hash_set> header files are not currently part 
// of the ISO C++ standard. Therefore, these types and members have been moved from 
// the std namespace to namespace stdext, to remain conformant with the C++ standard, 
// in Visual C++ .NET 2003.
#pragma warning( disable : 4996 )

#endif // YAKE_BASE_PREREQUISITES_PREREQUISITESVC71WARNINGS_H
