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
		OdeJointBase::OdeJointBase(IWorld* world, JointType type, IJointGroup *group) : mType( type ), mWorld( static_cast<OdeWorld*>(world) )
		{
			YAKE_ASSERT( mWorld ).debug("Need a world!");
			mSpringConstant = 2.0; // defaults
			mDampingConstant = 0.25;
		}

		//-----------------------------------------------------
		OdeJointBase::~OdeJointBase()
		{
			YAKE_SAFE_DELETE( mOdeJoint );
		}
		//-----------------------------------------------------
		void OdeJointBase::attach(IBody* body1, IBody* body2)
		{
			YAKE_ASSERT( body1 && body2 )(body1)(body2).debug("Need two valid bodies!");
			if (!body1 || !body2)
				return;

			mOdeJoint->attach( (static_cast<OdeBody*>(body1))->_getOdeBody()->id(),
									(static_cast<OdeBody*>(body2))->_getOdeBody()->id() );
		}

		//-----------------------------------------------------
		void OdeJointBase::setSpring(real spring)
		{
			mSpringConstant = spring;
			_applySpring();
		}

		//-----------------------------------------------------
		void OdeJointBase::setDamping(real damping)
		{
			mDampingConstant = damping;
			_applySpring();
		}

		//-----------------------------------------------------
		real OdeJointBase::getSpring() const
		{
			return mSpringConstant;
		}

		//-----------------------------------------------------
		real OdeJointBase::getDamping() const
		{
			return mDampingConstant;
		}

		//-----------------------------------------------------
		real OdeJointBase::_getCFMFromSpring() const
		{
			real time = mWorld->_getStepSize();
			// calculate CFM from spring constants
			real div = (time * mSpringConstant + mDampingConstant);
			if (div == 0.)
				return 0.;
			return (1.f / div);
		}

		//-----------------------------------------------------
		real OdeJointBase::_getERPFromSpring() const
		{
			real time = mWorld->_getStepSize();
			// calculate ERP from spring constants
			real div = time * mSpringConstant + mDampingConstant;
			if (div == 0) 
				return 0.;
			return ( (time * mSpringConstant) / div );
		}

		//-----------------------------------------------------
		void OdeJointBase::_applySpring()
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


	}
}