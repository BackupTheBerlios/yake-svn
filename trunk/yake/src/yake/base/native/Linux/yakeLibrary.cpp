/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
// Stl
#include <cassert>
#include <locale>

extern "C"
{
#include <dlfcn.h>
#include <assert.h>
}

// Yake
#include <yake/base/yakePCH.h>
#include <yake/base/native/yakeNative.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace native
{

YAKE_BASE_NATIVE_API LibraryHandle library_Load( const char* pFilename )
{
	assert( pFilename && "Invalid filename." );

	// Due to Linux libraries naming we decided to add some "brand" prefix
	String yakeLibPrefix( "libyake" );
	String yappLibPrefix( "libyapp" );
	
	String yakeName( yakeLibPrefix + String( pFilename ) );
	String yappName( yappLibPrefix + String( pFilename ) );
	
	// dlopen() does not add .so to the filename, like windows does for .dll
	if ( yakeName.substr( yakeName.length() - 3, 3 ) != ".so" )
		yakeName += ".so";
	
	// the same for other prefix
	if ( yappName.substr( yappName.length() - 3, 3 ) != ".so" )
		yappName += ".so";
	
	String name = yakeName;
	
#ifdef YAKE_DEBUG
	debug_Print( String( "Trying to load " + name ).c_str() );
#endif

	LibraryHandle handle = ( LibraryHandle )dlopen( name.c_str(), RTLD_LAZY );
	
	// trying to load library with libyapp prefix
	if ( handle == NULL )
	{
#ifdef YAKE_DEBUG
		debug_Print( String( "dlerror returned:" + String( dlerror() ) ).c_str() );
#endif
		
		name = yappName;
		
#ifdef YAKE_DEBUG
		debug_Print( String( "Now trying to load " + name ).c_str() );
#endif

		handle = ( LibraryHandle )dlopen( name.c_str(), RTLD_LAZY );
	}
	
	if ( handle == NULL )
		throw "Failed to load " + String( pFilename ) 
		    + String( " with both libyake and libyapp prefixes due to " ) 
		    + String( dlerror() );

#ifdef YAKE_DEBUG
	debug_Print( String( "successfully loaded " + name ).c_str() );
#endif
	
	return handle;
}

YAKE_BASE_NATIVE_API void* library_GetSymbol( LibraryHandle library, const char* pFunction )
{
	assert( library && "Invalid library handle." );
	assert( pFunction && "Invalid library function." );

	// Get the Procedure's Address.
	void* address = dlsym( const_cast< void* >( library ), pFunction );

	return address;
}
 
YAKE_BASE_NATIVE_API void library_Free( LibraryHandle library )
{
	assert( library && "Invalid library handle" );

	dlclose( const_cast< void* >( library ) );
}


} // native
} // yake

