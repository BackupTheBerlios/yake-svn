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
#ifndef __INC_PCH_H__
#define __INC_PCH_H__

#include "NxPhysics.h"

//YAKE:

#include <yake/base/yake.h>
#include <yake/physics/yakePhysics.h>

#ifdef YAKE_PHYSICS_NX_EXPORTS
#	define YAKE_PHYSICSNX_API DLLEXPORT
#else
#	define YAKE_PHYSICSNX_API DLLIMPORT
#endif

namespace yake {
namespace physics {
	NxPhysicsSDK* getNxSDK();
}
}

inline NxReal toNx( const ::yake::real v )
{ return NxReal(v); }
inline ::yake::real fromNx( const NxReal v )
{ return ::yake::real(v); }

inline NxVec3 toNx( const ::yake::Vector3 & v )
{ return NxVec3( v.x, v.y, v.z ); }
inline ::yake::Vector3 fromNx( const NxVec3 & v )
{ return ::yake::Vector3( v.x, v.y, v.z ); }

inline NxQuat toNx( const ::yake::Quaternion & v )
{ 
	NxQuat q;
	q.setWXYZ( v.w, v.x, v.y, v.z );
	return q;
}
inline ::yake::Quaternion fromNx( const NxQuat & v )
{ return ::yake::Quaternion( v.w, v.x, v.y, v.z ); }

#endif
