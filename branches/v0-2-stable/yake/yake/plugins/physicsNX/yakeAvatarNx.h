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
#ifndef YAKE_PHYSICS_AVATARNX_H
#define YAKE_PHYSICS_AVATARNX_H

namespace yake {
namespace physics {

	class AvatarNx : public IAvatar
	{
	public:
		AvatarNx( NxScene & rScene, WorldNx & rWorldNx );
		virtual ~AvatarNx();

		virtual void setPosition( const Vector3 & rkPosition );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & rkOrientation );
		virtual Quaternion getOrientation() const;

		virtual void setDimensions( const Vector3 & rkDimensions ) {}
		virtual void setInfluenceByDynamics( const real ratio ) {}

		virtual void setTargetVelocity( const Vector3 & rkTargetVelocity );
		virtual void jump();
		virtual void duck();

		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, OnJump )
		YAKE_MEMBERSIGNAL_FIRE_FN1( public, OnJump, bool fire, fire )
		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, OnDuck )
		YAKE_MEMBERSIGNAL_FIRE_FN1( public, OnDuck, bool fire, fire )

		// helper

		/** to be called each step! */
		virtual void update_(const real timeElapsed);
	private:
		bool		mJumping;
		bool		mJumpInitiated;
		bool		mJumpTakingOff;
		bool		mDucking;
		Vector3		mPosition;
		Vector3		mTargetVelocity;
		NxScene&	mNxScene;
		WorldNx&	mWorld;
		real		mTargetHeightAboveGround;
		SignalConnection mStepSigConn;
		real		mRadius;

		IActorPtr	mpBall;
	};

}
}
#endif