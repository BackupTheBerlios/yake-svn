/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
inline NxReal fromNx( const NxReal v )
{ return ::yake::real(v); }

inline NxVec3 toNx( const ::yake::base::Vector3 & v )
{ return NxVec3( v.x, v.y, v.z ); }
inline ::yake::base::Vector3 fromNx( const NxVec3 & v )
{ return ::yake::base::Vector3( v.x, v.y, v.z ); }

inline NxQuat toNx( const ::yake::base::Quaternion & v )
{ 
	NxQuat q;
	q.setWXYZ( v.w, v.x, v.y, v.z );
	return q;
}
inline ::yake::base::Quaternion fromNx( const NxQuat & v )
{ return ::yake::base::Quaternion( v.w, v.x, v.y, v.z ); }

#endif
