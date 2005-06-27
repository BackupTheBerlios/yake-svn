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
	OdeJoint::OdeJoint( OdeWorld* pWorld, JointType type ) : 
			mType( type ),
			mWorld( pWorld )
	{
		YAKE_ASSERT( mWorld ).debug( "Need a world!" );
		
		/// Setting default values
		mSpringConstant = 2.0;
		mDampingConstant = 0.25;
	}
	
	//-----------------------------------------------------
	OdeJoint::~OdeJoint()
	{
		YAKE_SAFE_DELETE( mOdeJoint );
	}
	
	//-----------------------------------------------------
	void OdeJoint::attach( IBody& rBody1, IBody& rBody2 )
	{
		mOdeJoint->attach( 
					( static_cast<OdeBody&>( rBody1 ) )._getOdeBody()->id(),
					( static_cast<OdeBody&>( rBody2 ) )._getOdeBody()->id() );
	}

	//-----------------------------------------------------
	void OdeJoint::setBreakable( bool enabled )
	{
	}
	
	//-----------------------------------------------------
	void OdeJoint::setBreakableForce( real force )
	{
	}
	
	//-----------------------------------------------------
	void OdeJoint::setBreakableTorque( real torque )
	{
	}
	
	//-----------------------------------------------------
	void OdeJoint::setConnectedBodiesCollide( bool enabled )
	{
	}
	
// 		//-----------------------------------------------------
// 		void OdeJoint::setSpring(real spring)
// 		{
// 			mSpringConstant = spring;
// 			_applySpring();
// 		}
// 
// 		//-----------------------------------------------------
// 		void OdeJoint::setDamping(real damping)
// 		{
// 			mDampingConstant = damping;
// 			_applySpring();
// 		}
// 
// 		//-----------------------------------------------------
// 		real OdeJoint::getSpring() const
// 		{
// 			return mSpringConstant;
// 		}
// 
// 		//-----------------------------------------------------
// 		real OdeJoint::getDamping() const
// 		{
// 			return mDampingConstant;
// 		}
// 
	//-----------------------------------------------------
	real OdeJoint::_getCFMFromSpring() const
	{
		real time = mWorld->_getStepSize();
		// calculate CFM from spring constants
		real div = (time * mSpringConstant + mDampingConstant);
		if (div == 0.)
			return 0.;
		return (1.f / div);
	}

	//-----------------------------------------------------
	real OdeJoint::_getERPFromSpring() const
	{
		real time = mWorld->_getStepSize();
		// calculate ERP from spring constants
		real div = time * mSpringConstant + mDampingConstant;
		if ( div == 0 ) 
			return 0.;
		return ( (time * mSpringConstant) / div );
	}

	//-----------------------------------------------------
	void OdeJoint::_applySpring()
	{
		// apply to joint
		switch (mOdeJoint->getType())
		{
		case dJointTypeHinge2:
			dJointSetHinge2Param( mOdeJoint->id(), dParamSuspensionERP, _getERPFromSpring() );
			dJointSetHinge2Param( mOdeJoint->id(), dParamSuspensionCFM, _getCFMFromSpring() );
			break;
		case dJointTypeHinge:
		case dJointTypeSlider:
		case dJointTypeUniversal:
		case dJointTypeBall:
		default:
			break;
		}
	}

} // physics
} // yake
