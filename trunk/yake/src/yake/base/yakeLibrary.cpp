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
#include <yake/base/yakePCH.h>
#include <yake/base/yakeLibrary.h>
#include <yake/base/yakeException.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

Library::Library( const String& rFilename )
{
	YAKE_DECLARE_FUNCTION( Library )

	mHandle = native::library_Load( rFilename.c_str() );
}

Library::~Library()
{
  if( 0 != mHandle )
    native::library_Free( mHandle );
}


void* Library::getSymbol( const String& rFunction )
{
  YAKE_DECLARE_FUNCTION( Symbol )
  void* pAddress = native::library_GetSymbol( mHandle, rFunction.c_str() );

  if( 0 == pAddress )
      YAKE_EXCEPT( "Couldn't get address for function \"" + rFunction + "\"" );

  return pAddress;
}

} // base
} // yake
