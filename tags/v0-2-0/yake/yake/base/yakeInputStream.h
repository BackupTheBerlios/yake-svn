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
#ifndef YAKE_BASE_INPUTSTREAM_H
#define YAKE_BASE_INPUTSTREAM_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API IInputStream
{
// Types
public:
  enum SeekBase
  { BEGIN, CURRENT, END };

// Class
public:
  virtual ~IInputStream();

// Methods
public:
  virtual int seek( int offset, SeekBase base ) = 0;
  virtual int tell() const = 0;
  virtual bool isEos() const = 0;

  virtual IInputStream& operator>>( bool& rB ) = 0;
  virtual IInputStream& operator>>( signed char& rCh ) = 0;
  virtual IInputStream& operator>>( unsigned char& rCh ) = 0;
  virtual IInputStream& operator>>( signed short& rNumber ) = 0;
  virtual IInputStream& operator>>( unsigned short& rNumber ) = 0;
  virtual IInputStream& operator>>( signed int& rNumber ) = 0;
  virtual IInputStream& operator>>( unsigned int& rNumber ) = 0;
  virtual IInputStream& operator>>( signed long& rNumber ) = 0;
  virtual IInputStream& operator>>( unsigned long& rNumber ) = 0;
  virtual IInputStream& operator>>( float& rNumber ) = 0;
  virtual IInputStream& operator>>( double& rNumber ) = 0;

  virtual int Read( bool* pBuffer, int count ) = 0;
  virtual int Read( signed char* pBuffer, int count ) = 0;
  virtual int Read( unsigned char* pBuffer, int count ) = 0;
  virtual int Read( signed short* pBuffer, int count ) = 0;
  virtual int Read( unsigned short* pBuffer, int count ) = 0;
  virtual int Read( signed int* pBuffer, int count ) = 0;
  virtual int Read( unsigned int* pBuffer, int count ) = 0;
  virtual int Read( signed long* pBuffer, int count ) = 0;
  virtual int Read( unsigned long* pBuffer, int count ) = 0;
  virtual int Read( float* pBuffer, int count ) = 0;
  virtual int Read( double* pBuffer, int count ) = 0;
};

} // base
} // yake

#endif // YAKE_BASE_INPUTSTREAM_H
