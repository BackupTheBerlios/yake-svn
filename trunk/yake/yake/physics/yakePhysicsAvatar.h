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
#ifndef YAKE_PHYSICS_AVATAR_H
#define YAKE_PHYSICS_AVATAR_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace ::yake::math;
namespace physics {

	class YAKE_PHYSICS_API IAvatarListener
	{
	public:
		virtual ~IAvatarListener() {}

		virtual void onJump(const bool jumping) = 0;
		virtual void onDuck(const bool ducking) = 0;
		virtual void onCollision(IActor* pOther) = 0;
	};

	/** An avatar in a physical world. */
	class YAKE_PHYSICS_API IAvatar : public Movable, public ListenerManager<IAvatarListener>
	{
	public:
		struct Desc
		{
			Desc(	const Vector3 & rkDimensions = Vector3(0,2,0),
					const Vector3 & rkPosition = Vector3::kZero,
					const Quaternion & rkOrientation = Quaternion::kIdentity) :
				position( rkPosition ),
				orientation( rkOrientation ),
				dimensions( rkDimensions )
			{}
			Vector3		position;
			Quaternion	orientation;
			Vector3		dimensions;
		};
	public:

		virtual void setDimensions( const Vector3 & rkDimensions ) = 0;
		virtual void setInfluenceByDynamics( const real ratio ) = 0;

		virtual void setTargetVelocity( const Vector3 & rkTargetVelocity ) = 0;
		virtual void jump() = 0;
		virtual bool isJumping() const = 0;
		virtual void duck(const bool yes) = 0;
		virtual bool isDucking() const = 0;

		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(bool), OnJump )
		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(bool), OnDuck )
	};
	YAKE_PHYSICS_COMMON_POINTERS( IAvatar );

}
}

#endif
