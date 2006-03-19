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
#ifndef YAKE_BASE_MPL_BUILDITERATION_H
#define YAKE_BASE_MPL_BUILDITERATION_H

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
/* Building an iteration with a comma as separater ( useful for template functions ) */
#define YAKE_ITER_ARG_0( INTERFACE )
#define YAKE_ITER_ARG_1( INTERFACE ) INTERFACE( 1 )
#define YAKE_ITER_ARG_2( INTERFACE ) INTERFACE( 2 ), YAKE_ITER_ARG_1( INTERFACE )
#define YAKE_ITER_ARG_3( INTERFACE ) INTERFACE( 3 ), YAKE_ITER_ARG_2( INTERFACE )
#define YAKE_ITER_ARG_4( INTERFACE ) INTERFACE( 4 ), YAKE_ITER_ARG_3( INTERFACE )
#define YAKE_ITER_ARG_5( INTERFACE ) INTERFACE( 5 ), YAKE_ITER_ARG_4( INTERFACE )
#define YAKE_ITER_ARG_6( INTERFACE ) INTERFACE( 6 ), YAKE_ITER_ARG_5( INTERFACE )
#define YAKE_ITER_ARG_7( INTERFACE ) INTERFACE( 7 ), YAKE_ITER_ARG_6( INTERFACE )
#define YAKE_ITER_ARG_8( INTERFACE ) INTERFACE( 8 ), YAKE_ITER_ARG_7( INTERFACE )
#define YAKE_ITER_ARG_9( INTERFACE ) INTERFACE( 9 ), YAKE_ITER_ARG_8( INTERFACE )

/* Building an iteration without separater */
#define YAKE_ITER_FON_0( INTERFACE ) INTERFACE( 0 )
#define YAKE_ITER_FON_1( INTERFACE ) INTERFACE( 1 ) YAKE_ITER_FON_0( INTERFACE )
#define YAKE_ITER_FON_2( INTERFACE ) INTERFACE( 2 ) YAKE_ITER_FON_1( INTERFACE )
#define YAKE_ITER_FON_3( INTERFACE ) INTERFACE( 3 ) YAKE_ITER_FON_2( INTERFACE )
#define YAKE_ITER_FON_4( INTERFACE ) INTERFACE( 4 ) YAKE_ITER_FON_3( INTERFACE )
#define YAKE_ITER_FON_5( INTERFACE ) INTERFACE( 5 ) YAKE_ITER_FON_4( INTERFACE )
#define YAKE_ITER_FON_6( INTERFACE ) INTERFACE( 6 ) YAKE_ITER_FON_5( INTERFACE )
#define YAKE_ITER_FON_7( INTERFACE ) INTERFACE( 7 ) YAKE_ITER_FON_6( INTERFACE )
#define YAKE_ITER_FON_8( INTERFACE ) INTERFACE( 8 ) YAKE_ITER_FON_7( INTERFACE )
#define YAKE_ITER_FON_9( INTERFACE ) INTERFACE( 9 ) YAKE_ITER_FON_8( INTERFACE )
#define YAKE_ITER_FON_N( INTERFACE ) YAKE_ITER_FON_9( INTERFACE )

#endif // YAKE_BASE_MPL_BUILDITERATION_H

