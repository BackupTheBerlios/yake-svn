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
#ifndef YAKE_BASE_INPUTSTREAMDECORATOR_H
#define YAKE_BASE_INPUTSTREAMDECORATOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeInputStream.h>
#include <yake/base/Templates/yakePointer.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API IInputStreamDecorator : public IInputStream
{
// Class
public:
  IInputStreamDecorator( const SharedPtr< IInputStream >& pDecorated )
    : mpDecorated( pDecorated )
  {}

// Methods
public:

  const SharedPtr< IInputStream >& getDecorated()                   { return mpDecorated; }

  int Seek( int offset, SeekBase base )                           { return mpDecorated->Seek( offset, base ); }
  int Tell() const                                                { return mpDecorated->Tell(); }
  bool IsEos() const                                              { return mpDecorated->IsEos(); }

  IInputStream& operator>>( bool& rB )                            { return ( *mpDecorated ) >> rB; }
  IInputStream& operator>>( signed char& rChar )                  { return ( *mpDecorated ) >> rChar; }
  IInputStream& operator>>( unsigned char& rChar )                { return ( *mpDecorated ) >> rChar; }
  IInputStream& operator>>( signed short& rNumber )               { return ( *mpDecorated ) >> rNumber; }
  IInputStream& operator>>( unsigned short& rNumber )             { return ( *mpDecorated ) >> rNumber; }
  IInputStream& operator>>( signed int& rNumber )                 { return ( *mpDecorated ) >> rNumber; }
  IInputStream& operator>>( unsigned int& rNumber )               { return ( *mpDecorated ) >> rNumber; }
  IInputStream& operator>>( signed long& rNumber )                { return ( *mpDecorated ) >> rNumber; }
  IInputStream& operator>>( unsigned long& rNumber )              { return ( *mpDecorated ) >> rNumber; }

  int read( bool* ppBuffer, int icount )                         { return mpDecorated->read( pBuffer, count ); }
  int read( signed char* pBuffer, int count )                    { return mpDecorated->read( pBuffer, count ); }
  int read( unsigned char* pBuffer, int count )                  { return mpDecorated->read( pBuffer, count ); }
  int read( signed short* pBuffer, int count )                   { return mpDecorated->read( pBuffer, count ); }
  int read( unsigned short* pBuffer, int count )                 { return mpDecorated->read( pBuffer, count ); }
  int read( signed int* pBuffer, int count )                     { return mpDecorated->read( pBuffer, count ); }
  int read( unsigned int* pBuffer, int count )                   { return mpDecorated->read( pBuffer, count ); }
  int read( signed long* pBuffer, int count )                    { return mpDecorated->read( pBuffer, count ); }
  int read( unsigned long* pBuffer, int count )                  { return mpDecorated->read( pBuffer, count ); }
  int read( float* pBuffer, int count )                          { return mpDecorated->read( pBuffer, count ); }
  int read( double* pBuffer, int count )                         { return mpDecorated->read( pBuffer, count ); }

private:
  SharedPtr< IInputStream > mpDecorated;
};

} // base
} // yake

#endif // YAKE_BASE_INPUTSTREAMDECORATOR_H
