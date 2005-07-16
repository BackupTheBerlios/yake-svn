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
	OdeBallJoint::OdeBallJoint( OdeWorld* pWorld ) : OdeJoint( pWorld, JT_BALL )
	{
		mOdeJoint = new dBallJoint( mWorld->_getOdeID() );
	
		YAKE_ASSERT( mOdeJoint ).debug( "Failed to create OdeBallJoint!" );
		
		_applySpring();
	}

	//-----------------------------------------------------
	JointType OdeBallJoint::getType() const
	{
		return JT_BALL;
	}
	
	//-----------------------------------------------------
	size_t OdeBallJoint::getNumAxis() const
	{
		return 0;
	}
	
	//-----------------------------------------------------
	void OdeBallJoint::setAxis( size_t axisIndex, Vector3 const& rAxis )
	{
		YAKE_ASSERT( false ).error( "Ball joint doesn't have any axes!" );
	}
	
	//-----------------------------------------------------
	size_t OdeBallJoint::getNumAnchors() const
	{
		return 1;
	}
	
	//-----------------------------------------------------
	void OdeBallJoint::setMotor( size_t axisIndex, real velocityTarget, real maximumForce )
	{
		YAKE_ASSERT( false ).error( "Motor can't be applied to ball joint!" );
	}
	
	//-----------------------------------------------------
	void OdeBallJoint::setMotorEnabled(size_t axisIndex, bool enabled)
	{
		YAKE_ASSERT( false ).error( "Motor can't be applied to ball joint!" );
	}

	//-----------------------------------------------------
	void OdeBallJoint::setAnchor( size_t anchorIndex, Vector3 const& rAnchor )
	{
		YAKE_ASSERT( anchorIndex < 1 ).error( "Ball joint has only one anchor! You are trying to set more..." );
		
		dJointSetBallAnchor( mOdeJoint->id(), rAnchor.x, rAnchor.y, rAnchor.z );
	}
	
	//-----------------------------------------------------
	void OdeBallJoint::setLimits( size_t axisIndex, real low, real high )
	{
		YAKE_ASSERT( false ).error( "Ball joint doesn't have any axes! No axes - no limits ;)" );
	}

} // physics
} // yake
