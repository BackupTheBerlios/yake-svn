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
	OdeHingeJoint::OdeHingeJoint( OdeWorld* pWorld ) : OdeJoint( pWorld, JT_HINGE )
	{
		mOdeJoint = new dHingeJoint( mWorld->_getOdeID() );
		
		YAKE_ASSERT( mOdeJoint != NULL ).debug( "Failed to create OdeHingeJoint!" );
		
		_applySpring();
	}
	
	//-----------------------------------------------------
	JointType OdeHingeJoint::getType() const
	{
		return JT_HINGE;
	}
	
	//-----------------------------------------------------
	size_t OdeHingeJoint::getNumAxis() const
	{
		return 1;
	}
	
	//-----------------------------------------------------
	void OdeHingeJoint::setAxis( size_t axisIndex, Vector3 const& rAxis )
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Hinge joint has only one axis! You are trying to set more..." );
		
		dJointSetHingeAxis( mOdeJoint->id(), rAxis.x, rAxis.y, rAxis.z );
	}
		
	//-----------------------------------------------------
	size_t OdeHingeJoint::getNumAnchors() const
	{
		return 1;
	}
	
	//-----------------------------------------------------
	void OdeHingeJoint::setAnchor( size_t anchorIndex, Vector3 const& rAnchor )
	{
		YAKE_ASSERT( anchorIndex < 1 ).error( "Hinge joint has only one anchor! You are trying to set more..." );
		
		dJointSetHingeAnchor( mOdeJoint->id(), rAnchor.x, rAnchor.y, rAnchor.z );
	}
	
	//-----------------------------------------------------
	void OdeHingeJoint::setMotor( size_t axisIndex, real targetVelocity, real maximumForce )
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Hinge joint has only one axis! You are trying to set more..." );
		if (axisIndex >= 1)
			return;
		mVelTarget0 = targetVelocity;
		mMaxForce0 = maximumForce;
	}
	
	//-----------------------------------------------------
	void OdeHingeJoint::setMotorEnabled(size_t axisIndex, bool enabled)
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Hinge joint has only one axis! You are trying to set more..." );
		if (enabled)
		{
			static_cast<dHingeJoint*>(mOdeJoint)->setParam( dParamVel, mVelTarget0 );
			static_cast<dHingeJoint*>(mOdeJoint)->setParam( dParamVel, mMaxForce0 );
		}
		else
		{
			static_cast<dHingeJoint*>(mOdeJoint)->setParam( dParamVel, 0 );
			static_cast<dHingeJoint*>(mOdeJoint)->setParam( dParamVel, 0 );
		}
	}
	
	//-----------------------------------------------------
	void OdeHingeJoint::setLimits( size_t axisIndex, real low, real high )
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Hinge joint has only one axis! You are trying to use more..." );
		
		/// TODO What about dParamBounce, dParamFudgeFactor and other stop params?
		/// Maybe to set 'em to defaults?
		dJointSetHingeParam( mOdeJoint->id(), dParamLoStop, low );
		dJointSetHingeParam( mOdeJoint->id(), dParamHiStop, high );
	}

} // physics
} // yake
