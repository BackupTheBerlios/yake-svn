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
		OdeHinge2Joint::OdeHinge2Joint(IWorld* world, IJointGroup *group) : OdeJointBase( world, JT_HINGE2, group )
		{
			mOdeJoint = new dHinge2Joint( mWorld->_getOdeID() );
			YAKE_ASSERT( mOdeJoint ).debug("Out of memory ?");
			_applySpring();
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setAnchor(const Vector3 & anchor)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Anchor( mOdeJoint->id(), anchor.x, anchor.y, anchor.z );
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setAxis1(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Axis1( mOdeJoint->id(), axis.x, axis.y, axis.z );
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setAxis2(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Axis2( mOdeJoint->id(), axis.x, axis.y, axis.z );
		}

		//-----------------------------------------------------
		Vector3 OdeHinge2Joint::getAnchor() const
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dVector3 anchor;
			static_cast<dHinge2Joint*>(mOdeJoint)->getAnchor( anchor );
			return Vector3( anchor[0], anchor[1], anchor[2] );
		}

		//-----------------------------------------------------
		Vector3 OdeHinge2Joint::getAxis1() const
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dVector3 axis;
			static_cast<dHinge2Joint*>(mOdeJoint)->getAxis1( axis );
			return Vector3( axis[0], axis[1], axis[2] );
		}

		//-----------------------------------------------------
		Vector3 OdeHinge2Joint::getAxis2() const
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dVector3 axis;
			static_cast<dHinge2Joint*>(mOdeJoint)->getAxis2( axis );
			return Vector3( axis[0], axis[1], axis[2] );
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setMotor2Velocity(float vel)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Param( mOdeJoint->id(), dParamVel2, vel );
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setMotor2MaximumForce(float force)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Param( mOdeJoint->id(), dParamFMax2, force );
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setLowStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Param( mOdeJoint->id(), dParamLoStop, stop );
		}

		//-----------------------------------------------------
		void OdeHinge2Joint::setHighStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHinge2Param( mOdeJoint->id(), dParamHiStop, stop );
		}

	}
}