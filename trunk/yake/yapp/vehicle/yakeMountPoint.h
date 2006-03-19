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
#ifndef YAKE_VEHICLE_MOUNTPOINT_H
#define YAKE_VEHICLE_MOUNTPOINT_H

#include "yapp/vehicle/yakePrerequisites.h"

namespace yake {
namespace vehicle {

	/** Base class for objects that can be attached to mount points (MountPoint).
		@see MountPoint
	*/
	class YAKE_VEH_API Mountable
	{
		friend class MountPoint;
		Mountable(const Mountable&);
		Mountable& operator=(const Mountable&);
	protected:
		virtual ~Mountable();
		Mountable();
	public:
		MountPoint* getMountPoint() const;
	protected:
		void onAttached( MountPoint* mountpoint );
		void onDetached();
	private:
		MountPoint*	mPt;
	};

	/** Base class for mount points. Mountable objects can be attached to it.
		@see Mountable
	*/
	class YAKE_VEH_API MountPoint : public Mountable, public Movable
	{
		MountPoint(const MountPoint&);
		MountPoint& operator=(const MountPoint&);
	public:
		MountPoint();
		virtual ~MountPoint();

		void setParent(MountPoint*);
		void setOverrideParentMovable(Movable*);

		void addChild(MountPoint*,bool transferOwnership = true);
		MountPoint* createChild() const;

		virtual bool isSuitableFor( const Mountable* mountable ) const;
		void attach( Mountable* mountable );
		void detach( Mountable* mountable );
		void setDirection( const math::Vector3& dir );
		math::Vector3 getDirection() const;
		virtual void setOrientation( const math::Quaternion& );
		virtual math::Quaternion getOrientation() const;
		virtual void setPosition(const math::Vector3& rPosition );
		virtual math::Vector3 getPosition() const;
		math::Vector3 getDerivedPosition() const;
		math::Quaternion getDerivedOrientation() const;
		void lookAt(const math::Vector3&);
	private:
		typedef Deque< Mountable* > MountablePtrList;
		MountablePtrList	mMountables;
		math::Quaternion			mOrientation;
		math::Vector3				mPosition;
		typedef Deque< std::pair<MountPoint*,bool> > MountPointList;
		MountPointList		mChildren;
		MountPoint*			mParent;
		Movable*			mOverrideParent;
	};

} // namespace vehicle
} // namespace yake


#endif

