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
#ifndef YAKE_BASE_MPL_CAST_H
#define YAKE_BASE_MPL_CAST_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "../yakePrerequisites.h"
#endif
// Yake
#include <yake/base/native/yakeNative.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{

template< typename T >
void* getBasePointer( T* pointer );

template< typename U, typename T >
struct PolymorphicCast
{
	static U* execute( T* pointer );
};

template< typename OutType, typename InType >
OutType streamCast( const InType& from );

template< typename T >
void  swapBytes( T& t );

template< typename T >
void swapBytes( T* buffer, int count );

template< typename T >
T join( const std::vector< T >& what, const T& separator );

template< typename T >
std::vector< T > split( const T& where, const T& separator );

template< typename From, typename To >
To safe_reinterpret_cast( From from )
{
	COMPILE_TIME_CHECK( sizeof(From) <= sizeof(To), Destination_Type_Too_Narrow );
	return reinterpret_cast< To >( from );
}

} // mpl
} // base
} // yake

#include "yakeCast.inl"

#endif // YAKE_BASE_MPL_CAST_H
