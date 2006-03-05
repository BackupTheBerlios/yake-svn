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
// stl
#include <cassert>
#include <locale>
// yake
#include <yake/base/yakePCH.h>
#include <yake/base/native/win32/yakeConfig.h>
#include <yake/base/templates/yakeSmartAssert.h>
#include <yake/base/yakeException.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace native
{

YAKE_BASE_NATIVE_API LibraryHandle library_Load(const char * filename)
{
	YAKE_ASSERT(filename).debug("Invalid filename.");
	// load library
	LibraryHandle handle = (LibraryHandle)::LoadLibraryA(filename);

	// todo: http://yake.org/forum/viewtopic.php?t=340
	if (!handle) 
		YAKE_EXCEPT("Couldn't load library " + String(filename) + ".");
	return handle;
}

YAKE_BASE_NATIVE_API void * library_GetSymbol(LibraryHandle library, const char * function)
{
	YAKE_ASSERT(library).debug("Invalid library handle.");
	YAKE_ASSERT(function).debug("Invalid library function.");
	// get the procedure's address
	void * address = ::GetProcAddress((HMODULE) library, function);
	return address;
}

YAKE_BASE_NATIVE_API void library_Free(LibraryHandle library)
{
	YAKE_ASSERT(library).debug("Invalid library handle.");
	// free library
	::FreeLibrary((HMODULE) library);
}


} // native
} // yake
