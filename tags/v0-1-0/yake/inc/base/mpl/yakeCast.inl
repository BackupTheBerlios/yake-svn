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
#include <inc/base/mpl/yakeTraits.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{
namespace detail
{

  template< bool isFundamental >
  struct getBasePointer1
  {
    template< typename T >
    static void* execute( T* pointer )
    {
		    void* p = static_cast< void* >( pointer );

		    return p;
    }
  };

  template<>
  struct getBasePointer1< false >
  {
    template< typename T >
    static void* execute( T* pointer )
    {
      return getBasePointer2< IsPolymorphic< T >::value >::execute( pointer );
    }
  };

  template< bool bIsPolymorphic >
  struct getBasePointer2
  {
    template< typename T >
    static void* execute( T* pointer )
    {
	    void* p = dynamic_cast< void* >( pointer );

	    return p;
    }
  };

  template<>
  struct getBasePointer2< false >
  {
    template< typename T >
    static void* execute( T* pointer )
    {
		    void* p = static_cast< void* >( pointer );

		    return p;
    }
  };
} // detail

template< typename T >
void* getBasePointer( T* pointer )
{
  return detail::getBasePointer1< boost::is_fundamental< T >::value >::execute( pointer );
}


namespace detail
{
  template< typename T, typename U >
  struct PolymorphicCast
  {
    template< bool isPolymorphic >
    struct Result
    {
      static U* execute( T* pointer )
      {
      return dynamic_cast< U* >( pointer );
      }
    };
  
    template<>
    struct Result< false >
    {
      static U* execute( T* pointer )
      {
      return pointer;
      }
    };
  };
} // detail

template< typename U, typename T >
U* PolymorphicCast< U, T >::execute( T* pointer )
{
	return detail::PolymorphicCast< T, U >::Result< IsPolymorphic< U >::value >::execute( pointer );
}


template< typename OutType, typename InType >
OutType streamCast( const InType& rFrom )
{
	std::stringstream ss;
	OutType result;   // value will be converted to out_type

	ss << rFrom;      // first insert value to stream
	ss >> result;     // write value to result

	return result;
}

namespace detail
{

template< typename T >
struct swapBytesHelper1
{
  template< int numBytes >
  struct swapBytesHelper2;

  template<>
  struct swapBytesHelper2< 1 >
  {
    static void Execute( T& t )
    {
    }
  };

  template<>
  struct swapBytesHelper2< 2 >
  {
    static void Execute( T& t )
    {
      int16_t& i = safe_reinterpret_cast< int16_t& >( t );

      i = ( int16_t )( ( i<<8) | (i>>8) );
    }
  };

  template<>
  struct swapBytesHelper2< 4 >
  {
    static void execute( T& t )
    {
      int32_t& i = safe_reinterpret_cast< int32_t& >( t );

	    i = (i>>24) |
		    (i>> 8) & 0x0000ff00 |
		    (i<< 8) & 0x00ff0000 |
		    (i<<24);
    }
  };

  template<>
  struct swapBytesHelper2< 8 >
  {
    static void execute( T& t )
    {
      int64_t& i = safe_reinterpret_cast< int64_t& >( t );

	    i = (i>>56) |
		    (i>>40) & 0x000000000000ff00 |
		    (i>>24) & 0x0000000000ff0000 |
		    (i>> 8) & 0x00000000ff000000 |
		    (i<< 8) & 0x000000ff00000000 |
		    (i<<24) & 0x0000ff0000000000 |
		    (i<<40) & 0x00ff000000000000 |
		    (i<<56);
    }
  };

  static void execute( T& t)
  {
    swapBytesHelper2< sizeof( T ) >::execute( t );
  }
};

}// detail


template< typename T >
void swapBytes( T& t )
{
	detail::swapBytesHelper1< T >::execute( t );
}

template< typename T >
void swapBytes( T* buffer, int count )
{
  for( int i = 0; i < count; i++ )
    swapBytes( buffer[ i ] );
}


template< typename T >
T join( const std::vector< T >& what, const T& separator )
{
  T result;

  for( std::vector< T >::const_iterator iter = what.begin(); iter != what.end(); iter++ )
  {
    result += *iter;

    if( iter != what.end() - 1 )
      result += separator;
  }

  return result;
}

template< typename T >
std::vector< T > split( const T& where, const T& separator )
{
  std::vector< T > result;

  T::size_type oldpos = 0, pos = 0;
  while( ( pos = where.find( separator, pos ) ) != T::npos )
  {
    result.push_back( where.substr( oldpos, pos - oldpos ) );

    pos += separator.size();
    oldpos = pos;
  }

  result.push_back( where.substr( oldpos, pos - oldpos ) );

  return result;
}

} // mpl
} // base
} // yake
