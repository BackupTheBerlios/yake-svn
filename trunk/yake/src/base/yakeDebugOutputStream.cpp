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
#include <inc/base/yakePCH.h>
#include <inc/base/yakeDebugOutputStream.h>
#include <inc/base/yakeException.h>
#include <inc/base/mpl/yakeCast.h>
#include <inc/base/native/yakeNative.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

DebugOutputStream::DebugOutputStream()
{
}


int DebugOutputStream::seek( int offset, SeekBase base )
{
  YAKE_DECLARE_FUNCTION( seek )

  throw yake::base::Exception( "Can't seek in debug output streams.", YAKE_HERE );
}

int DebugOutputStream::tell() const
{
  YAKE_DECLARE_FUNCTION( tell )

  throw yake::base::Exception( "Can't retrieve debug output stream position.", YAKE_HERE );
}

bool DebugOutputStream::isEos() const
{
  return false;
}

  
IOutputStream& DebugOutputStream::operator<<( bool b )
{
	yake::base::native::debug_Print( b ? "true" : "false" );

  return *this;
}

IOutputStream& DebugOutputStream::operator<<( signed char ch )
{
  static char string[ 2 ] = { 0, 0 };

  string[ 0 ] = ch;
  yake::base::native::debug_Print( string );

  return *this;
}

IOutputStream& DebugOutputStream::operator<<( unsigned char ch )
{
  static char string[ 2 ] = { 0, 0 };

  string[ 0 ] = ch;
  yake::base::native::debug_Print( string );

  return *this;
}


IOutputStream& DebugOutputStream::operator<<( signed short number )
{
	yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}

IOutputStream& DebugOutputStream::operator<<( unsigned short number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}


IOutputStream& DebugOutputStream::operator<<( signed int number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}

IOutputStream& DebugOutputStream::operator<<( unsigned int number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}


IOutputStream& DebugOutputStream::operator<<( signed long number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}

IOutputStream& DebugOutputStream::operator<<( unsigned long number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}


IOutputStream& DebugOutputStream::operator<<( float number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}

IOutputStream& DebugOutputStream::operator<<( double number )
{
  yake::base::native::debug_Print( mpl::streamCast< String >( number ).c_str() );

  return *this;
}


IOutputStream& DebugOutputStream::operator<<( const char* string )
{
  yake::base::native::debug_Print( string );

  return *this;
}


int DebugOutputStream::write( const bool* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}


int DebugOutputStream::write( const signed char* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}

int DebugOutputStream::write( const unsigned char* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}


int DebugOutputStream::write( const signed short* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}

int DebugOutputStream::write( const unsigned short* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}


int DebugOutputStream::write( const signed int* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}

int DebugOutputStream::write( const unsigned int* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}


int DebugOutputStream::write( const signed long* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}

int DebugOutputStream::write( const unsigned long* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}


int DebugOutputStream::write( const float* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}

int DebugOutputStream::write( const double* pBuffer, int count )
{
  for( int i = 0; i < count; i++ )
    *this << pBuffer[ i ];

  return count;
}

} // base
} // yake