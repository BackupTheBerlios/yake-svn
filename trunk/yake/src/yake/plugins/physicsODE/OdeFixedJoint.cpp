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
		OdeFixedJoint::OdeFixedJoint(IWorld* world, IJointGroup *group) : OdeJointBase( world, JT_FIXED, group )
		{
			mOdeJoint = new dFixedJoint( mWorld->_getOdeID() );
			YAKE_ASSERT( mOdeJoint ).debug("Out of memory ?");
			_applySpring();
		}

		//-----------------------------------------------------
		void OdeFixedJoint::attach(IBody* body1, IBody* body2)
		{
			YAKE_ASSERT( body1 && body2 )(body1)(body2).debug("Need two valid bodies!");
			if (!body1 || !body2)
				return;
			OdeJointBase::attach( body1, body2 );
			dJointSetFixed( mOdeJoint->id() );
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setAnchor(const Vector3 & anchor)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a fixed joint!");
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setAxis1(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a fixed joint!");
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setAxis2(const Vector3 & axis)
		{
			YAKE_ASSERT( mOdeJoint ).warning("A fixed joint doesn't have any axis!");
		}

		//-----------------------------------------------------
		Vector3 OdeFixedJoint::getAnchor() const
		{
			YAKE_ASSERT( mOdeJoint ).warning("A fixed joint doesn't have an anchor point!");
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		Vector3 OdeFixedJoint::getAxis1() const
		{
			YAKE_ASSERT( mOdeJoint ).warning("A fixed joint doesn't have any axis!");
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		Vector3 OdeFixedJoint::getAxis2() const
		{
			YAKE_ASSERT( mOdeJoint ).warning("A fixed joint doesn't have any axis!");
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setMotor2Velocity(float vel)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a fixed joint!");
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setMotor2MaximumForce(float force)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a fixed joint!");
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setLowStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a fixed joint!");
		}

		//-----------------------------------------------------
		void OdeFixedJoint::setHighStop(float stop)
		{
			YAKE_ASSERT( mOdeJoint ).debug("Not valid for a fixed joint!");
		}
	}
}