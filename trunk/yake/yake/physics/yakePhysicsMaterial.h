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
#ifndef YAKE_PHYSICS_MATERIAL_H
#define YAKE_PHYSICS_MATERIAL_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	class IMaterial
	{
	public:
		struct Desc
		{
			Desc( real bounciness = 0.5,
					real softness = 0.2,
					real friction = 0.1,
					real restitution = 0.5, 
					real staticFriction = 0.3, 
					real rollingFriction = 0.2 ) :
				mFriction( friction ),
				mRestitution( restitution ),
				mStaticFriction( staticFriction ),
				mRollingFriction( rollingFriction )
			{}
			
			virtual ~Desc() {}
			
			real mBounciness;
			real mSoftness;
			real mFriction;
			real mRestitution;
			real mStaticFriction;
			real mRollingFriction;
		};

		virtual ~IMaterial() {}

		virtual void setBounciness(const real bounciness) = 0;
		virtual void setSoftness(const real softness) = 0;
		virtual void setRollingFriction(const real friction) = 0;
		virtual void setRestitution(const real restitution) = 0;
		virtual void setStaticFriction(const real friction) = 0;
		virtual void setStaticFrictionV(const real friction) = 0;
		virtual void setStaticFrictionVEnabled(bool enabled) = 0;

		//@todo add more properties that can quite easily be generalised...
	};
	YAKE_PHYSICS_COMMON_POINTERS( IMaterial );

}
}
#endif
