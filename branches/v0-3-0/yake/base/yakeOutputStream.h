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
#ifndef YAKE_BASE_OUTPUTSTREAM_H
#define YAKE_BASE_OUTPUTSTREAM_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeString.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API IOutputStream
{
public:
  virtual ~IOutputStream();

  enum SeekBase
  { BEGIN, CURRENT, END };

  virtual int seek( int offset, SeekBase base ) = 0;
  virtual int tell() const = 0;
  virtual bool isEos() const = 0;

  virtual IOutputStream& operator<<( bool b ) = 0;
  virtual IOutputStream& operator<<( signed char ch ) = 0;
  virtual IOutputStream& operator<<( unsigned char ch ) = 0;
  virtual IOutputStream& operator<<( signed short rNumber ) = 0;
  virtual IOutputStream& operator<<( unsigned short rNumber ) = 0;
  virtual IOutputStream& operator<<( signed int rNumber ) = 0;
  virtual IOutputStream& operator<<( unsigned int rNumber ) = 0;
  virtual IOutputStream& operator<<( signed long rNumber ) = 0;
  virtual IOutputStream& operator<<( unsigned long rNumber ) = 0;
  virtual IOutputStream& operator<<( float rNumber ) = 0;
  virtual IOutputStream& operator<<( double rNumber ) = 0;
  virtual IOutputStream& operator<<( const char* pString ) = 0;

  virtual int write( const bool* pBuffer, int count ) = 0;
  virtual int write( const signed char* pBuffer, int count ) = 0;
  virtual int write( const unsigned char* pBuffer, int count ) = 0;
  virtual int write( const signed short* pBuffer, int count ) = 0;
  virtual int write( const unsigned short* pBuffer, int count ) = 0;
  virtual int write( const signed int* pBuffer, int count ) = 0;
  virtual int write( const unsigned int* pBuffer, int count ) = 0;
  virtual int write( const signed long* pBuffer, int count ) = 0;
  virtual int write( const unsigned long* pBuffer, int count ) = 0;
  virtual int write( const float* pBuffer, int count ) = 0;
  virtual int write( const double* pBuffer, int count ) = 0;
};

inline IOutputStream& operator<<( IOutputStream& rOut, const String& rString )
{
  return rOut << rString.c_str();
}


} // base
} // yake

#endif // YAKE_BASE_OUTPUTSTREAM_H
