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
#ifndef YAKE_BASE_LIBRARY_H
#define YAKE_BASE_LIBRARY_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <inc/base/native/yakeNative.h>
#include <inc/base/yakeString.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API Library
{
YAKE_DECLARE_CLASS( yake::base::Library )
// Class
public:
  Library( const String& rFilename );
  virtual ~Library();

// Methods
public:
  void* getSymbol( const String& rFunction );

// Data
private:
	native::LibraryHandle mHandle;
};

} // base
} // yake

#define YAKE_USE_LIB( dllname ) \
	yake::base::templates::Pointer< yake::base::Library > pDynLib( new yake::base::Library( dllname ) ); \
	YAKE_ASSERT( pDynLib ).debug( "Out of memory." );

#endif // YAKE_BASE_LIBRARY_H
