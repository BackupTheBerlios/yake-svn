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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeNativeWheel.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	//-----------------------------------------------------
	NativeWheel::NativeWheel(physics::IWorld* pPWorld) : mCO(0)
	{
		YAKE_ASSERT( pPWorld );
		mCO = pPWorld->createSphere( 1 );
		YAKE_ASSERT( mCO );
		YAKE_ASSERT( mCO->getBody() );
		mCO->getBody()->setMassSphere( 1, 1 );
		setMass( 1 );
		mCO->setPosition(Vector3::kZero);
		mCO->setOrientation(Quaternion::kIdentity);
		mCO->setLateralSlipEnabled( true );
		mCO->setSlipCoefficients( 0.1, 0. );
		mCO->setSlipNormalSource( this );
	}
	//-----------------------------------------------------
	NativeWheel::~NativeWheel()
	{
		YAKE_SAFE_DELETE( mCO );
	}
	//-----------------------------------------------------
	void NativeWheel::setRadius( const real radius )
	{
		YAKE_ASSERT( mCO );
		ShapePtrVector v = mCO->getShapes();
		YAKE_ASSERT( v.size() == 1 );
		physics::ICollisionGeometry* pGeom = v.front();
		YAKE_ASSERT( pGeom );
		YAKE_ASSERT( pGeom->getType() == physics::ICollisionGeometry::CGT_SPHERE );
		pGeom->sphereSetRadius( radius );
	}
	//-----------------------------------------------------
	void NativeWheel::setMass( const real mass )
	{
		YAKE_ASSERT( mCO );
		YAKE_ASSERT( mCO->getBody() );
		mCO->getBody()->setMass( mass );
	}
	//-----------------------------------------------------
	void NativeWheel::setPosition(const Vector3 & rPosition)
	{
		YAKE_ASSERT( mCO );
		mCO->setPosition( rPosition );
	}
	//-----------------------------------------------------
	void NativeWheel::setOrientation(const Quaternion& rOrientation)
	{
		YAKE_ASSERT( mCO );
		mCO->setOrientation( rOrientation );
	}
	//-----------------------------------------------------
	Vector3 NativeWheel::getPosition() const
	{
		YAKE_ASSERT( mCO );
		return mCO->getPosition();
	}
	//-----------------------------------------------------
	Quaternion NativeWheel::getOrientation() const
	{
		YAKE_ASSERT( mCO );
		return mCO->getOrientation();
	}
	//-----------------------------------------------------
	SharedPtr<yake::app::model::CachedInterpolator<real> > NativeWheel::_getLinSkidCache()
	{
		if (mLinSkidCache.get() == 0)
		{
			mLinSkidCache.reset( new yake::app::model::CachedInterpolator<real>(8) );
			mLinSkidCache->setWeight(0, 1.0);
			mLinSkidCache->setWeight(0, 0.8);
			mLinSkidCache->setWeight(0, 0.6);
			mLinSkidCache->setWeight(0, 0.5);
			mLinSkidCache->setWeight(0, 0.4);
			mLinSkidCache->setWeight(0, 0.3);
			mLinSkidCache->setWeight(0, 0.2);
			mLinSkidCache->setWeight(0, 0.1);
		}
		return mLinSkidCache;
	}
	//-----------------------------------------------------
	SharedPtr<yake::app::model::CachedInterpolator<real> > NativeWheel::_getAngSkidCache()
	{
		if (mAngSkidCache.get() == 0)
		{
			mAngSkidCache.reset( new yake::app::model::CachedInterpolator<real>(8) );
			mAngSkidCache->setWeight(0, 1.0);
			mAngSkidCache->setWeight(0, 0.8);
			mAngSkidCache->setWeight(0, 0.6);
			mAngSkidCache->setWeight(0, 0.5);
			mAngSkidCache->setWeight(0, 0.4);
			mAngSkidCache->setWeight(0, 0.3);
			mAngSkidCache->setWeight(0, 0.2);
			mAngSkidCache->setWeight(0, 0.1);
		}
		return mAngSkidCache;
	}
	//-----------------------------------------------------
	physics::IActor* NativeWheel::getPhysicsComplex() const
	{
		return mCO;
	}
	//-----------------------------------------------------
	Vector3 NativeWheel::getLateralSlipNormal() const
	{
		YAKE_ASSERT( mJoint );
		return mJoint->getAxis2();
	}
	//-----------------------------------------------------
	void NativeWheel::setJoint( SharedPtr<physics::IJoint> & jt )
	{
		mJoint = jt;
	}
	//-----------------------------------------------------
	void NativeWheel::setSuspension( const real spring, const real damping )
	{
		YAKE_ASSERT( mJoint );
		mJoint->setSpring( spring );
		mJoint->setDamping( damping );
	}
	//-----------------------------------------------------
	real NativeWheel::getSuspensionSpring() const
	{
		YAKE_ASSERT( mJoint );
		return mJoint->getSpring();
	}
	//-----------------------------------------------------
	real NativeWheel::getSuspensionDamping() const
	{
		YAKE_ASSERT( mJoint );
		return mJoint->getDamping();
	}
	//-----------------------------------------------------
	void NativeWheel::applySteer( real steer )
	{
		real epsilon = 0.015;
		//if (steer < 0) epsilon *= -1;
		if (steer > -0.04 && steer < 0.04)
		{
			steer = 0;
			epsilon = 0;
		}
		mJoint->setLowStop( steer - epsilon );
		mJoint->setHighStop( steer + epsilon );
	}
	//-----------------------------------------------------
	void NativeWheel::applyTq( const Vector3 & torque )
	{
		mCO->getBody()->addTorque( mCO->getBody()->getOrientation() * torque );
	}
	//-----------------------------------------------------
	void NativeWheel::applyBrakeTq( const Vector3 & torque )
	{
		Vector3 linVel = mCO->getBody()->getLinearVelocity();
		Vector3 dir = mCO->getBody()->getOrientation() * Vector3::kUnitZ;
		if (dir.dotProduct(linVel) > 0)
			mCO->getBody()->addRelTorque( torque );
		else
			mCO->getBody()->addRelTorque( -torque );
	}
	//-----------------------------------------------------
	void NativeWheel::apply( real velocity, real fmax )
	{
		if (mJoint->getType() == physics::IJoint::JT_HINGE)
		{
			mJoint->setMotor1Velocity( velocity );
			mJoint->setMotor1MaximumForce( fmax );
		}
		else
		{
			mJoint->setMotor2Velocity( velocity );
			mJoint->setMotor2MaximumForce( fmax );
		}
	}
	//-----------------------------------------------------
	physics::IJoint* NativeWheel::getJoint() const
	{
		return mJoint.get();
	}


}
}
}
}