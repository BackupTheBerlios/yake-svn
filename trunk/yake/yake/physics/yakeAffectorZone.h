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
#ifndef YAKE_PHYSICS_AFFECTORZONE_H
#define YAKE_PHYSICS_AFFECTORZONE_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
namespace physics {

	/**@todo move to base::math
	*/

	class BoundingVolume
	{
	public:
		virtual ~BoundingVolume() {}

		//static bool intersectsWith( BoundingSphere& rSphere, BoundingSphere& rSphere );
	};
	/*
	class BoundingSphere
	{
	public:
		BoundingSphere(const BoundingSphere& rOther);
		BoundingSphere(const real radius = 1, const Vector3& rPosition = Vector3::kZero );
		BoundingSphere& operator=(const BoundingSphere& rOther);

		void setRadius(const real radius)
		{ mRadius = radius; mRadiusSquared = radius*radius; }
		inline real getRadius() const
		{ return mRadius; }
		void setPosition(const Vector3& rPosition)
		{ mPosition = rPosition; }
		inline Vector3 getPosition() const
		{ return mPosition; }

		bool intersectsWith( const BoundingSphere& rSphere )
		{
			real distanceSquared = (rSphere.getPosition() - mPosition()).squaredLength();
			return (distanceSquared <= (mRadiusSquared + rSphere.mRadiusSquared));
		}
	private:
		real		mRadius;
		real		mRadiusSquared;
		Vector3		mPosition;
	};
	BoundingSphere::BoundingSphere(const BoundingSphere& rOther) :
		mRadius( rOther.mRadius ),
		mRadiusSquared( rOther.mRadiusSquared ),
		mPosition( rOther.mPosition )
	{
	}
	BoundingSphere::BoundingSphere(const real radius = 1, const Vector3& rPosition = Vector3::kZero ) :
		mRadius( radius ),
		mRadiusSquared( radius*radius ),
		mPosition( rPosition )
	{
	}
	BoundingSphere& BoundingSphere::operator=(const BoundingSphere& rOther)
	{
		mRadius = rOther.mRadius;
		mRadiusSquared = rOther.mRadiusSquared;
		mPosition = rOther.mPosition;
	}

	class SpatialManager
	{
		typedef Deque<BoundingVolume> BoundingVolumeList;
	};
	*/

	class AffectorZone
	{
	protected:
		typedef Vector< SharedPtr<BoundingVolume> > VolumeList;
		VolumeList				mVolumes;

		typedef Vector< SharedPtr<physics::IBodyAffector> > AffectorList;
		AffectorList			mAffectors;

		SharedPtr<IWorld>		mWorld;
		BodyGroup				mAffectedBodies;
	public:
		AffectorZone();
		void addVolume( SharedPtr<BoundingVolume>& pVolume );
		void addAffector( SharedPtr<IBodyAffector>& pAffector );
		void setWorld( SharedPtr<IWorld>& pWorld );

		void setEnabled( bool enabled );
		bool isEnabled() const;

		virtual void update( const real timeElapsed );

	protected:
		void _determineAffectedBodies( const SharedPtr<IWorld> & pWorld );
	private:
		bool		mEnabled;
		bool		mNeedsUpdate;
	};

	class AffectorZoneManager
	{
	public:
		void addAffectorZone( SharedPtr<AffectorZone> & pSpace );

		virtual void update( const real timeElapsed )
		{
			/**@todo
			foreach forceSpace
				if ((*it)->isEnabled())
					(*it)->update( upd );
			*/
		}
	};

}
}

#endif
