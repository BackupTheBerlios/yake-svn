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
	OdeSliderJoint::OdeSliderJoint( OdeWorld* pWorld ) : OdeJoint( pWorld, JT_SLIDER )
	{
		mOdeJoint = new dSliderJoint( mWorld->_getOdeID() );
		
		YAKE_ASSERT( mOdeJoint ).debug( "Failed to create OdeSliderJoint!" );
		
		_applySpring();
	}

	//-----------------------------------------------------
	JointType OdeSliderJoint::getType() const
	{
		return JT_SLIDER;
	}
	
	//-----------------------------------------------------
	size_t OdeSliderJoint::getNumAxis() const
	{
		return 1;
	}
	
	//-----------------------------------------------------
	void OdeSliderJoint::setAxis( size_t axisIndex, const math::Vector3& rAxis )
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Slider joint has only one axis! You are trying to set more..." );
		
		dJointSetSliderAxis( mOdeJoint->id(), rAxis.x, rAxis.y, rAxis.z );
	}
	
	//-----------------------------------------------------
	size_t OdeSliderJoint::getNumAnchors() const
	{
		return 0;
	}
	
	//-----------------------------------------------------
	void OdeSliderJoint::setAnchor( size_t anchorIndex, const math::Vector3& rAnchor )
	{
		YAKE_ASSERT( false ).error( "Slider joint doesn't have any anchors!" );
	}
	
	//-----------------------------------------------------
	void OdeSliderJoint::setMotor( size_t axisIndex, real targetVelocity, real maximumForce )
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Slider joint has only one axis! You are trying to set more..." );
		if (axisIndex >= 1)
			return;
		mVelTarget0 = targetVelocity;
		mMaxForce0 = maximumForce;
	}
	
	//-----------------------------------------------------
	void OdeSliderJoint::setMotorEnabled(size_t axisIndex, bool enabled)
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Slider joint has only one axis! You are trying to set more..." );
		if (enabled)
		{
			static_cast<dSliderJoint*>(mOdeJoint)->setParam( dParamVel, mVelTarget0 );
			static_cast<dSliderJoint*>(mOdeJoint)->setParam( dParamVel, mMaxForce0 );
		}
		else
		{
			static_cast<dSliderJoint*>(mOdeJoint)->setParam( dParamVel, 0 );
			static_cast<dSliderJoint*>(mOdeJoint)->setParam( dParamVel, 0 );
		}
	}

	//-----------------------------------------------------
	void OdeSliderJoint::setLimits( size_t axisIndex, real low, real high )
	{
		YAKE_ASSERT( axisIndex < 1 ).error( "Slider joint has only one axis! You are trying to set more..." );
		
		///@todo What about dParamBounce, dParamFudgeFactor and other stop params?
		/// Maybe to set 'em to defaults?
		dJointSetSliderParam( mOdeJoint->id(), dParamLoStop, low );
		dJointSetSliderParam( mOdeJoint->id(), dParamHiStop, high );
	}

} // physics
} // yake
