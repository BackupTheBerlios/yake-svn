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
#ifndef YAKE_ODEJOINT_H
#define YAKE_ODEJOINT_H

namespace yake {
	namespace physics {

		class OdeJointBase : public IJoint
		{
		protected:
			OdeJointBase();
			OdeJointBase( const OdeJointBase & );
			OdeJointBase( IWorld* world, JointType type, IJointGroup* group = 0 );
		public:
			virtual ~OdeJointBase();

			virtual JointType getType() const
			{ return mType; }

			virtual void attach(IBody* body1, IBody* body2);

			virtual void setSpring(real spring);
			virtual void setDamping(real damping);
			virtual real getSpring() const;
			virtual real getDamping() const;

		protected:
			void _applySpring();
			real _getCFMFromSpring() const;
			real _getERPFromSpring() const;
		protected:
			OdeWorld		* mWorld;
			dJoint		* mOdeJoint;
			real			mSpringConstant;
			real			mDampingConstant;
			JointType	mType;
		};

		class OdeHinge2Joint : public OdeJointBase
		{
		public:
			OdeHinge2Joint( IWorld* world, IJointGroup* group = 0 );

			virtual void setAnchor(const Vector3 & anchor);
			virtual Vector3 getAnchor() const;

			virtual void setAxis1(const Vector3 & axis);
			virtual void setAxis2(const Vector3 & axis);
			virtual Vector3 getAxis1() const;
			virtual Vector3 getAxis2() const;

			virtual void setMotor2Velocity(real vel);
			virtual void setMotor2MaximumForce(real force);

			virtual void setLowStop(real stop);
			virtual void setHighStop(real stop);
		};

		class OdeHingeJoint : public OdeJointBase
		{
		public:
			OdeHingeJoint( IWorld* world, IJointGroup* group = 0 );

			virtual void setAnchor(const Vector3 & anchor);
			virtual Vector3 getAnchor() const;

			virtual void setAxis1(const Vector3 & axis);
			virtual void setAxis2(const Vector3 & axis);
			virtual Vector3 getAxis1() const;
			virtual Vector3 getAxis2() const;

			virtual void setMotor2Velocity(real vel);
			virtual void setMotor2MaximumForce(real force);

			virtual void setLowStop(real stop);
			virtual void setHighStop(real stop);
		};

		class OdeBallJoint : public OdeJointBase
		{
		public:
			OdeBallJoint( IWorld* world, IJointGroup* group = 0 );

			virtual void setAnchor(const Vector3 & anchor);
			virtual Vector3 getAnchor() const;

			virtual void setAxis1(const Vector3 & axis);
			virtual void setAxis2(const Vector3 & axis);
			virtual Vector3 getAxis1() const;
			virtual Vector3 getAxis2() const;

			virtual void setMotor2Velocity(real vel);
			virtual void setMotor2MaximumForce(real force);

			virtual void setLowStop(real stop);
			virtual void setHighStop(real stop);
		};

		class OdeFixedJoint : public OdeJointBase
		{
		public:
			OdeFixedJoint( IWorld* world, IJointGroup* group = 0 );

			virtual void attach(IBody* body1, IBody* body2);

			virtual void setAnchor(const Vector3 & anchor);
			virtual Vector3 getAnchor() const;

			virtual void setAxis1(const Vector3 & axis);
			virtual void setAxis2(const Vector3 & axis);
			virtual Vector3 getAxis1() const;
			virtual Vector3 getAxis2() const;

			virtual void setMotor2Velocity(real vel);
			virtual void setMotor2MaximumForce(real force);

			virtual void setLowStop(real stop);
			virtual void setHighStop(real stop);
		};

	}
}

#endif