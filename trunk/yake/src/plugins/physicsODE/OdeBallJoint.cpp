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
#include <plugins/yakePhysicsODE/inc/pch.h>
#include <plugins/yakePhysicsODE/inc/OdeWorld.h>
#include <plugins/yakePhysicsODE/inc/OdeJoint.h>
#include <plugins/yakePhysicsODE/inc/OdeBody.h>

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		OdeBallJoint::OdeBallJoint(IWorld* world, IJointGroup *group) : OdeJointBase( world, JT_BALL, group )
		{
			mOdeJoint = new dBallJoint( mWorld->_getOdeID() );
			YAKE_ASSERT( mOdeJoint ).debug("Out of memory ?");
			_applySpring();
		}

		//-----------------------------------------------------
		void OdeBallJoint::setAnchor(const Vector3 & anchor)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetBallAnchor( mOdeJoint->id(), anchor.x, anchor.y, anchor.z );
		}

		//-----------------------------------------------------
		void OdeBallJoint::setAxis1(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).debug("A ball joint doesn't have any axis!");
		}

		//-----------------------------------------------------
		void OdeBallJoint::setAxis2(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).debug("A ball joint doesn't have any axis!");
		}

		//-----------------------------------------------------
		Vector3 OdeBallJoint::getAnchor() const
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dVector3 anchor;
			static_cast<dBallJoint*>(mOdeJoint)->getAnchor( anchor );
			return Vector3( anchor[0], anchor[1], anchor[2] );
		}

		//-----------------------------------------------------
		Vector3 OdeBallJoint::getAxis1() const
		{
			YAKE_ASSERT( mOdeJoint ).warning("A ball joint doesn't have any axis!");
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		Vector3 OdeBallJoint::getAxis2() const
		{
			YAKE_ASSERT( mOdeJoint ).warning("A ball joint doesn't have any axis!");
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		void OdeBallJoint::setMotor2Velocity(float vel)
		{
			YAKE_ASSERT( mOdeJoint ).debug("A ball joint doesn't have any motors!");
		}

		//-----------------------------------------------------
		void OdeBallJoint::setMotor2MaximumForce(float force)
		{
			YAKE_ASSERT( mOdeJoint ).debug("A ball joint doesn't have any motors!");
		}

		//-----------------------------------------------------
		void OdeBallJoint::setLowStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a ball joint!");
		}

		//-----------------------------------------------------
		void OdeBallJoint::setHighStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a ball joint!");
		}
	}
}