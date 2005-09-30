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
#ifndef YAKE_VEHICLE_NATIVEODE_H
#define YAKE_VEHICLE_NATIVEODE_H

#include "yakePrerequisites.h"

namespace yake {
namespace vehicle {

	class YAKE_VEH_API OdeVehicleSystem : public IVehicleSystem
	{
		YAKE_DECLARE_CONCRETE(OdeVehicleSystem,"ode");
	public:
		OdeVehicleSystem();
		virtual IVehicle* create(const VehicleTemplate&, physics::IWorld& PWorld);
	};

	class OdeWheel;
	class OdeVehicle : public IVehicle
	{
	public:
		OdeVehicle();
		virtual ~OdeVehicle();

		virtual void updateSimulation( real timeElapsed );

		//virtual void addMountPoint(MountPoint*);
		//virtual MountPoint* removeMountPoint(MountPoint*);

		virtual MountPoint* getMountPoint(size_t index) const;

		virtual IEngine* getEngineInterface(size_t index) const;
		virtual IWheel* getWheelInterface(size_t index) const;

		virtual Vector3 getChassisPosition() const;
		virtual Quaternion getChassisOrientation() const;
		virtual Movable* getChassisMovable() const;

		void _create(const VehicleTemplate&, physics::IWorld& PWorld);

		YAKE_MEMBERSIGNAL(private,void(real),UpdateThrusterSimulation);
		YAKE_MEMBERSIGNAL(private,void(void),ApplyThrusterToTargets);
	private:
		typedef Deque<IEngine*> EnginePtrList;
		EnginePtrList		mEngines; // owner of all engine objects

		physics::IActorPtr	mpChassis;

		typedef AssocVector<uint32,Deque<OdeWheel*> > SteeringGroupList;
		SteeringGroupList	mSteeringGroups;

		typedef Deque<OdeWheel*> WheelList;
		WheelList			mWheels;

		typedef Deque<MountPoint*> MountPointList;
		MountPointList		mMountPoints;
	};

	class GearBox;
	class OdeCarEngine : public ICarEngine
	{
	public:
		OdeCarEngine();
		virtual ~OdeCarEngine();

		virtual void setThrottle(real throttle);
		virtual real getThrottle() const;
		virtual void updateSimulation( real timeElapsed );

		//virtual void setParameter( const String & name, const String & value );
		virtual void setParamMinRPM( const real rpm );
		virtual void setParamMaxRPM( const real rpm );
		virtual void setParamRedlineRPM( const real rpm );

		virtual real getRPM() const;
		virtual uint8 getGear() const;
		virtual void shiftGear( uint8 gear );
		virtual void shiftGearUp();
		virtual void shiftGearDown();

		/** Returns the torque for the current engine data. [Nm] */
		virtual real getDriveTorque() const;

		//---------------------------

		GearBox& getGearBox();

		real getMinRPM() const;
		real getMaxRPM() const;

		/** Get the amount the RPM can rise [RPM/sec]. */
		real getRPMRise() const;
		/** Amount of RPM when decreasing when throttle is 0. [RPM/sec] */
		real getRPMDieOff() const;
		/** Minimum time needed to shift gears. */
		real getMinShiftingTime() const;
	private:
		void updateCurrentRPM( const real timeElapsed );
		void updateGearRatios();
		real getMaxTorque( const real rpm );
	private:
		real		mCurrentRPM;
		real		mMaxRPM;
		real		mMinRPM;
		real		mThrottle;
		real		mThrottleRPMRise;
		real		mRPMDieOff;
		real		mGearRatio;
		real		mInvGearRatio;
		real		mDifferentialRatio;
		GearBox*	mpGearBox;
		uint8		mCurrentGear;
		real		mDriveTorque;
	};

	class OdeWheel : public IWheel
	{
		OdeWheel();
	public:
		~OdeWheel();
		OdeWheel(	physics::IActorPtr chassisObj,
					const VehicleTemplate::WheelTpl& tpl,
					physics::IWorld& PWorld );

		virtual void setPosition(const Vector3&) {}
		virtual Vector3 getPosition() const;
		virtual void setOrientation(const Quaternion&) {}
		virtual Quaternion getOrientation() const;
	private:
		physics::IJointPtr		mpJoint;
		physics::IActorPtr		mpWheel;
	};

	class OdeMountPoint : public MountPoint
	{
	public:
		OdeMountPoint() {}

	private:
		//typedef Deque<IThruster*> ThrusterPtrList;
		//ThrusterPtrList		mThrusters; // references thruster engines (does NOT own them!)
	};

	class OdeThruster : public IThruster
	{
	public:
		OdeThruster();

		virtual void setThrottle( real throttle );
		virtual real getThrottle() const;
		virtual void updateSimulation( real timeElapsed );

	private:
		real	mThrottle;
	};
	class OdeMountedThruster : public IMountedThruster
	{
	public:
		OdeMountedThruster();

		virtual void onApplyToTargets();
	private:
	};

} // namespace vehicle
} // namespace yake


#endif
