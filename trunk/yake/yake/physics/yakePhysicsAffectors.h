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
#ifndef YAKE_PHYSICS_AFFECTORS_H
#define YAKE_PHYSICS_AFFECTORS_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
namespace physics {

	/** A generic body affector. Basically defines the interface, factory and implements
		some basic and usually common functionality.
	*/
	class YAKE_PHYSICS_API IBodyAffector
	{
		YAKE_DECLARE_REGISTRY_0( IBodyAffector, String )
	public:
		virtual ~IBodyAffector() {}

		virtual void applyTo( IBody & rBody, const real timeElapsed ) = 0;
		virtual void applyTo( BodyGroup & rGroup, const real timeElapsed );
	};

	/** Constant acceleration affector template. Base class for constant acceleration affectors.
		\see ConstantDirectionalAccelerationBodyAffector
		\see ConstantPointAccelerationBodyAffector
	*/
	template< typename AccType >
	class ConstantAccelerationBodyAffector : public IBodyAffector
	{
	protected:
		AccType		mAcceleration;
	public:
		ConstantAccelerationBodyAffector()
		{
		}
		
		ConstantAccelerationBodyAffector( const AccType & acc ) : mAcceleration( acc )
		{ }

		void setAcceleration( const AccType & acc )
		{ mAcceleration = acc; }

		AccType getAcceleration() const
		{ return mAcceleration; }
	};

	/** A directional constant acceleration affector. Actually it could be used for dynamic accelerations, too.
	*/
	class ConstantDirectionalAccelerationBodyAffector : public ConstantAccelerationBodyAffector<math::Vector3>
	{
		YAKE_DECLARE_CONCRETE( ConstantDirectionalAccelerationBodyAffector, "yake.constant_directional_acceleration" )
	public:
		virtual void applyTo( IBody & rBody, const real /*timeElapsed*/ )
		{
			rBody.addForce( mAcceleration * rBody.getMass() );
		}
	};

	/** A point acceleration affector which applies a constant acceleration on affected bodies where
		the resulting force vector always points toward a given point. Nice for spherical gravitation,
		like on planets.
	*/
	class BodyConstantSphericalAccelerationAffector : public ConstantAccelerationBodyAffector<real>
	{
		YAKE_DECLARE_CONCRETE( BodyConstantSphericalAccelerationAffector, "yake.constant_spherical_acceleration" )
	protected:
		math::Vector3		mPoint;
	public:
		BodyConstantSphericalAccelerationAffector( const math::Vector3 & point = math::Vector3::kZero, const real acc = 1. ) : 
			mPoint( point ), 
			ConstantAccelerationBodyAffector<real>( acc )
		{ }

		virtual void applyTo( IBody & rBody, const real timeElapsed )
		{
			math::Vector3 direction = rBody.getActor().getPosition() - mPoint;
			direction.normalise();
			rBody.addForce( direction * mAcceleration * rBody.getMass() );
		}
	};

}
}

#endif

