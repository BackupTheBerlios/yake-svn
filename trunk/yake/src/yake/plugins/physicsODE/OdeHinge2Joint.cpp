/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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

namespace yake {
namespace physics {

	//-----------------------------------------------------
	OdeHinge2Joint::OdeHinge2Joint( OdeWorld* pWorld ) : OdeJoint( pWorld, JT_HINGE2 )
	{
		mOdeJoint = new dHinge2Joint( mWorld->_getOdeID() );
		
		YAKE_ASSERT( mOdeJoint ).debug( "Failed to create OdeHinge2Joint!" );
		
		_applySpring();
	}

	//-----------------------------------------------------
	JointType OdeHinge2Joint::getType() const
	{
		return JT_HINGE2;
	}
		
	//-----------------------------------------------------
	size_t OdeHinge2Joint::getNumAxis() const
	{
		return 2;
	}
	
	//-----------------------------------------------------
	void OdeHinge2Joint::setAxis( size_t axisIndex, const math::Vector3& rAxis )
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Hinge2 joint has only two axes! You are trying to set more..." );
		
		if ( axisIndex == 0 )
			dJointSetHinge2Axis1( mOdeJoint->id(), rAxis.x, rAxis.y, rAxis.z );
		else if ( axisIndex == 1 )
			dJointSetHinge2Axis2( mOdeJoint->id(), rAxis.x, rAxis.y, rAxis.z );
	}
	
	//-----------------------------------------------------
	size_t OdeHinge2Joint::getNumAnchors() const
	{
		return 1;
	}
	
	//-----------------------------------------------------
	void OdeHinge2Joint::setAnchor( size_t anchorIndex, const math::Vector3& rAnchor )
	{
		YAKE_ASSERT( anchorIndex < 1 ).error( "Hinge2 joint has only one anchor! You are trying to set more..." );
		
		dJointSetHinge2Anchor( mOdeJoint->id(), rAnchor.x, rAnchor.y, rAnchor.z );
	}
	
	//-----------------------------------------------------
	void OdeHinge2Joint::setMotor( size_t axisIndex, real targetVelocity, real maximumForce )
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Hinge2 joint has only two axes! You are trying to use more..." );
		
		mVelTarget[axisIndex] = targetVelocity;
		mMaxForce[axisIndex] = maximumForce;
	}
	
	//-----------------------------------------------------
	void OdeHinge2Joint::setMotorEnabled(size_t axisIndex, bool enabled)
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Hinge2 joint has only two axes! You are trying to use more..." );
		if (enabled)
		{
			if (axisIndex == 0)
			{
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamVel, mVelTarget[0] );
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamFMax, mMaxForce[0] );
			}
			else if (axisIndex == 1)
			{
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamVel2, mVelTarget[1] );
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamFMax2, mMaxForce[1] );
			}
		}
		else
		{
			if (axisIndex == 0)
			{
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamVel, 0 );
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamFMax, 0 );
			}
			else if (axisIndex == 1)
			{
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamVel2, 0 );
				static_cast<dHinge2Joint*>(mOdeJoint)->setParam( dParamFMax2, 0 );
			}
		}
	}
	
	//-----------------------------------------------------
	void OdeHinge2Joint::setLimits( size_t axisIndex, real low, real high )
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Hinge2 joint has only two axes! You are trying to use more..." );
	
		/// TODO What about dParamBounce, dParamFudgeFactor and other stop params?
		/// Maybe to set 'em to defaults?
		if ( axisIndex == 0 )
		{
			dJointSetHinge2Param( mOdeJoint->id(), dParamLoStop, low );
			dJointSetHinge2Param( mOdeJoint->id(), dParamHiStop, high );
		}
		else if ( axisIndex == 1 )
		{
			dJointSetHinge2Param( mOdeJoint->id(), dParamLoStop2, low );
			dJointSetHinge2Param( mOdeJoint->id(), dParamHiStop2, high );
		}
	}

} // physics
} // yake
