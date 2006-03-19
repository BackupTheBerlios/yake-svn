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

#include <yapp/vehicle/yakePrerequisites.h>

namespace yake {
namespace vehicle {

	//-----------------------------------------------------
	// Class: GearBox
	//-----------------------------------------------------
	class GearBox
	{
	public:
		enum GearMode {
			GM_REVERSE,
			GM_NEUTRAL,
			GM_FORWARD
		};
		void setSpeed( uint8 numGears );
		uint8 getSpeed() const;

		void setGearRatio( uint8 gear, real ratio );
		void setGearMode( uint8 gear, const GearMode mode );
		real getGearRatio( uint8 gear );

		void setFromTemplate( const VehicleTemplate::GearTplList & gears );
	private:
		struct Gear
		{
			real		ratio_;
			GearMode	mode_;
		};
		typedef Vector< Gear > GearList;
		GearList	mGears;
	};

	class DotVehicleParser;
	class YAKE_VEH_API GenericVehicleSystem : public IVehicleSystem
	{
		YAKE_DECLARE_CONCRETE(GenericVehicleSystem,"generic");
	public:
		GenericVehicleSystem();
		~GenericVehicleSystem();
		virtual IVehicle* create(const VehicleTemplate&, physics::IWorld& PWorld, model::Physical& physModel);
		virtual bool loadTemplates(const String& fn);
		virtual VehicleTemplate* getTemplate(const String& tpl) const;
		//virtual VehicleTemplate* cloneTemplate(const String& tpl);
		virtual IVehicle* create(const String& tpl, physics::IWorld& PWorld, model::Physical& physModel);
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

		virtual math::Vector3 getChassisPosition() const;
		virtual math::Quaternion getChassisOrientation() const;
		virtual Movable* getChassisMovable() const;

		virtual void setSteering( const uint32 sg, const real );
		virtual real getSteering( const uint32 sg ) const;

		virtual void enableDebugGeometry(graphics::IWorld&);
		virtual void disableDebugGeometry();

		void _create(const VehicleTemplate&, physics::IWorld& PWorld, model::Physical& physModel );

		YAKE_MEMBERSIGNAL(private,void(real),UpdateEngineSimulation);
		YAKE_MEMBERSIGNAL(private,void(void),ApplyThrusterToTargets);

	private:
		void _createMountPoint(const String& id, const VehicleTemplate::MountPointTpl&,MountPoint* parentMtPt = 0);
		void _applyDriveTorqueToAxles( real timeElapsed );
	private:
		typedef AssocVector<String,IEngine*> EnginePtrList;
		EnginePtrList		mEngines; // owner of all engine objects

		physics::IActorPtr	mpChassis;

		typedef AssocVector<uint32,Deque<OdeWheel*> > SteeringGroupList;
		SteeringGroupList	mSteeringGroups;

		typedef std::pair<ICarEngine*,Deque<OdeWheel*> > CarEngineWheelsPair;
		typedef AssocVector<uint32,CarEngineWheelsPair> AxleList;
		AxleList			mAxles;

		typedef AssocVector<String,OdeWheel*> WheelList;
		WheelList			mWheels;

		typedef AssocVector<String,MountPoint*> MountPointList;
		MountPointList		mMountPoints;

		model::complex::Model*	mDebugModel;
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

		virtual void setPosition(const math::Vector3&) {}
		virtual math::Vector3 getPosition() const;
		virtual math::Vector3 getDerivedPosition() const;
		virtual void setOrientation(const math::Quaternion&) {}
		virtual math::Quaternion getOrientation() const;
		virtual math::Quaternion getDerivedOrientation() const;

		virtual real getRadius() const;

		virtual void brake(const real ratio);

		virtual real getSkid() const;

		void setSteering( const real s );

		void _applyDriveTq( const real tq );
	private:
		void _applyTq( const math::Vector3 & torque );
		void _applyBrakeTq( const math::Vector3 & torque );
		void _applyMotor( real velocity, real fmax );

		void _onPreStepInternal( const real dt );
		void _onPostStep( const real dt );
	private:
		physics::IActorPtr		mpChassis;
		physics::IJointPtr		mpJoint;
		physics::IActorPtr		mpWheel;
		real				mRadius;
		real				mTargetSteer;
		real				mCurrSteer;
		SignalConnection		mPreStepSigConn, mPostStepSigConn;
		real				mBrakeRatio;
		real				mSkid;
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

	/** In terms of control systems it is just a gain.
	 * Output = input*K_gain; force = voltage*k.
	 */
	class GenericThruster : public IThruster
	{
	public:
	    GenericThruster();

	    virtual void setInputSignal( real voltage );
	    virtual real getInputSignal() const;

	    virtual void updateSimulation( real timeElapsed );

	    virtual void setGain( real gain );
	    virtual real getGain() const;

	private:
	    real    mVoltage; // cached gain value

	    /// gain coefficient is the only *real* characteristic of such device.
	    real    mGain;
	};

	class GenericLinearThruster : public ILinearThruster
	{
	public:
		GenericLinearThruster();

		virtual void setInputSignal( real voltage );
		virtual real getInputSignal() const;
		virtual void updateSimulation( real timeElapsed );

	private:
		real	mVoltage;
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

