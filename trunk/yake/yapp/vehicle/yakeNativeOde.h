/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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

	class DotVehicleParser;
	class YAKE_VEH_API GenericVehicleSystem : public IVehicleSystem
	{
		YAKE_DECLARE_CONCRETE(GenericVehicleSystem,"generic");
	public:
		GenericVehicleSystem();
		~GenericVehicleSystem();
		virtual IVehicle* create(const VehicleTemplate&, physics::IWorld& PWorld);
		virtual bool loadTemplates(const String& fn);
		virtual VehicleTemplate* getTemplate(const String& tpl) const;
		//virtual VehicleTemplate* cloneTemplate(const String& tpl);
		virtual IVehicle* create(const String& tpl, physics::IWorld& PWorld);
	private:
		void _onVehicleTpl(DotVehicleParser& parser, const String& tplId);
	private:
		typedef AssocVector<String,SharedPtr<vehicle::VehicleTemplate> > VehTplList;
		VehTplList				mVehicleTemplates;
	};

	class OdeWheel;
	class GenericVehicle : public IVehicle
	{
	public:
		GenericVehicle();
		virtual ~GenericVehicle();

		virtual void updateSimulation( real timeElapsed );

		//virtual void addMountPoint(MountPoint*);
		//virtual MountPoint* removeMountPoint(MountPoint*);

		virtual MountPoint* getMountPoint(const String& id) const;

		virtual IEngine* getEngineInterface(const String& id) const;
		virtual IEnginePtrList getEngineInterfaces() const;
		virtual IWheel* getWheelInterface(const String& id) const;

		virtual Vector3 getChassisPosition() const;
		virtual Quaternion getChassisOrientation() const;
		virtual Movable* getChassisMovable() const;

		virtual void setSteering( const uint32 sg, const real );
		virtual real getSteering( const uint32 sg ) const;

		void _create(const VehicleTemplate&, physics::IWorld& PWorld);

		YAKE_MEMBERSIGNAL(private,void(real),UpdateThrusterSimulation);
		YAKE_MEMBERSIGNAL(private,void(void),ApplyThrusterToTargets);

	private:
		void _createMountPoint(const String& id, const VehicleTemplate::MountPointTpl&,MountPoint* parentMtPt = 0);
	private:
		typedef AssocVector<String,IEngine*> EnginePtrList;
		EnginePtrList		mEngines; // owner of all engine objects

		physics::IActorPtr	mpChassis;

		typedef AssocVector<uint32,Deque<OdeWheel*> > SteeringGroupList;
		SteeringGroupList	mSteeringGroups;

		typedef AssocVector<String,OdeWheel*> WheelList;
		WheelList			mWheels;

		typedef AssocVector<String,MountPoint*> MountPointList;
		MountPointList		mMountPoints;
	};

	class GearBox;
	class GenericCarEngine : public ICarEngine
	{
	public:
		GenericCarEngine();
		virtual ~GenericCarEngine();

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

#if defined(YAKE_VEHICLE_USE_ODE)
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

		virtual real getRadius() const;

		void setSteering( const real s );
	private:
		void _onPreStepInternal( const real dt );
	private:
		physics::IJointPtr		mpJoint;
		physics::IActorPtr		mpWheel;
		real					mRadius;
		real					mTargetSteer;
		real					mCurrSteer;
		SignalConnection		mPostStepSigConn;
	};
#endif // YAKE_VEHICLE_USE_ODE
	class GenericMountPoint : public MountPoint
	{
	public:
		GenericMountPoint() {}

	private:
		//typedef Deque<IThruster*> ThrusterPtrList;
		//ThrusterPtrList		mThrusters; // references thruster engines (does NOT own them!)
	};

	class GenericThruster : public IThruster
	{
	public:
		GenericThruster();

		virtual void setThrottle( real throttle );
		virtual real getThrottle() const;
		virtual void updateSimulation( real timeElapsed );

	private:
		real	mThrottle;
	};
	class GenericMountedThruster : public IMountedThruster
	{
	public:
		GenericMountedThruster();

		virtual void onApplyToTargets();
	private:
	};

} // namespace vehicle
} // namespace yake


#endif
