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
#ifndef YAKE_BASE_MPL_BUILDARGUMENTS_H
#define YAKE_BASE_MPL_BUILDARGUMENTS_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include "yakeBuildIteration.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
// typename T2, typename T1, typename T0
#define YAKE_TYPENAME( number ) typename T##number
#define YAKE_TYPES_0
#define YAKE_TYPES_1 template< YAKE_ITER_ARG_1( YAKE_TYPENAME ) >
#define YAKE_TYPES_2 template< YAKE_ITER_ARG_2( YAKE_TYPENAME ) >
#define YAKE_TYPES_3 template< YAKE_ITER_ARG_3( YAKE_TYPENAME ) >
#define YAKE_TYPES_4 template< YAKE_ITER_ARG_4( YAKE_TYPENAME ) >
#define YAKE_TYPES_5 template< YAKE_ITER_ARG_5( YAKE_TYPENAME ) >
#define YAKE_TYPES_6 template< YAKE_ITER_ARG_6( YAKE_TYPENAME ) >
#define YAKE_TYPES_7 template< YAKE_ITER_ARG_7( YAKE_TYPENAME ) >
#define YAKE_TYPES_8 template< YAKE_ITER_ARG_8( YAKE_TYPENAME ) >
#define YAKE_TYPES_9 template< YAKE_ITER_ARG_9( YAKE_TYPENAME ) >
#define YAKE_TYPES( number ) YAKE_TYPES_##number

// typename T, typename T2, typename T1, typename T0
#define YAKE_TYPES_ONE_FREE_0 template< typename T >
#define YAKE_TYPES_ONE_FREE_1 template< typename T, YAKE_ITER_ARG_1( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_2 template< typename T, YAKE_ITER_ARG_2( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_3 template< typename T, YAKE_ITER_ARG_3( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_4 template< typename T, YAKE_ITER_ARG_4( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_5 template< typename T, YAKE_ITER_ARG_5( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_6 template< typename T, YAKE_ITER_ARG_6( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_7 template< typename T, YAKE_ITER_ARG_7( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_8 template< typename T, YAKE_ITER_ARG_8( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE_9 template< typename T, YAKE_ITER_ARG_9( YAKE_TYPENAME ) >
#define YAKE_TYPES_ONE_FREE( number ) YAKE_TYPES_ONE_FREE_##number

// typename TT, typename T2, typename T1, typename T0
#define YAKE_TYPES_TWO_FREE_0 template< typename T, typename TT >
#define YAKE_TYPES_TWO_FREE_1 template< typename T, typename TT, YAKE_ITER_ARG_1( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_2 template< typename T, typename TT, YAKE_ITER_ARG_2( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_3 template< typename T, typename TT, YAKE_ITER_ARG_3( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_4 template< typename T, typename TT, YAKE_ITER_ARG_4( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_5 template< typename T, typename TT, YAKE_ITER_ARG_5( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_6 template< typename T, typename TT, YAKE_ITER_ARG_6( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_7 template< typename T, typename TT, YAKE_ITER_ARG_7( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_8 template< typename T, typename TT, YAKE_ITER_ARG_8( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE_9 template< typename T, typename TT, YAKE_ITER_ARG_9( YAKE_TYPENAME ) >
#define YAKE_TYPES_TWO_FREE( number ) YAKE_TYPES_TWO_FREE_##number

// const T2& t2, const T1& t1, const T0& t0
#define YAKE_ARGUMENT( number ) const T##number##& t##number
#define YAKE_ARGUMENTS( number ) YAKE_ITER_ARG_##number ( YAKE_ARGUMENT )

// const T2& t2, const T1& t1, const T0& t0
#define YAKE_ARGUMENTS_ONE_FREE_0 yake::base::mpl::EmptyType Null = yake::base::mpl::EmptyType()
#define YAKE_ARGUMENTS_ONE_FREE_1 YAKE_ITER_ARG_1( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_2 YAKE_ITER_ARG_2( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_3 YAKE_ITER_ARG_3( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_4 YAKE_ITER_ARG_4( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_5 YAKE_ITER_ARG_5( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_6 YAKE_ITER_ARG_6( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_7 YAKE_ITER_ARG_7( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_8 YAKE_ITER_ARG_8( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE_9 YAKE_ITER_ARG_9( YAKE_ARGUMENT )
#define YAKE_ARGUMENTS_ONE_FREE( number ) YAKE_ARGUMENTS_ONE_FREE_##number

// const_cast< T2& >( t2 ), const_cast< T1& >( t1 ), const_cast< T0& >( t0 )
#define YAKE_USE_ARGUMENT( number ) const_cast< T##number##& >( t##number )
#define YAKE_USE_ARGUMENTS( number ) YAKE_ITER_ARG_##number ( YAKE_USE_ARGUMENT )

// F(), F( const T0& t0 ), F( const T1& t1, const T0& t0 ), ...
#define YAKE_IMPLEMENT_FUNCTION( FUNCTION ) YAKE_ITER_FON_N( FUNCTION )


#endif // YAKE_BASE_MPL_BUILDARGUMENTS_H
