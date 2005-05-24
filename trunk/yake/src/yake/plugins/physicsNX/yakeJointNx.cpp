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
#include <yake/plugins/physicsNX/yakeJointNx.h>
#include <yake/plugins/physicsNX/yakeActorNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	JointNx::JointNx( NxScene & rScene ) :
		mNxScene( rScene ),
		mpNxJoint( 0 ),
		mBreakable( false ),
		mBreakingForce( 0 ),
		mBreakingTorque( 0 ),
		mType(JT_OTHER)
	{
		mpActors[0] = 0;
		mpActors[1] = 0;
	}
	JointNx::~JointNx()
	{
		if (mpNxJoint)
		{
			mNxScene.releaseJoint( *mpNxJoint );
			mpNxJoint = 0;
		}
		mType = JT_OTHER;
	}
	const Deque<JointType>& JointNx::getSupportedTypes_()
	{
		static Deque<JointType> jointTypes;
		if (jointTypes.empty())
		{
			jointTypes.push_back( JT_FIXED );
		}
		return jointTypes;
	}
	JointNx* JointNx::createFromDesc_( NxScene & rScene, const IJoint::DescBase & rkJointDesc )
	{
		//@todo handle case for "actor <-> static environment"
		//@todo in debug: dynamic cast, in release: static cast
		if (!rkJointDesc.actor0 || !rkJointDesc.actor1)
			return 0;
		ActorNx & rActor1 = dynamic_cast<ActorNx&>(*rkJointDesc.actor0);
		ActorNx & rActor2 = dynamic_cast<ActorNx&>(*rkJointDesc.actor1);

		const Deque<JointType>& jointTypes = getSupportedTypes_();
		Deque<JointType>::const_iterator itFind = std::find(jointTypes.begin(), jointTypes.end(), rkJointDesc.type );
		YAKE_ASSERT(jointTypes.end() != itFind);
		if (jointTypes.end() == itFind)
			return 0; // unsupported type

		JointNx* pJoint = new JointNx(rScene);
		YAKE_ASSERT( pJoint );
		if (!pJoint)
			return 0;

		switch( rkJointDesc.type )
		{
		case JT_FIXED:
			if (!pJoint->createAsFixed( static_cast<const IJoint::DescFixed&>( rkJointDesc ), rActor1, rActor2 ))
			{
				delete pJoint;
				return false;
			}
			break;
		default:
			YAKE_ASSERT(1==0);
			return false;
		}

		return pJoint;
	}
	bool JointNx::createAsFixed( const IJoint::DescFixed& rkDesc, ActorNx & rActor1, ActorNx & rActor2 )
	{
		YAKE_ASSERT( mpNxJoint == 0 );
		if (mpNxJoint)
			return false;

		NxPointOnLineJointDesc desc;
		//NxPrismaticJointDesc desc;
		desc.setToDefault();
		desc.actor[0] = rActor1.getNxActorPtr_();
		desc.actor[1] = rActor2.getNxActorPtr_();

		//@todo do we really have to fill all of these options in ?
		for (int i=0; i<2; ++i)
		{
			const int iOne = i;
			const int iTwo = (i == 0) ? 1 : 0;

			/*Vector3 one2two = rActor2.getPosition() - rActor1.getPosition();
			const real distance = one2two.length();
			Vector3 one2two_n = one2two.normalisedCopy();

			const Vector3 perp = one2two_n.perpendicular().normalisedCopy();

			// "X axis of joint space, in actor[i]'s space, orthogonal to localAxis[i]."
			desc.localNormal[i] = toNx( one2two_n.crossProduct( perp ) );

			// "Z axis of joint space, in actor[i]'s space. This is the primary axis of the joint."
			desc.localAxis[i] = toNx( one2two_n );

			// "Attachment point of joint in actor[i]'s space."
			desc.localAnchor[i] = toNx( rActor1.getPosition() + one2two_n * distance * 0.5 );*/

		}
		//ignored:
		//desc.method
		//desc.maxForce
		//desc.maxTorque
		//desc.userData
		//desc.name -> debug string
		desc.jointFlags |= NX_JF_COLLISION_ENABLED;
		desc.setGlobalAxis( toNx( (rActor2.getPosition() - rActor1.getPosition()).normalisedCopy() ) );

		mpNxJoint = mNxScene.createJoint( desc );
		YAKE_ASSERT( mpNxJoint );
		if (!mpNxJoint)
			return false;

		mType = rkDesc.type;

		return true;
	}
	JointType JointNx::getType() const
	{
		return mType;
	}
	size_t JointNx::getNumAxis() const
	{
		return 1;
	}
	void JointNx::setAxis(size_t axisIndex, const Vector3 & rkAxis)
	{
		YAKE_ASSERT( axisIndex == 0 );
		YAKE_ASSERT( mpNxJoint );
		mpNxJoint->setGlobalAxis( toNx( rkAxis ) );
	}
	size_t JointNx::getNumAnchors() const
	{
		return 1;
	}
	void JointNx::setAnchor(size_t anchorIndex, const Vector3 & rkAnchor)
	{
		YAKE_ASSERT( anchorIndex == 0 );
		YAKE_ASSERT( mpNxJoint );
		mpNxJoint->setGlobalAnchor( toNx(rkAnchor) );
	}
	void JointNx::setMotor(size_t axisIndex, real velocityTarget, real maximumForce)
	{
		YAKE_ASSERT( false );
	}
	void JointNx::setMotorEnabled(size_t axisIndex, bool enabled)
	{
		YAKE_ASSERT( false );
	}
	void JointNx::setLimits(size_t axisIndex, real low, real high)
	{
		YAKE_ASSERT( false );
	}
	void JointNx::setBreakable(bool enabled)
	{
		YAKE_ASSERT( mpNxJoint );
		mBreakable = enabled;
		if (mBreakable)
			mpNxJoint->setBreakable( toNx(mBreakingForce), toNx(mBreakingTorque) );
		else
			mpNxJoint->setBreakable( NX_MAX_REAL, NX_MAX_REAL );
	}
	void JointNx::setBreakableForce(real force)
	{
		mBreakingForce = force;
		if (mBreakable)
			mpNxJoint->setBreakable( toNx(mBreakingForce), toNx(mBreakingTorque) );
	}
	void JointNx::setBreakableTorque(real torque)
	{
		mBreakingTorque = torque;
		if (mBreakable)
			mpNxJoint->setBreakable( toNx(mBreakingForce), toNx(mBreakingTorque) );
	}
	void JointNx::setConnectedBodiesCollide(bool enabled)
	{
	}
	IActor& JointNx::getAttachedActor(bool bFirst)
	{
		const size_t index = bFirst ? 0 : 1;
		YAKE_ASSERT( mpActors[ index ] );
		return *mpActors[ index ];
	}

}
}