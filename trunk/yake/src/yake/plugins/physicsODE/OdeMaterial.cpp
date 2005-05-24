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
#include <yake/plugins/physicsODE/OdeMaterial.h>

namespace yake {
namespace physics {

	//---------------------------------------------------
	OdeMaterial::OdeMaterial() : 
		mSoftness(0.5),
		mFriction(1),
		mFriction2(0),
		mLateralSlip(false),
		mSlipNormal(Vector3::kZero),
		mSlipLinearCoeff(0),
		mSlipAngularCoeff(0)
	{
	}
	void OdeMaterial::setBounciness(const real bounciness)
	{
		//@todo
	}
	void OdeMaterial::setSoftness(const real softness)
	{
		mSoftness = softness;
	}
	void OdeMaterial::setRollingFriction( const real friction )
	{
		//@todo
	}
	void OdeMaterial::setRestitution( const real restitution )
	{
		//@todo
	}
	void OdeMaterial::setStaticFriction( const real friction )
	{
		//@todo
	}
	void OdeMaterial::setStaticFrictionV( const real friction )
	{
		//@todo
	}
	void OdeMaterial::setStaticFrictionVEnabled( bool enabled )
	{
		//@todo
	}

} // physics
} // yake
