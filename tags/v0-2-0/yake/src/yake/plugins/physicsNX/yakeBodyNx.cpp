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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakeShapeNx.h>
#include <yake/plugins/physicsNX/yakeActorNx.h>
#include <yake/plugins/physicsNX/yakeBodyNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	BodyNx::BodyNx( NxActor* pActor, ActorNx* pActorNx ) : mNxActor(pActor), mActor(pActorNx)
	{
		YAKE_ASSERT( mNxActor );
		YAKE_ASSERT(mNxActor->isDynamic());
		bool ret = mNxActor->saveBodyToDesc( mNxDesc );
		YAKE_ASSERT( ret );
		YAKE_ASSERT( mNxDesc.isValid() );
		//mNxActor->setDynamic( mNxDesc );
	}
	BodyNx::~BodyNx()
	{
	}
	IActor& BodyNx::getActor() const
	{
		YAKE_ASSERT( mActor );
		return *mActor;
	}
	void BodyNx::setMass(const real mass)
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->setMass( NxReal( mass ) );
	}
	real BodyNx::getMass() const
	{
		YAKE_ASSERT( mNxActor );
		return mNxActor->getMass();
	}
	void BodyNx::setLinearVelocity(const Vector3 & rkVelocity)
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->setLinearVelocity( toNx( rkVelocity ) );
	}
	Vector3 BodyNx::getLinearVelocity() const
	{
		YAKE_ASSERT( mNxActor );
		return fromNx( mNxActor->getLinearVelocity() );
	}
	void BodyNx::setAngularVelocity(const Vector3 & rkVelocity)
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->setAngularVelocity( toNx( rkVelocity ) );
	}
	Vector3 BodyNx::getAngularVelocity() const
	{
		YAKE_ASSERT( mNxActor );
		return fromNx( mNxActor->getAngularVelocity() );
	}
	void BodyNx::addForce( const Force& force )
	{
		//@todo duration should be handled differently, i.e. by applying smaller amounts of force
		//		over a number of steps.
		YAKE_ASSERT( mNxActor );
		if (force.frameType == RF_GLOBAL)
			mNxActor->addForce( toNx( force.force * force.duration ) );
		else
			mNxActor->addLocalForce( toNx( force.force * force.duration ) );
	}
	void BodyNx::addForce( const Vector3 & rkForce )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addForce( toNx( rkForce ) );
	}
	void BodyNx::addForceAtPos( const Vector3 & rkForce, const Vector3 & rkPos )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addForceAtPos( toNx( rkForce ), toNx( rkPos ) );
	}
	void BodyNx::addForceAtLocalPos( const Vector3& rForce, const Vector3& rPos )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addForceAtLocalPos( toNx( rForce ), toNx( rPos ) );
	}
	void BodyNx::addLocalForce( const Vector3 & rkForce )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addLocalForce( toNx( rkForce ) );
	}
	void BodyNx::addLocalForceAtLocalPos( const Vector3 & rkForce, const Vector3 & rkPos )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addLocalForceAtLocalPos( toNx( rkForce ), toNx( rkPos ) );
	}
	void BodyNx::addLocalForceAtPos( const Vector3 & rkForce, const Vector3 & rkPos )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addLocalForceAtPos( toNx( rkForce ), toNx( rkPos ) );
	}
	void BodyNx::addTorque( const Vector3 & rkTorque )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addTorque( toNx( rkTorque ) );
	}
	void BodyNx::addLocalTorque( const Vector3 & rkTorque )
	{
		YAKE_ASSERT( mNxActor );
		mNxActor->addLocalTorque( toNx( rkTorque ) );
	}

}
}