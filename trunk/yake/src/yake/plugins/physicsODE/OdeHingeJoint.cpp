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
#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeJoint.h>
#include <yake/plugins/physicsODE/OdeBody.h>

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		OdeHingeJoint::OdeHingeJoint(IWorld* world, IJointGroup *group) : OdeJointBase( world, JT_HINGE, group )
		{
			mOdeJoint = new dHingeJoint( mWorld->_getOdeID() );
			YAKE_ASSERT( mOdeJoint ).debug("Out of memory ?");
			_applySpring();
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setAnchor(const Vector3 & anchor)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHingeAnchor( mOdeJoint->id(), anchor.x, anchor.y, anchor.z );
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setAxis1(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHingeAxis( mOdeJoint->id(), axis.x, axis.y, axis.z );
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setAxis2(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).warning("A hinge joint has no 2nd axis!");
		}

		//-----------------------------------------------------
		Vector3 OdeHingeJoint::getAnchor() const
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dVector3 anchor;
			static_cast<dHingeJoint*>(mOdeJoint)->getAnchor( anchor );
			return Vector3( anchor[0], anchor[1], anchor[2] );
		}

		//-----------------------------------------------------
		Vector3 OdeHingeJoint::getAxis1() const
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dVector3 axis;
			static_cast<dHingeJoint*>(mOdeJoint)->getAxis( axis );
			return Vector3( axis[0], axis[1], axis[2] );
		}

		//-----------------------------------------------------
		Vector3 OdeHingeJoint::getAxis2() const
		{
			YAKE_ASSERT( mOdeJoint ).warning("A hinge joint has no 2nd axis!");
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setMotor2Velocity(float vel)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHingeParam( mOdeJoint->id(), dParamVel2, vel );
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setMotor2MaximumForce(float force)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHingeParam( mOdeJoint->id(), dParamFMax2, force );
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setLowStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHingeParam( mOdeJoint->id(), dParamLoStop, stop );
		}

		//-----------------------------------------------------
		void OdeHingeJoint::setHighStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Need an ODE joint to work with!");
			dJointSetHingeParam( mOdeJoint->id(), dParamHiStop, stop );
		}
	}
}