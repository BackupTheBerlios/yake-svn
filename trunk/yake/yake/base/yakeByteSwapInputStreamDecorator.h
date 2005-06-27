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
#ifndef YAKE_BASE_BYTESWAPINPUTSTREAMDECORATOR_H
#define YAKE_BASE_BYTESWAPINPUTSTREAMDECORATOR_H

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

// This macro converts a native stream into a little endian stream.
#ifdef YAKE_BIG_ENDIAN
#   define YAKE_LITTLE_ENDIAN_INPUT_STREAM( inputStreamPointer )   create< yake::base::ByteSwapInputStreamDecorator >( inputStreamPointer )
#else
#   define YAKE_LITTLE_ENDIAN_INPUT_STREAM( inputStreamPointer )   inputStreamPointer
#endif


class YAKE_BASE_API ByteSwapInputStreamDecorator : public IInputStreamDecorator
{
// Class
public:
  ByteSwapInputStreamDecorator( const SharedPtr< IInputStream >& pDecorated )
    : IInputStreamDecorator( pDecorated )
  {}

// Methods
public:
  IInputStream& operator>>( bool& rB )                 { ( *getDecorated() ) >> rB; swapBytes( rB ); return *this; }
  IInputStream& operator>>( signed char& rChar )       { ( *getDecorated() ) >> rChar; swapBytes( rChar ); return *this; }
  IInputStream& operator>>( unsigned char& rChar )     { ( *getDecorated() ) >> rChar; swapBytes( rChar ); return *this; }
  IInputStream& operator>>( signed short& rNumber )    { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( unsigned short& rNumber )  { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( signed int& rNumber )      { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( unsigned int& rNumber )    { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( signed long& rNumber )     { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( unsigned long& rNumber )   { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( float& rNumber )           { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }
  IInputStream& operator>>( double& rNumber )          { ( *getDecorated() ) >> rNumber; swapBytes( rNumber ); return *this; }

  int read( bool* pBuffer, int count )               { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( signed char* pBuffer, int count )        { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( unsigned char* pBuffer, int count )      { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( signed short* pBuffer, int count )       { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( unsigned short* pBuffer, int count )     { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( signed int* pBuffer, int count )         { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( unsigned int* pBuffer, int count )       { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( signed long* pBuffer, int count )        { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( unsigned long* pBuffer, int count )      { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( float* pBuffer, int count )              { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
  int read( double* pBuffer, int count )             { int read = getDecorated()->read( pBuffer, count ); swapBytes( pBuffer, read ); return read; }
};

} // base
} // yake

#endif // YAKE_BASE_BYTESWAPINPUTSTREAMDECORATOR_H
