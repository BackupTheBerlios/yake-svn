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
#ifndef YAKE_BASE_OUTPUTSTREAMDECORATOR_H
#define YAKE_BASE_OUTPUTSTREAMDECORATOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeOutputStream.h>
#include <yake/base/Templates/yakePointer.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class YAKE_BASE_API IOutputStreamDecorator : public IOutputStream
{
public:
  IOutputStreamDecorator( const Pointer< IOutputStream >& pDecorated )
    : mpDecorated( pDecorated )
  {}

  const Pointer< IOutputStream >& GetDecorated()                  { return mpDecorated; }

  int Seek( int iOffset, SeekBase Base )                          { return mpDecorated->Seek( iOffset, Base ); }
  int Tell() const                                                { return mpDecorated->Tell(); }
  bool IsEos() const                                              { return mpDecorated->IsEos(); }

  IOutputStream& operator<<( bool b )                             { return ( *mpDecorated ) << b; }
  IOutputStream& operator<<( signed char ch )                     { return ( *mpDecorated ) << ch; }
  IOutputStream& operator<<( unsigned char ch )                   { return ( *mpDecorated ) << ch; }
  IOutputStream& operator<<( signed short number )                { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( unsigned short number )              { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( signed int number )                  { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( unsigned int number )                { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( signed long number )                 { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( unsigned long number )               { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( float number )                       { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( double number )                      { return ( *mpDecorated ) << number; }
  IOutputStream& operator<<( const char* pString )                 { return ( *mpDecorated ) << pString; }


  int write( const bool* pBuffer, int count )                    { return mpDecorated->write( pBuffer, count ); }
  int write( const signed char* pBuffer, int count )             { return mpDecorated->write( pBuffer, count ); }
  int write( const unsigned char* pBuffer, int count )           { return mpDecorated->write( pBuffer, count ); }
  int write( const signed short* pBuffer, int count )            { return mpDecorated->write( pBuffer, count ); }
  int write( const unsigned short* pBuffer, int count )          { return mpDecorated->write( pBuffer, count ); }
  int write( const signed int* pBuffer, int count )              { return mpDecorated->write( pBuffer, count ); }
  int write( const unsigned int* pBuffer, int count )            { return mpDecorated->write( pBuffer, count ); }
  int write( const signed long* pBuffer, int count )             { return mpDecorated->write( pBuffer, count ); }
  int write( const unsigned long* pBuffer, int count )           { return mpDecorated->write( pBuffer, count ); }
  int write( const float* pBuffer, int count )                   { return mpDecorated->write( pBuffer, count ); }
  int write( const double* pBuffer, int count )                  { return mpDecorated->write( pBuffer, count ); }

private:
	templates::Pointer< IOutputStream > mpDecorated;
};

} // base
} // yake

#endif // YAKE_BASE_OUTPUTSTREAMDECORATOR_H
