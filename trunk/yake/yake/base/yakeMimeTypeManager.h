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
#ifndef YAKE_BASE_MIMETYPEMANAGER_H
#define YAKE_BASE_MIMETYPEMANAGER_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/Templates/yakeSingleton.h>
#include <yake/base/yakeString.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API MimeTypeManager
{
YAKE_DECLARE_CLASS( yake::base::MimeTypeManager )
// Types
private:
  typedef std::map< String, String > MappingType;

// Class
public:
  MimeTypeManager();

// Methods
public:
  void add( const String& rMimeType, const String& rExtension );

  const String& getMimeTypeForExtension( const String& rExtension ) const;
  const String& getMimeTypeForFileName( const String& rFilename ) const;
      
// Data
public:
  static const String UNKNOWN;

private:
	MappingType mMapping;

YAKE_BUILD_SINGLETON( MimeTypeManager )
};

} // base
} // yake

#endif // YAKE_BASE_MIMETYPEMANAGER_H
