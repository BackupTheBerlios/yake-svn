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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakeMaterialNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	MaterialNx::MaterialNx( const NxMaterialIndex index ) :
		mNxMatIndex( index )
	{
	}
	MaterialNx::MaterialNx() :
		mNxMatIndex( 0 )
	{
	}
	MaterialNx::~MaterialNx()
	{
		//@todo
	}
	void MaterialNx::_createFromDesc( const IMaterial::Desc & rDesc )
	{
		NxMaterial nxMat;
		nxMat.setToDefault();
		nxMat.restitution = toNx( rDesc.mRestitution );
		nxMat.staticFriction = toNx( rDesc.mStaticFriction );
		nxMat.staticFrictionV = 0;
		YAKE_ASSERT( nxMat.isValid() );
		mNxMatIndex = getNxSDK()->addMaterial( nxMat );
	}
	void MaterialNx::setBounciness(const real bounciness)
	{
	}
	void MaterialNx::setSoftness(const real softness)
	{
	}
	void MaterialNx::setRollingFriction(const real friction)
	{
	}
	void MaterialNx::setRestitution(const real restitution)
	{
		NxMaterial* pNxMat = getNxSDK()->getMaterial( mNxMatIndex );
		YAKE_ASSERT( pNxMat );
		pNxMat->restitution = restitution;
	}
	void MaterialNx::setStaticFriction(const real friction)
	{
		NxMaterial* pNxMat = getNxSDK()->getMaterial( mNxMatIndex );
		YAKE_ASSERT( pNxMat );
		pNxMat->staticFriction = friction;
	}
	void MaterialNx::setStaticFrictionV(const real friction)
	{
		NxMaterial* pNxMat = getNxSDK()->getMaterial( mNxMatIndex );
		YAKE_ASSERT( pNxMat );
		pNxMat->staticFrictionV = friction;
	}
	void MaterialNx::setStaticFrictionVEnabled(bool enabled)
	{
	}

}	
}