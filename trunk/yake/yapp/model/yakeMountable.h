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
#ifndef YAPP_MODEL_MOUNTABLE_H
#define YAPP_MODEL_MOUNTABLE_H

#include <yapp/base/yappPrerequisites.h>

namespace yake {
namespace app {
namespace model {

	class MountPoint;

	/** Base class for objects that can be attached to mount points (MountPoint).
		@see MountPoint
	*/
	class YAPP_BASE_API Mountable
	{
	public:
		MountPoint* getMountPoint() const;
	};

	/** Base class for mount points. Mountable objects can be attached to it.
		@see Mountable
	*/
	class YAPP_BASE_API MountPoint : public yake::base::Movable
	{
	protected:
		MountPoint() {}
	public:
		virtual ~MountPoint() {}
		bool isSuitableFor( const Mountable & mountable ) const;
		void attach( SharedPtr<Mountable> & mountable );
		void detach( const SharedPtr<Mountable> & mountable );
	};

	/** Example class representing a turret with one or two axis of rotation.
	*/
	class YAPP_BASE_API Turret //: public Entity
	{
	public:
	};

	/** Example class representing a mountable turret, i.e. a turret which can
		be attached to a mount point.
	*/
	class YAPP_BASE_API MountedTurrent : public Turret, public Mountable
	{
	public:
	};

	/** A thruster is a special kind of engine. It applies force to the vehicle
		it's attached to at a specified offset, direction etc.
		@see MountedThruster
		@see vehicle::IEngine
	*/
	class YAPP_BASE_API Thruster : public vehicle::IEngine
	{
	public:
		/** At least the following methods are inherited from vehicle::IEngine:

		virtual void updateSimulation( real timeElapsed ) = 0;
		virtual void setThrottle( real throttle ) = 0;
		virtual real getThrottle() const = 0;
		*/
		void setMinimumForce( real force );
		void setMaximumForce( real force );
		real getForce() const;
	};

	/** A thruster that can be attached to a mount point.
		@see Thruster
	*/
	class YAPP_BASE_API MountedThruster :  public Mountable
	{
	public:
	};

} // model
} // app
} // yake

#endif