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
#ifndef YAKE_APP_MODEL_VEHICLE_NATIVEWHEEL_H
#define YAKE_APP_MODEL_VEHICLE_NATIVEWHEEL_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeModel.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	/** Utilized by the "yake.native" vehicle implementation.
	*/
	class NativeWheel : public physics::IActor::ISlipNormalSource, public Movable
	{
	public:
		NativeWheel(physics::IWorld* pPWorld);
		virtual ~NativeWheel();
		physics::IActor* getPhysicsComplex() const;

		void setJoint( SharedPtr<physics::IJoint> & jt );
		physics::IJoint* getJoint() const;
		void setSuspension( const real spring, const real damping );
		real getSuspensionSpring() const;
		real getSuspensionDamping() const;
		void applySteer( real steer );

		void apply( real velocity, real fmax );
		void applyTq( const Vector3 & torque );
		void applyBrakeTq( const Vector3 & torque );

		void update();

		void setRadius( const real radius );
		void setMass( const real mass );

		Vector3 getLateralSlipNormal() const;

		virtual void setPosition(const Vector3 & rPosition);
		virtual void setOrientation(const Quaternion& rOrientation);
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;

		SharedPtr<CachedInterpolator<real> > _getLinSkidCache();
		SharedPtr<CachedInterpolator<real> > _getAngSkidCache();
	private:
		SharedPtr<physics::IJoint>	mJoint;
		physics::IActor*	mCO;
		SharedPtr<CachedInterpolator<real> >	mLinSkidCache;
		SharedPtr<CachedInterpolator<real> >	mAngSkidCache;
	};

}
}
}
}


#endif