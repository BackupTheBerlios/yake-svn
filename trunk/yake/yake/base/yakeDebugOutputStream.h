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
#ifndef YAKE_BASE_DEBUGOUTPUTSTREAM_H
#define YAKE_BASE_DEBUGOUTPUTSTREAM_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeOutputStream.h>
#include <yake/base/templates/yakeSingleton.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API DebugOutputStream : public IOutputStream
{
YAKE_DECLARE_CLASS( yake::base::DebugOutputStream )
// Class
public:
  DebugOutputStream();

// Methods
public:
  int seek( int offset, SeekBase base );
  int tell() const;
  bool isEos() const;

  IOutputStream& operator<<( bool b );
  IOutputStream& operator<<( signed char ch );
  IOutputStream& operator<<( unsigned char ch );
  IOutputStream& operator<<( signed short rNumber );
  IOutputStream& operator<<( unsigned short rNumber );
  IOutputStream& operator<<( signed int rNumber );
  IOutputStream& operator<<( unsigned int rNumber );
  IOutputStream& operator<<( signed long rNumber );
  IOutputStream& operator<<( unsigned long rNumber );
  IOutputStream& operator<<( float rNumber );
  IOutputStream& operator<<( double rNumber );
  IOutputStream& operator<<( const char* pString );

  int write( const bool* pBuffer, int count ); 
  int write( const signed char* pBuffer, int count ); 
  int write( const unsigned char* pBuffer, int count ); 
  int write( const signed short* pBuffer, int count ); 
  int write( const unsigned short* pBuffer, int count ); 
  int write( const signed int* pBuffer, int count ); 
  int write( const unsigned int* pBuffer, int count ); 
  int write( const signed long* pBuffer, int count ); 
  int write( const unsigned long* pBuffer, int count ); 
  int write( const float* pBuffer, int count ); 
  int write( const double* pBuffer, int count ); 
  
YAKE_BUILD_SINGLETON( DebugOutputStream )
};

} // base
} // yake

#endif // YAKE_BASE_DEBUGOUTPUTSTREAM_H
