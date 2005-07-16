/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#include <yake/base/yakePCH.h>

namespace yake {

	Movable::Movable()
	{}

	Movable::~Movable()
	{}

	/// @remarks Another shameless rip from OGRE (http://www.ogre3d.org)
// 	void Movable::lookAt( const Vector3 & target )
// 	{
// 		this->setDirection(target - getPosition() /*FIXME:*//*mDerivedPosition*/);
// 	}

	/// @remarks Another shameless rip from OGRE (http://www.ogre3d.org)
// 	void Movable::setDirection( const Vector3 & vec )
// 	{
// 		if (vec == Vector3::kZero)
// 			return;
// 
//         Vector3 zAdjustVec = vec;
//         zAdjustVec.normalise();
// 
//         if( mFixedYaw )
//         {
//             Vector3 xVec = mYawAxis.crossProduct( zAdjustVec );
//             xVec.normalise();
// 
//             Vector3 yVec = zAdjustVec.crossProduct( xVec );
//             yVec.normalise();
// 
// 			Quaternion orientation;
//             orientation.FromAxes( xVec, yVec, zAdjustVec );
// 			setOrientation( orientation );
//         }
//         else
//         {
// 
//             // Get axes from current quaternion
//             Vector3 axes[3];
//             Quaternion derivedOrientation = getOrientation(); //FIXME
// 			derivedOrientation.ToAxes(axes[0],axes[1],axes[2]);
//             Quaternion rotQuat;
//             if (-zAdjustVec == axes[2])
//             {
//                 // Oops, a 180 degree turn (infinite possible rotation axes)
//                 // Default to yaw i.e. use current UP
//                 rotQuat.FromAngleAxis(Math::PI, axes[1]);
//             }
//             else
//             {
//                 // Derive shortest arc to new direction
//                 rotQuat = axes[2].getRotationTo(zAdjustVec);
// 
//             }
//             setOrientation( rotQuat * getOrientation() );
//         }
// 
// 
//         // TODO If we have a fixed yaw axis, we mustn't break it by using the
//         // shortest arc because this will sometimes cause a relative yaw
//         // which will tip the camera	
// 	}

} // yake
