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
#ifndef YAKE_BASE_BYTESWAPOUTPUTSTREAMDECORATOR_H
#define YAKE_BASE_BYTESWAPOUTPUTSTREAMDECORATOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake

#define YAKE_SWAP_AND_OUTPUT_ARRAY( variable, type, count ) \
  type* temp = new type[ count ]; \
  memcpy( temp, variable, count ); \
  swapBytes( temp, count ); \
  int written = ( *getDecorated() ).write( temp, count ); \
  delete[] temp; \
  return written;

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

// This macro converts a native stream into a little endian stream.
#ifdef YAKE_BIG_ENDIAN
# define YAKE_LITTLE_ENDIAN_OUTPUT_STREAM( outputStreamPointer )   create< yake::base::ByteSwapOutputStreamDecorator >( outputStreamPointer )
#else
# define YAKE_LITTLE_ENDIAN_OUTPUT_STREAM( outputStreamPointer )   outputStreamPointer
#endif


class YAKE_BASE_API ByteSwapOutputStreamDecorator : public IOutputStreamDecorator
{
// Class
public:
  ByteSwapOutputStreamDecorator( const Pointer< IOutputStream >& pDecorated )
    : IOutputStreamDecorator( pDecorated )
  {}

// Methods
public:
  IOutputStream& operator<<( bool b )                 { swapBytes( b ); return ( *getDecorated() ) << b; }
  IOutputStream& operator<<( signed char ch )         { swapBytes( ch ); return ( *getDecorated() ) << ch; }
  IOutputStream& operator<<( unsigned char ch )       { swapBytes( ch ); return ( *getDecorated() ) << ch; }
  IOutputStream& operator<<( signed short number )    { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( unsigned short number )  { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( signed int number )      { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( unsigned int number )    { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( signed long number )     { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( unsigned long number )   { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( float number )           { swapBytes( number ); return ( *getDecorated() ) << number; }
  IOutputStream& operator<<( double number )          { swapBytes( number ); return ( *getDecorated() ) << number; }

  IOutputStream& operator<<( const char* pString )
  {
    int count = strlen( pString );
    char* temp = new char[ count ];
    memcpy( temp, pString, count );
    swapBytes( temp, count );
    ( *getDecorated() ) << temp;
    delete[] temp;
    return *this;
  }
  
  IOutputStream& operator<<( const wchar_t* pString )
  {
    int count = wcslen( pString );
    wchar_t* temp = new wchar_t[ count ];
    memcpy( temp, pString, count );
    swapBytes( temp, count );
    ( *getDecorated() ) << temp;
    delete[] temp;
    return *this;
  }


  int write( const bool* pBuffer, int count )              { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, bool, count ) }
  int write( const signed char* pBuffer, int count )       { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, signed char, count ) }
  int write( const unsigned char* pBuffer, int count )     { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, unsigned char, count ) }
  int write( const signed short* pBuffer, int count )      { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, signed short, count ) }
  int write( const unsigned short* pBuffer, int count )    { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, unsigned short, count ) }
  int write( const signed int* pBuffer, int count )        { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, signed int, count ) }
  int write( const unsigned int* pBuffer, int count )      { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, unsigned int, count ) }
  int write( const signed long* pBuffer, int count )       { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, signed long, count ) }
  int write( const unsigned long* pBuffer, int count )     { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, unsigned long, count ) }
  int write( const float* pBuffer, int count )             { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, float, count ) }
  int write( const double* pBuffer, int count )            { YAKE_SWAP_AND_OUTPUT_ARRAY( pBuffer, double, count ) }
};

#undef YAKE_SWAP_AND_OUTPUT_ARRAY

} // base
} // yake

#endif // YAKE_BASE_BYTESWAPOUTPUTSTREAMDECORATOR_H
