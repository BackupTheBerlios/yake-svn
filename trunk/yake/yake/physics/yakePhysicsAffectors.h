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
#ifndef INC_YAKE_PHYSICS_AFFECTORS_H
#define INC_YAKE_PHYSICS_AFFECTORS_H

#ifndef INC_YAKE_PREREQUISITES_H
#include <yake/base/yakePrerequisites.h>
#endif

namespace yake {
	namespace physics {

		/** A generic body affector. Basically defines the interface and implements
			some basic and usually common functionality.
		*/
		class BodyAffector
		{
		public:
			virtual ~BodyAffector() {}
			virtual void apply( IBody* pBody, real fTimeElapsed ) = 0;

			/** Default implementation for applying an affector to a group of bodies.
			*/
			virtual void apply( const BodyGroup & bodyGroup, real fTimeElapsed )
			{
				for (BodyGroup::BodyList::const_iterator it = bodyGroup.getBodyList().begin(); it != bodyGroup.getBodyList().end(); ++it)
				{
					apply( *it, fTimeElapsed );
				}
			}
		};

		/** Constant acceleration affector template. Base class for constant acceleration affectors.
			\see ConstantDirectionalAccelerationBodyAffector
			\see ConstantPointAccelerationBodyAffector
		*/
		template< typename AccType >
		class ConstantAccelerationBodyAffector : public BodyAffector
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
		class ConstantDirectionalAccelerationBodyAffector : public ConstantAccelerationBodyAffector<Vector3>
		{
		public:
			virtual void apply( physics::IBody* pBody, real fTimeElapsed )
			{
				if (pBody)
				{
					Vector3 force = mAcceleration * pBody->getMass();
					pBody->addForce( force );
				}
			}
		};

		/** A point acceleration affector which applies a constant acceleration on affected bodies where
			the resulting force vector always points toward a given point. Nice for spherical gravitation,
			like on planets.
		*/
		class BodyConstantPointAccelerationAffector : public ConstantAccelerationBodyAffector<real>
		{
		protected:
			Vector3		mPoint;
		private:
			BodyConstantPointAccelerationAffector( const real & acc );
		public:
			BodyConstantPointAccelerationAffector( const Vector3 & point, const real & acc ) : mPoint( point ), ConstantAccelerationBodyAffector<real>( acc )
			{ }

			virtual void apply( IBody* pBody, real fTimeElapsed )
			{
				if (pBody)
				{
					Vector3 direction = pBody->getPosition() - mPoint;
					direction.normalise();
					Vector3 force = direction * mAcceleration * pBody->getMass();
					pBody->addForce( force );
				}
			}
		};


	}
}

#endif

