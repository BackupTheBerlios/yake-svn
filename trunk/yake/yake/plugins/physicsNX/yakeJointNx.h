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
#ifndef YAKE_PHYSICS_JOINTNX_H
#define YAKE_PHYSICS_JOINTNX_H

namespace yake {
namespace physics {

	class ActorNx;
	class JointNx : public IJoint
	{
	public:
		JointNx( NxScene & rScene );
		virtual ~JointNx();

		virtual IActor& getAttachedActor(bool bFirst);

		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, OnBreaking )
		YAKE_MEMBERSIGNAL_FIRE_FN0( public, OnBreaking )

		virtual JointType getType() const;
		virtual size_t getNumAxis() const;
		virtual void setAxis(size_t axisIndex, const Vector3 & rkAxis);
		virtual size_t getNumAnchors() const;
		virtual void setAnchor(size_t anchorIndex, const Vector3 & rkAnchor);
		virtual void setMotor(size_t axisIndex, real velocityTarget, real maximumForce);
		virtual void setMotorEnabled(size_t axisIndex, bool enabled);
		virtual void setLimits(size_t axisIndex, real low, real high);
		virtual void setBreakable(bool enabled);
		virtual void setBreakableForce(real force);
		virtual void setBreakableTorque(real torque);
		virtual void setConnectedBodiesCollide(bool enabled);
		//virtual void addForce(const size_t axisIndex, const real force, const real duration = real(-1));
		//virtual void addTorque(const size_t axisIndex, const real torque, const real duration = real(-1));
		//
		static JointNx* createFromDesc_( NxScene & rScene, const IJoint::DescBase & rkJointDesc );
		static const Deque<JointType>& getSupportedTypes_();
		static const AssocVector<NxJointType, JointType>& getNxJointMap();
	private:
		bool createAsFixed( const IJoint::DescFixed& rkDesc, ActorNx & rActor1, ActorNx & rActor2 );
	private:
		NxScene&		mNxScene;
		NxJoint*		mpNxJoint;
		JointType		mType;
		ActorNx*		mpActors[2];
		real			mBreakingForce;
		real			mBreakingTorque;
		bool			mBreakable;
	};

}
}

#endif