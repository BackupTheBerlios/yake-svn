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

		virtual void setTargetVelocity( const Vector3 & rkTargetVelocity );
		virtual void jump();
		virtual void duck();

		// helper

		/** to be called each step! */
		virtual void update_(const real timeElapsed);
	private:
		bool		mJumping;
		bool		mJumpInitiated;
		bool		mDucking;
		Vector3		mPosition;
		Vector3		mTargetVelocity;
		NxScene&	mNxScene;
		real		mTargetHeightAboveGround;
		SignalConnection mStepSigConn;
		real		mRadius;

		SharedPtr<IActor> mBall;
	};

}
}
#endif