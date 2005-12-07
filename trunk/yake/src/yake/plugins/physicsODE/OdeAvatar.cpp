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
#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeJoint.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeActor.h>
#include <yake/plugins/physicsODE/OdeAvatar.h>
#include <yake/plugins/physicsODE/OdeShapes.h>
#include <yake/plugins/physicsODE/OdeRay.h>

namespace yake {
namespace physics {

    //-----------------------------------------------------
    //	OdeAvatar
    //-----------------------------------------------------
	
    //-----------------------------------------------------
    OdeAvatar::OdeAvatar( OdeWorld* pWorld ) :
		mOdeWorld(pWorld),
		mRay(0),
		mActor(0),
		mDynamicsInfluence( real(0.8) ),
		mHeightAboveGround( real(1.0) ),
		mHeightAboveGroundDuck( real(0.5) ),
		mJumping(false),
		mJumpStartInProgress(false),
		mDucking(false),
		mJumpApplies(0),
		mJumpTime(real(0.))
    {
    }

    //-----------------------------------------------------
    OdeAvatar::~OdeAvatar()
    {
		mPreStepConn.disconnect();
		mPostStepConn.disconnect();
		YAKE_SAFE_DELETE( mRay );
		mOdeWorld->destroyActor( mActor );
		mActor = 0;
		mOdeWorld = 0;
    }

    //-----------------------------------------------------
	bool OdeAvatar::init( const IAvatar::Desc& desc )
    {
		YAKE_ASSERT( mOdeWorld );
		YAKE_ASSERT( !mActor );
		YAKE_ASSERT( !mRay );

		mPosition = desc.position;
		mOrientation = desc.orientation;
		mDimensions = desc.dimensions;

		mHeightAboveGround = mDimensions.y*real(0.75);
		mHeightAboveGroundDuck = 0.5 * mHeightAboveGround;

		mSphereRadius = mDimensions.x * real(0.5);
		mSphereOffset = Vector3(0,mHeightAboveGround,0);
		mRayLength = 100.; // set to expected maximum height above ground... or better...

		mActor = static_cast<OdeActor*>(static_cast<IWorld*>(mOdeWorld)->createActor( ACTOR_DYNAMIC ));
		YAKE_ASSERT( mActor );
		OdeGeom* pShape = static_cast<OdeGeom*>(mActor->createShape( IShape::SphereDesc( real(mSphereRadius) ) ));
		mActor->setPosition( mPosition + mSphereOffset );

		mRay = new OdeRay( mOdeWorld, real(mRayLength) );
		mRay->setDirection( -Vector3::kUnitY );
		mRay->ignore( pShape->_getOdeGeomID() );

		mPreStepConn = mOdeWorld->subscribeToPreStepInternal( boost::bind(&OdeAvatar::onPreStepInternal,this,_1) );
		mPostStepConn = mOdeWorld->subscribeToPostStepInternal( boost::bind(&OdeAvatar::onPostStepInternal,this,_1) );
		return true;
    }

	//-----------------------------------------------------
	void OdeAvatar::onPostStepInternal(const real dt)
	{
		if (dt <= 0)
			return;
		// gather information

		mPosition = mActor->getPosition();
		mOrientation = mActor->getOrientation();

		const real targetHeightAboveGround = mDucking ? 
						mHeightAboveGroundDuck : mHeightAboveGround;

		const Vector3 currLinVel = mActor->getBody().getLinearVelocity();
		const real currVel = currLinVel.y;
		const Vector3 currPos = mActor->getPosition();

		// let's do some real work:

		Vector3 targetVel = 2 * (mTargetVelocity - currLinVel) * dt;
		targetVel.y = 0.; // !
		Vector3 f(0,0,0);
		if (mJumpStartInProgress)
		{
			if (mJumpTime > 0.)
			{
				// "Starting phase" (we apply force)
				mJumpTime -= dt;

				f += Vector3(0,100,0);
				++mJumpApplies;
			}
			else
			{
				// "Jumping phase"
				const real minGlobalHeight = mRay->intersectionPoint().y + targetHeightAboveGround;
				std::cout << "JUMPWAITING (applied=" << mJumpApplies << ")" << currPos.y << " > 0.1 + " << minGlobalHeight << "\n";
				if (currVel < -0.01 || currPos.y > (minGlobalHeight + 0.1))
				{
					mJumpStartInProgress = false; // we're going downwards again.
				}
			}
		}
		if (mRay->intersects())
		{
			if (mJumping && !mJumpStartInProgress)
			{
				if (mRay->intersectionDepth() < targetHeightAboveGround)
					mJumping = false;
			}
			if (!mJumping)
			{
				const Vector3 targetPoint = mRay->intersectionPoint()
										+ Vector3::kUnitY * targetHeightAboveGround;

				const real distToTarget = (targetPoint.y - currPos.y);

				//@todo make all these spring and damping stuff configurable...
				const real k = real(1.0);
				const real f1 = k * real(20.) * distToTarget;
				const real f2 = k * real(15.) * currVel;

				f = Vector3::kUnitY * (f1 - f2);
			}
		}
		const Vector3 force =
			(dt == 0.) ? Vector3::kZero : 
				(f + (mActor->getBody().getMass() * (targetVel / dt)));
		mActor->getBody().addForce( 1./*mDynamicsInfluence?*/ * force );
	}

	//-----------------------------------------------------
	void OdeAvatar::onPreStepInternal(const real dt)
	{
		_updateRayData();
		mRay->_clearIntersection();
	}

	//-----------------------------------------------------
	void OdeAvatar::_updateRayData()
	{
		mRay->setPosition( mPosition/*ball position*/ );
	}

	//-----------------------------------------------------
	void OdeAvatar::setDimensions( const Vector3& dim )
    {
		mDimensions = dim;
    }

    //-----------------------------------------------------
	void OdeAvatar::setInfluenceByDynamics( const real ratio )
    {
		mDynamicsInfluence = ratio;
    }

    //-----------------------------------------------------
	void OdeAvatar::setTargetVelocity( const Vector3& vel )
    {
		mTargetVelocity = vel;
    }

    //-----------------------------------------------------
	void OdeAvatar::jump()
    {
		if (mJumping)
			return;
		mDucking = false;
		mJumpApplies = 0;
		mJumpTime = real(0.08);
		mJumping = true;
		mJumpStartInProgress = true;
    }

    //-----------------------------------------------------
	bool OdeAvatar::isJumping() const
    {
		return mJumping;
    }

    //-----------------------------------------------------
	void OdeAvatar::duck(const bool yes)
    {
		mDucking = yes;
    }

    //-----------------------------------------------------
	bool OdeAvatar::isDucking() const
    {
		return mDucking;
    }

	//-----------------------------------------------------
    void OdeAvatar::setPosition( const Vector3& rPosition )
    {
		mPosition = rPosition;
		if (mActor)
			mActor->setPosition( mPosition );
    }

    //-----------------------------------------------------
    void OdeAvatar::setOrientation( const Quaternion& rOrientation )
    {
		mOrientation = rOrientation;
		if (mActor)
			mActor->setOrientation( mOrientation );
    }

    //-----------------------------------------------------
    Vector3 OdeAvatar::getPosition() const
    {
		if (mActor)
			mPosition = mActor->getPosition();
		return mPosition;
    }

    //-----------------------------------------------------
    Quaternion OdeAvatar::getOrientation() const
    {
		if (mActor)
			mOrientation = mActor->getOrientation();
		return mOrientation;
    }

} // physics
} // yake
