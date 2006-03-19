/*
   ------------------------------------------------------------------------------------
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
	OdeUniversalJoint::OdeUniversalJoint( OdeWorld* pWorld ) : OdeJoint( pWorld, JT_HINGE2 )
	{
		mOdeJoint = new dUniversalJoint( mWorld->_getOdeID() );
		
		YAKE_ASSERT( mOdeJoint ).debug( "Failed to create OdeUniversalJoint!" );
		
		_applySpring();
	}

	//-----------------------------------------------------
	JointType OdeUniversalJoint::getType() const
	{
		return JT_UNIVERSAL;
	}
		
	//-----------------------------------------------------
	size_t OdeUniversalJoint::getNumAxis() const
	{
		return 2;
	}
	
	//-----------------------------------------------------
	void OdeUniversalJoint::setAxis( size_t axisIndex, math::Vector3 const& rAxis )
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Universal joint has only two axes! You are trying to set more..." );
		
		if ( axisIndex == 0 )
			dJointSetUniversalAxis1( mOdeJoint->id(), rAxis.x, rAxis.y, rAxis.z );
		else if ( axisIndex == 1 )
			dJointSetUniversalAxis2( mOdeJoint->id(), rAxis.x, rAxis.y, rAxis.z );
	}
	
	//-----------------------------------------------------
	size_t OdeUniversalJoint::getNumAnchors() const
	{
		return 1;
	}
	
	//-----------------------------------------------------
	void OdeUniversalJoint::setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor )
	{
		YAKE_ASSERT( anchorIndex < 1 ).error( "Universal joint has only one anchor! You are trying to set more..." );
		
		dJointSetUniversalAnchor( mOdeJoint->id(), rAnchor.x, rAnchor.y, rAnchor.z );
	}
	
	//-----------------------------------------------------
	void OdeUniversalJoint::setMotor( size_t axisIndex, real targetVelocity, real maximumForce )
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Universal joint has only two axes! You are trying to use more..." );
		
		mVelTarget[axisIndex] = targetVelocity;
		mMaxForce[axisIndex] = maximumForce;
	}
	
	//-----------------------------------------------------
	void OdeUniversalJoint::setMotorEnabled(size_t axisIndex, bool enabled)
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Universal joint has only two axes! You are trying to use more..." );
		if (enabled)
		{
			if (axisIndex == 0)
			{
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel, mVelTarget[0] );
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel, mMaxForce[0] );
			}
			else if (axisIndex == 1)
			{
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel2, mVelTarget[1] );
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel2, mMaxForce[1] );
			}
		}
		else
		{
			if (axisIndex == 0)
			{
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel, 0 );
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel, 0 );
			}
			else if (axisIndex == 1)
			{
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel2, 0 );
				static_cast<dUniversalJoint*>(mOdeJoint)->setParam( dParamVel2, 0 );
			}
		}
	}

	//-----------------------------------------------------
	void OdeUniversalJoint::setLimits( size_t axisIndex, real low, real high )
	{
		YAKE_ASSERT( axisIndex < 2 ).error( "Universal joint has only two axes! You are trying to use more..." );
	
		/// TODO What about dParamBounce, dParamFudgeFactor and other stop params?
		/// Maybe to set 'em to defaults?
		if ( axisIndex == 0 )
		{
			dJointSetUniversalParam( mOdeJoint->id(), dParamLoStop, low );
			dJointSetUniversalParam( mOdeJoint->id(), dParamHiStop, high );
		}
		else if ( axisIndex == 1 )
		{
			dJointSetUniversalParam( mOdeJoint->id(), dParamLoStop2, low );
			dJointSetUniversalParam( mOdeJoint->id(), dParamHiStop2, high );
		}
	}

} // physics
} // yake

