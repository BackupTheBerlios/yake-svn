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
#include <yake/base/yakePCH.h>
#include <yake/base/yakeMimeTypeManager.h>
#include <yake/base/yakeException.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

const String MimeTypeManager::UNKNOWN = "unknown";

MimeTypeManager::MimeTypeManager()
{
}

void MimeTypeManager::add( const String& rMimeType, const String& rExtension )
{
  YAKE_DECLARE_FUNCTION( add )

  if( mMapping.find( rExtension ) != mMapping.end() )
    throw yake::base::Exception( "Extension \"" + rExtension + "\" already registered.", YAKE_HERE );

  mMapping[ rExtension ] = rMimeType;
}


const String& MimeTypeManager::getMimeTypeForExtension( const String& rExtension ) const
{
  MappingType::const_iterator iter = mMapping.find( rExtension );

  if( iter == mMapping.end() )
    return UNKNOWN;

  return ( *iter ).second;
}
    
const String& MimeTypeManager::getMimeTypeForFileName( const String& rFilename ) const
{
  String::size_type pos = rFilename.rfind( '.' );
  
  if( pos == String::npos )
    return UNKNOWN;

  String extension = rFilename.substr( pos + 1 );

  return getMimeTypeForExtension( extension );
}

} // base
} // yake