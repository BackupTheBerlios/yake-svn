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
#ifndef ODE_MATERIAL_H
#define ODE_MATERIAL_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>

namespace yake
{
namespace physics
{
	class YAKE_PHYSICSODE_API OdeMaterial : public IMaterial
	{
	public:
		OdeMaterial(const physics::IMaterial::Desc& desc, const String& name);
		OdeMaterial(const String& name);
		virtual void setBounciness(const real bounciness);
		virtual void setSoftness(const real softness);
		virtual void setRollingFriction( const real friction );
		virtual void setRestitution( const real restitution );
		virtual void setStaticFriction( const real friction );
		virtual void setStaticFrictionV( const real friction );
		virtual void setStaticFrictionVEnabled( bool enabled );

		real					mSoftness;
		real					mFriction;
		real					mRestitution;
		real					mFriction2;
		bool					mLateralSlip;
//		ISlipNormalSource*		mSlipNormalSource;
		Vector3					mSlipNormal;
		real					mSlipLinearCoeff;
		real					mSlipAngularCoeff;

		String					mName;
	};

} // physics
} // yake

#endif
