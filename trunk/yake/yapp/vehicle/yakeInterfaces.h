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
#ifndef YAKE_VEHICLE_INTERFACES_H
#define YAKE_VEHICLE_INTERFACES_H

#include "yakePrerequisites.h"
#include "yakeTemplates.h"

namespace yake {
namespace vehicle {

	enum Steering {
		S_LEFT,
		S_RIGHT,
		S_NONE
	};

	class IEngine;
	typedef Deque<IEngine*> IEnginePtrList;
	class IWheel;
	class MountPoint;
	class Mountable;
	class YAKE_VEH_API IVehicle
	{
		IVehicle(const IVehicle&);
		IVehicle& operator=(const IVehicle&);
	protected:
		IVehicle() {}
	public:
		virtual ~IVehicle() {}

		virtual void updateSimulation( real timeElapsed ) = 0;

		//virtual void addMountPoint(MountPoint*) = 0;
		//virtual MountPoint* removeMountPoint(MountPoint*) = 0;

		virtual MountPoint* getMountPoint(const String& id) const = 0;
		virtual IEngine* getEngineInterface(const String& id) const = 0;
		virtual IEnginePtrList getEngineInterfaces() const = 0;
		virtual IWheel* getWheelInterface(const String& id) const = 0;
		virtual void setSteering( const uint32 sg, const real ) = 0;
		virtual real getSteering( const uint32 sg ) const = 0;

		virtual Vector3 getChassisPosition() const = 0;
		virtual Quaternion getChassisOrientation() const = 0;
		virtual Movable* getChassisMovable() const = 0;

		virtual void enableDebugGeometry(graphics::IWorld&) = 0;
		virtual void disableDebugGeometry() = 0;
	};

	/** Interface to a vehicle engine.
		@remarks This is work-in-progress and may be revised at any time.
	*/
	class YAKE_VEH_API IEngine
	{
	public:
		virtual ~IEngine() 
		{}

		virtual void updateSimulation( real timeElapsed ) = 0;

		virtual void setThrottle( real throttle ) = 0;
		virtual real getThrottle() const = 0;
	};

	/** Interface to a vehicle engine.
		@remarks This is work-in-progress and may be revised at any time.
	*/
	class YAKE_VEH_API ICarEngine : public IEngine
	{
	public:
		virtual ~ICarEngine() 
		{}

		//virtual void setParameter( const String & name, const String & value );
		virtual void setParamMinRPM( const real rpm ) = 0;
		virtual void setParamMaxRPM( const real rpm ) = 0;
		virtual void setParamRedlineRPM( const real rpm ) = 0;

		virtual real getRPM() const = 0;
		virtual uint8 getGear() const = 0;
		virtual void shiftGear( uint8 gear ) = 0;
		virtual void shiftGearUp() = 0;
		virtual void shiftGearDown() = 0;

		/** Returns the torque for the current engine data. [Nm] */
		virtual real getDriveTorque() const = 0;
	};

	/** A thruster is a special kind of engine. It applies force to the vehicle
		it's attached to at a specified offset, direction etc.
		@see MountedThruster
		@see vehicle::IEngine
	*/
	class YAKE_VEH_API IThruster : public IEngine
	{
	protected:
		virtual ~IThruster();
		IThruster();
	public:
		/** At least the following methods are inherited from vehicle::IEngine:

		virtual void updateSimulation( real timeElapsed ) = 0;
		virtual void setThrottle( real throttle ) = 0;
		virtual real getThrottle() const = 0;
		*/
		void setMinimumForce( real force );
		real getMinimumForce() const;
		void setMaximumForce( real force );
		real getMaximumForce() const;
		real getForce() const;
	protected:
		void setForce( real force );
	private:
		real	mMaxForce;
		real	mMinForce;
		real	mForce;
	};

	/** A thruster that can be attached to a mount point.
		@see Thruster
	*/
	class YAKE_VEH_API IMountedThruster : public Mountable
	{
	protected:
		IMountedThruster();
		virtual ~IMountedThruster();
	public:
		void setThruster(IThruster*);

		void addTarget( physics::IBody* );
		void removeTarget( physics::IBody* );
		void applyToTargets();

	protected:
		virtual void onApplyToTargets() = 0;
	protected:
		typedef Deque<physics::IBody*> BodyPtrList;
		BodyPtrList		mTargets;
		IThruster*		mThruster;
	};

	class YAKE_VEH_API IWheel : public Movable
	{
	public:
		virtual ~IWheel() {}

		virtual real getRadius() const = 0;
		virtual void brake(const real ratio) = 0;
	};

} // namespace vehicle
} // namespace yake


#endif
