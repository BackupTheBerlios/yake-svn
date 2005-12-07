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
#ifndef ODE_AVATAR_H
#define ODE_AVATAR_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>

namespace yake
{	
namespace physics
{
	class OdeWorld;
	class OdeGeom;
	class OdeActor;
	class OdeJoint;
	class OdeRay;

	class YAKE_PHYSICSODE_API OdeAvatar : public IAvatar
	{
	public:
		OdeAvatar( OdeWorld* pOdeWorld );
		virtual ~OdeAvatar();

		bool init(const IAvatar::Desc&);

		virtual void setPosition( const Vector3& rPosition );
		virtual void setOrientation( const Quaternion& rOrientation );
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;

		virtual void setDimensions( const Vector3 & rkDimensions );
		virtual void setInfluenceByDynamics( const real ratio );

		virtual void setTargetVelocity( const Vector3 & rkTargetVelocity );
		virtual void jump();
		virtual bool isJumping() const;
		virtual void duck(const bool yes);
		virtual bool isDucking() const;

		YAKE_MEMBERSIGNAL_VIRTUALIMPL(public, void(bool), OnJump)
		YAKE_MEMBERSIGNAL_FIRE_FN1(public, OnJump, bool jumping, jumping)

		YAKE_MEMBERSIGNAL_VIRTUALIMPL(public, void(bool), OnDuck)
		YAKE_MEMBERSIGNAL_FIRE_FN1(public, OnDuck, bool ducking, ducking)

	protected:
	private:
		void onPreStepInternal(const real dt);
		void onPostStepInternal(const real dt);
		void _updateRayData();
	private:
		mutable Vector3					mPosition;
		mutable Quaternion				mOrientation;

		Vector3							mDimensions;
		real							mDynamicsInfluence;
		Vector3							mTargetVelocity;

		bool							mJumping;
		bool							mJumpStartInProgress;
		real							mJumpTime;
		size_t							mJumpApplies;

		bool							mDucking;
		
		OdeActor*						mActor;
		OdeWorld*						mOdeWorld;
		OdeRay*							mRay;
		double							mRayLength;

		double							mSphereRadius;
		Vector3							mSphereOffset;

		real							mHeightAboveGround;
		real							mHeightAboveGroundDuck;

		yake::SignalConnection			mPreStepConn;
		yake::SignalConnection			mPostStepConn;

	};

} // physics
} // yake

#endif
