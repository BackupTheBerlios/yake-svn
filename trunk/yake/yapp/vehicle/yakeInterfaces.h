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

namespace yake {
namespace vehicle {

	const size_t MPID_NO_PARENT = 0xffffffff;
	struct VehicleTemplate
	{
		struct MountPointTpl
		{
			Vector3		mPosition;
			Quaternion	mOrientation;
			Vector3		mDirection;
			size_t		mParentMountPointIdx;
			bool		mUseDirection;
			MountPointTpl(
				const Vector3& pos = Vector3::kZero,
				const Quaternion rot = Quaternion::kIdentity,
				size_t parentMountPoint = MPID_NO_PARENT) :
				mPosition(pos),
				mOrientation(rot),
				mParentMountPointIdx(parentMountPoint),
				mUseDirection(false)
			{}
			MountPointTpl(
				const Vector3& pos,
				const Vector3& dir,
				size_t parentMountPoint = MPID_NO_PARENT) :
				mPosition(pos),
				mDirection(dir),
				mParentMountPointIdx(parentMountPoint),
				mUseDirection(true)
			{}
		};
		typedef Deque< MountPointTpl > MountPointTplList;
		enum GearMode
		{
			GM_FORWARD,
			GM_NEUTRAL,
			GM_REVERSE
		};
		struct GearTpl
		{
			real				ratio_;
			GearMode			mode_;
			GearTpl() : ratio_(1.), mode_(GM_FORWARD) {}
		};
		typedef Vector<GearTpl> GearTplList;
		struct EngineTpl
		{
			virtual ~EngineTpl() {}
		};
		struct ThrusterTpl : public EngineTpl
		{
			real				minForce;
			real				maxForce;
			size_t				mountPtIdx;
			ThrusterTpl(real minF = 0., real maxF = 1., size_t mtPt = 0) : minForce(minF), maxForce(maxF), mountPtIdx(mtPt) {}
		};
		struct CarEngineTpl : public EngineTpl
		{
			real				rpmMin_;
			real				rpmMax_;
			real				rpmRedline_;
			real				rpmDieOff_;
			real				rpmRise_;
			real				minShiftingTime_;
			GearTplList			gears_;
			//Curve*			torqueCurve_;
			real				differentialRatio;
			CarEngineTpl() :
				rpmMin_(0.),
				rpmMax_(8000.),
				rpmRedline_(6000.),
				rpmDieOff_(800.),
				rpmRise_(2000.),
				minShiftingTime_(0.15),
				differentialRatio(3.14)
			{}
		};

		typedef Deque<EngineTpl*> EngineTplList;

		typedef Deque<physics::IShape::Desc*> ShapeTplList;

		struct ChassisTpl
		{
			Vector3			mPosition; // initial position
			real			mMass;
			ShapeTplList	mChassisShapes;
			String			mGfxReference; // e.g. dotScene file
			ChassisTpl() : 
				mPosition(Vector3::kZero),
				mMass(1.6)
			{}
		};

		struct WheelTpl
		{
			Vector3			mPosition;
			Quaternion		mOrientation;
			uint32			mSteeringGroup;
			real			mRadius;
			real			mMass;
			bool			mMassRelativeToChassis;
			real			mSuspensionSpring;
			real			mSuspensionDamping;
			String			mGfxReference; // e.g. dotScene file
			String			mGfxReferenceType; // e.g. "dotscene"
			WheelTpl() :
				mPosition(Vector3::kZero),
				mOrientation(Quaternion::kIdentity),
				mSteeringGroup(0),
				mRadius(0.8),
				mMass(0.015),
				mMassRelativeToChassis(true),
				mSuspensionSpring(1),
				mSuspensionDamping(0.9)
			{}
		};

		typedef Deque< WheelTpl > WheelTplList;

		//

		ChassisTpl		mChassis;
		MountPointTplList	mMountPoints;
		EngineTplList	mEngines;
		uint32			mSteeringGroups;
		WheelTplList	mWheels;

		VehicleTemplate() : mSteeringGroups(0)
		{}
		~VehicleTemplate()
		{
			ConstVectorIterator< EngineTplList > itEngine( mEngines );
			while (itEngine.hasMoreElements())
				delete itEngine.getNext();
			mEngines.clear();
			ConstVectorIterator< ShapeTplList > itShape( mChassis.mChassisShapes );
			while (itShape.hasMoreElements())
				delete itShape.getNext();
			mChassis.mChassisShapes.clear();
		}
	};

	/** Base class for objects that can be attached to mount points (MountPoint).
		@see MountPoint
	*/
	class YAKE_VEH_API Mountable
	{
		friend class MountPoint;
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
		void setDirection( const Vector3& dir );
		Vector3 getDirection() const;
		virtual void setOrientation( const Quaternion& );
		virtual Quaternion getOrientation() const;
		virtual void setPosition(const Vector3& rPosition );
		virtual Vector3 getPosition() const;
		Vector3 getDerivedPosition() const;
		Quaternion getDerivedOrientation() const;
		void lookAt(const Vector3&);
	private:
		typedef Deque< Mountable* > MountablePtrList;
		MountablePtrList	mMountables;
		Quaternion			mOrientation;
		Vector3				mPosition;
		typedef Deque< std::pair<MountPoint*,bool> > MountPointList;
		MountPointList		mChildren;
		MountPoint*			mParent;
		Movable*			mOverrideParent;
	};

	class IVehicle;
	class YAKE_VEH_API IVehicleSystem
	{
		YAKE_DECLARE_REGISTRY_0(IVehicleSystem,String);
	public:
		virtual IVehicle* create(const VehicleTemplate&, physics::IWorld& PWorld) = 0;
	};

	class IEngine;
	class IWheel;
	class YAKE_VEH_API IVehicle
	{
	public:
		virtual ~IVehicle() {}

		virtual void updateSimulation( real timeElapsed ) = 0;

		//virtual void addMountPoint(MountPoint*) = 0;
		//virtual MountPoint* removeMountPoint(MountPoint*) = 0;

		virtual MountPoint* getMountPoint(size_t index) const = 0;
		virtual IEngine* getEngineInterface(size_t index) const = 0;
		virtual IWheel* getWheelInterface(size_t index) const = 0;

		virtual Vector3 getChassisPosition() const = 0;
		virtual Quaternion getChassisOrientation() const = 0;
		virtual Movable* getChassisMovable() const = 0;
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
	};

} // namespace vehicle
} // namespace yake


#endif
