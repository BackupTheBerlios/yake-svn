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
// Stl
#include <cassert>
#include <locale>

extern "C"
{
#	include <dlfcn.h>
}

// Yake
#include <yake/base/yakePCH.h>
#include <yake/base/templates/yakeSmartAssert.h>
#include <yake/base/yakeException.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace native
{

YAKE_BASE_NATIVE_API LibraryHandle library_Load( const char* pFilename )
{
	YAKE_ASSERT( pFilename ).debug( "Invalid filename." );    

	// Due to Linux libraries naming we decided to add some "brand" prefix
	yake::base::String yakeLibPrefix( "libyake" );
	yake::base::String name( pFilename );
	
	// We're adding Linux port prefix to every library name
	name = yakeLibPrefix + name;        
	
	// dlopen() does not add .so to the filename, like windows does for .dll
	if ( name.substr( name.length() - 3, 3 ) != ".so" )
		name += ".so";

	LibraryHandle handle = ( LibraryHandle )dlopen( name.c_str(), RTLD_LAZY );
	
	if ( handle == 0 )
		throw Exception( "Failed to load " + name + String("due to ") + String( dlerror() ) , "native::Linux::library_Load" );

	return handle;
}

YAKE_BASE_NATIVE_API void* library_GetSymbol( LibraryHandle library, const char* pFunction )
{
	YAKE_ASSERT( library ).debug( "Invalid library handle." );
	YAKE_ASSERT( pFunction ).debug( "Invalid library function." );

	// Get the Procedure's Address.
	void* address = dlsym( const_cast< void* >( library ), pFunction );

	return address;
}
 
YAKE_BASE_NATIVE_API void library_Free( LibraryHandle library )
{
	YAKE_ASSERT( library ).debug( "Invalid library handle" );

	dlclose( const_cast< void* >( library ) );
}


} // native
} // base
} // yake
