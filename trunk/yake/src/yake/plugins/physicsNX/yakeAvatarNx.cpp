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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakeWorldNx.h>
#include <yake/plugins/physicsNX/yakeActorNx.h>
#include <yake/plugins/physicsNX/yakeAvatarNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace base::math;
namespace physics {

	/**

			O   <- ball
			|   <- ray

		 __
		/   \
	   |  x  |    x is ball position
		\_|_/     
		  R       R is rayOrigin = mpBall.position + rayDirection * (radius + 0.1 *offset so to not intersect the ball*)
          |
		  |

		Ball:
			- dimension: d=0.4 ; r=0.5*d
		Ray:
			- origin: = mpBall.position + rayDirection * (radius + 0.1)

		INPUT: targetVel

		F = m * a
		v = a * t  |  a = v / t

		=> F = m * (v/t)

	*/

	AvatarNx::AvatarNx( NxScene & rScene, WorldNx & rWorldNx ) :
		mNxScene( rScene ),
		mWorld( rWorldNx ),
		mJumping( false ),
		mJumpInitiated( false ),
		mJumpTakingOff( false ),
		mDucking( false ),
		mTargetVelocity( Vector3::kZero ),
		mTargetHeightAboveGround( 1.0 ),
		mPosition( 0,1.0,0 ),
		mRadius(real(0.2))
	{
		mStepSigConn = rWorldNx.stepSignal.connect( boost::bind(AvatarNx::update_, this, _1) );

		mpBall = rWorldNx.createDynamicActor();
		YAKE_ASSERT( !mpBall.expired() );
		mpBall.lock()->setPosition( mPosition );
		mpBall.lock()->createShape( IShape::SphereDesc(mRadius) );
		mpBall.lock()->getBody().setMass( 1 );
	}
	AvatarNx::~AvatarNx()
	{
		YAKE_ASSERT( !mpBall.expired() );
		mWorld.destroyActor( WeakIActorPtr( mpBall ) );
		YAKE_ASSERT( mpBall.expired() );

		mStepSigConn.disconnect();
	}
	void AvatarNx::setPosition( const Vector3 & rkPosition )
	{
		mPosition = rkPosition;
	}
	Vector3 AvatarNx::getPosition() const
	{
		if (!mpBall.expired())
			return mpBall.lock()->getPosition();
		else
			return mPosition;
	}
	void AvatarNx::setOrientation( const Quaternion & rkOrientation )
	{
	}
	Quaternion AvatarNx::getOrientation() const
	{
		return Quaternion::kIdentity;
	}
	void AvatarNx::setTargetVelocity( const Vector3 & rkTargetVelocity )
	{
		mTargetVelocity = rkTargetVelocity;
	}
	void AvatarNx::jump()
	{
		if (mJumping)
			return;
		mJumping = true;
		mJumpInitiated = false;
		mJumpTakingOff = false;
	}
	void AvatarNx::duck()
	{
		mDucking = true;
	}
	void AvatarNx::update_(const real timeElapsed)
	{
		YAKE_ASSERT( !mpBall.expired() );

		NxRaycastHit hit;

		const Vector3 rayOffset(0,-mRadius*1.5,0);

		mPosition = mpBall.lock()->getPosition();

		// origin shoud be the lowest point of the "body"/capsule/...
		const NxRay ray( toNx( mPosition + rayOffset ), NxVec3(0,-1,0) );

		NxShape* pFirstHitShape = 
			mNxScene.raycastClosestShape(
						ray,
						NX_ALL_SHAPES, // NX_STATIC_SHAPES or NX_DYNAMIC_SHAPES or NX_ALL_SHAPES
						hit
						); // all groups.. could change that with additional 4th param.
		real distanceToImpact = 0;
		if (!pFirstHitShape)
		{
			YAKE_ASSERT( pFirstHitShape );
			YAKE_LOG( "NX: falling..." );
			std::cout << "ball: " << mpBall.lock()->getPosition().x << ", " << mpBall.lock()->getPosition().y << ", " << mpBall.lock()->getPosition().z << "\n";
			//return; // uh oh.. no ground below us.
		}
		else
		{
			YAKE_ASSERT( hit.shape );
			YAKE_ASSERT( hit.flags & NX_RAYCAST_DISTANCE );
			YAKE_ASSERT( hit.flags & NX_RAYCAST_IMPACT );
			//const Vector3 impactPt = fromNx( hit.worldImpact );
			distanceToImpact = fromNx( hit.distance );
		}

		//std::cout << "ball: " << mpBall.lock()->getPosition().x << ", " << mpBall.lock()->getPosition().y << ", " << mpBall.lock()->getPosition().z << "\n";

		mTargetVelocity.y = 0.; // !!!

		//Vector3 targetPos = mPosition + mTargetVelocity * timeElapsed;

		if (!mJumping)
		{
			//targetPos.y = mTargetHeightAboveGround;
		}
		else
		{
			YAKE_LOG( "still jumping...\n" );
			std::cout << "distance = " << distanceToImpact << "\n";
			if (!mJumpInitiated)
			{
				YAKE_LOG( "initiating...\n" );
				mpBall.lock()->getBody().addForce( Vector3(0,200,0) );
				mJumpInitiated = true;
				mJumpTakingOff = true;
			}
			else if (mJumpTakingOff)
			{
				YAKE_LOG( "taking off...\n" );
				if (distanceToImpact < mTargetHeightAboveGround)
					mpBall.lock()->getBody().addForce( Vector3(0,200,0) );
				else
					mJumpTakingOff = false;
			}
			else
			{
				if (distanceToImpact < mTargetHeightAboveGround)
				{
					YAKE_LOG( "landed.\n" );
					mJumping = false;
					mJumpInitiated = false;
					mJumpTakingOff = false;
				}
				else
				{
					std::cout << "...and on and on...\n";
					// still jumping?
				}
			}
		}

		// gravity
		//mpBall.lock()->getBody().addForce(Vector3(0,-10,0));

		// move the ball
		if (timeElapsed > 0.)
		{
			Vector3 linVel = mpBall.lock()->getBody().getLinearVelocity();
			if (mJumping)
				linVel.y = 0; // !!!
			Vector3 velocityDifference = mTargetVelocity - linVel;
			if (!mJumping)
			{
				if (distanceToImpact < mTargetHeightAboveGround)
					velocityDifference += Vector3(0,mTargetHeightAboveGround-distanceToImpact,0) * 8;
				else if (distanceToImpact > mTargetHeightAboveGround)
				{
					velocityDifference += Vector3(0,-9.81,0);
				}
			}


			// F = m * a
			// a = v/t
			// => F = m * (v/t)

			Vector3 n = (velocityDifference).normalisedCopy();
			Vector3 force = mpBall.lock()->getBody().getMass() * n * (velocityDifference.length() / timeElapsed);

			mpBall.lock()->getBody().addForce( force );
		}
	}

}
}