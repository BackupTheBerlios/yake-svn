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
#ifndef INC_YAKE_PHYSICS_AFFECTORZONE_H
#define INC_YAKE_PHYSICS_AFFECTORZONE_H

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
	};

	class AffectorZone
	{
	protected:
		typedef Vector< SharedPtr<math::BoundingVolume> > VolumeList;
		VolumeList				mVolumes;

		typedef Vector< SharedPtr<physics::BodyAffector> > AffectorList;
		AffectorList			mAffectors;

		SharedPtr<IWorld>		mWorld;
		BodyGroup				mAffectedBodies;
		bool					mNeedsUpdate;

	public:
		void addVolume( SharedPtr<BoundingVolume> & pVolume );

		void addAffector( SharedPtr<IBodyAffector> & pAffector );

		void setWorld( SharedPtr<IWorld> & pWorld );

		void setEnabled( bool enabled );
		bool isEnabled() const;

		virtual void update( const real timeElapsed );
		{
			/**@todo
			if (mNeedsUpdate)
			{
				foreach world
					_determineAffectedBodies( *it );
			}
			foreach affector
				(*it)->apply( mAffectedBodies, upd.timeElapsed );
			*/
		}

	protected:
		void _determineAffectedBodies( const SharedPtr<IWorld> & pWorld );
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