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
#ifndef YAPP_MODEL_VEHICLE_H
#define YAPP_MODEL_VEHICLE_H

#define PROFILE_BEGIN( x )
#define PROFILE_END( x )

#pragma warning( disable: 4305 ) // truncation from 'double' to 'yake::real'

using namespace yake;

namespace yapp {
namespace model {
namespace complex {

	class MountPoint;
	class YAPP_BASE_API Mountable
	{
	public:
		MountPoint* getMountPoint() const;
	};

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

	class YAPP_BASE_API Turret //: public Entity
	{
	public:
	};

	class YAPP_BASE_API MountedTurrent : public Turret, public Mountable
	{
	public:
	};

	class YAPP_BASE_API WeaponMountPoint : public MountPoint
	{
	public:
	};

	class YAPP_BASE_API Thruster
	{
	public:
		void setForce( real force );
		void setMinimumForce( real force );
		void setMaximumForce( real force );
		void setThrust( real thrust ); //!< range [0,1]
		real getForce() const;
	};

	class YAPP_BASE_API MountedThruster :  public Mountable
	{
	public:
	};


namespace vehicle {

	class YAPP_BASE_API Wheel : public physics::IComplexObject::ISlipNormalSource
	{
	public:
		Wheel()
		{}
		virtual ~Wheel()
		{}
		//void applySteering( ... );
		//void setGraphical( pGraphical
		void setComplex( physics::IComplexObject* pCO );
		void setJoint( SharedPtr<physics::IJoint> & jt );
		void setSuspension( const real spring, const real damping );
		real getSuspensionSpring() const;
		real getSuspensionDamping() const;
		void applySteer( real steer );

		void apply( real velocity, real fmax );
		void applyTq( const Vector3 & torque );
		void applyBrakeTq( const Vector3 & torque );

		void update();

		Vector3 getLateralSlipNormal() const;
	private:
		int8						mDamageState; ///> tempoarily, just for illustration...
		AutoPtr<Graphical>			mGraphical;
		SharedPtr<physics::IJoint>	mJoint;
		physics::IComplexObject*	mCO;
	};

	/** Simple Axle. Supports one joint & two wheels.
		It could be enhanced to support more wheels
		if necessary.
	*/
	class YAPP_BASE_API Axle
	{
	public:
		Axle()
		{}
		~Axle()
		{}
		void attachAffectedWheel( Wheel* pWheel );
		void applyTorque( real torque );
	private:
		typedef Vector< Wheel* > WheelList;
		WheelList	mWheels;
	};

	class YAPP_BASE_API GearBox
	{
	public:
		enum GearMode {
			GM_REVERSE,
			GM_NEUTRAL,
			GM_NORMAL
		};
		void setSpeed( uint8 numGears );
		uint8 getSpeed() const;

		void setGearRatio( uint8 gear, real ratio );
		void setGearMode( uint8 gear, const GearMode mode );
		real getGearRatio( uint8 gear );

		//TEMP:
		//FIXME:
	private:
		struct Gear
		{
			real		ratio_;
			GearMode	mode_;
		};
		typedef Vector< Gear > GearList;
		GearList	mGears;
	};

	class YAPP_BASE_API Engine // TODO: it's not abstract enough (what about Jet engines? = thrusters?)
	{
	public:
		Engine();
		~Engine();
		GearBox& getGearBox();
		void attachAxle( Axle* pAxle );
		void setThrottle( real throttle );
		real getThrottle() const;

		void setGearBox( GearBox* pGearBox );

		real getMinRPM() const;
		real getMaxRPM() const;
		real getRPM() const;
		uint8 getGear() const;
		void shiftGear( uint8 gear );
		void shiftGearUp();
		void shiftGearDown();

		/** Get the amount the RPM can rise [RPM/sec]. */
		real getThrottleRPMRise() const;
		/** Amount of RPM when decreasing when throttle is 0. [RPM/sec] */
		real getRPMDieOff() const;
		/** Returns the torque for the current engine data. [Nm] */
		real getDriveTorque() const;
		/** Minimum time needed to shift gears. */
		real getMinShiftingTime() const;

		void update( const real timeElapsed );
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
		typedef Vector< Axle* > AxleList;
		AxleList	mAxles;
	private:
		//FIXME:
		void init();
		void updateCurrentRPM( const real timeElapsed );
		void updateGearRatios();
		real getMaxTorque( const real rpm );
		void applyToAxles();
	};

	/** High-level class. Manages a vehicle. Links different components, e.g. in the case of
		a land vehicle these components include gearbox, engine, axles and wheels.
	*/
	class YAPP_BASE_API Vehicle
	{
	protected:
		AutoPtr<complex::Model>	mComplex; // contains graphical,physical,audible etc objects/models

		typedef Vector< SharedPtr<MountPoint> > MountPointList;
		MountPointList			mMountPoints;
	public:
		virtual ~Vehicle()
		{
			mMountPoints.clear();
		}
		/** Sets the complex model (which includes graphical, physical, audible etc data)
			for this vehicle. Ownership is transferred to this Vehicle object.
		*/
		void setModel( complex::Model* pModel )
		{
			YAKE_ASSERT( pModel ).debug("0 ptr");
			if (!pModel)
				return;
			mComplex.reset( pModel );
		}
		virtual void update( real timeElapsed )
		{
			updateMountPoints(); // update mount pt transform from matching scene nodes..
		}
	protected:
		void updateMountPoints()
		{
		}
	};

	class YAPP_BASE_API LandVehicle : public Vehicle
	{
	protected:
		AutoPtr<Engine>			mEngine; // make it a list for multi-engine vehicles

		typedef Vector< Axle* > AxleList;
		AxleList				mAxles;

		typedef AssocVector< String, Wheel* > WheelMap;
		WheelMap				mWheels;

		typedef Vector< std::pair<Wheel*,real> > WheelList;
		struct BrakeEntry
		{
			WheelList	wheels;
			real		amount;
		};
		typedef AssocVector< uint8, SharedPtr<BrakeEntry> > BrakeList;
		BrakeList				mBrakes;

		real					mCurrentSteer;
		real					mSteerTarget;
	public:
		LandVehicle() : mCurrentSteer(0), mSteerTarget(0)
		{}
		~LandVehicle();
		void setEngine( AutoPtr<Engine> & engine );
		Engine* getEngine() const;
		void addAxle( Axle* pAxle );
		void addWheel( const String & id, Wheel* pWheel );
		Wheel* getWheel( const String & id );
		void setSteer( real steer );
		real getSteer() const;

		void setWheelAffectedByBrake( uint8 brake, Wheel* pWheel, real ratio );
		void brake( uint8 brake, real amount );

		void environmentalUpdate( Model* pComplex )
		{
			YAKE_ASSERT( pComplex );
			//pComplex.
		}
		virtual void update( real timeElapsed )
		{
			PROFILE_BEGIN( steer );
			//FIXME: move this out of here:
			real diff = mSteerTarget-mCurrentSteer;
			if (fabs(diff) > 0.04)
				//mCurrentSteer += timeElapsed * diff;// * (0.05 + 0.05 * diff);
				mCurrentSteer = mSteerTarget;
			else
				mCurrentSteer = mSteerTarget;
			getWheel("lf")->applySteer( 0.5 * mCurrentSteer );
			getWheel("rf")->applySteer( 0.5 * mCurrentSteer );
			PROFILE_END( steer );

			//
			PROFILE_BEGIN( vehicle );
			Vehicle::update( timeElapsed );
			PROFILE_END( vehicle );
			PROFILE_BEGIN( engine );
			mEngine->update( timeElapsed );
			PROFILE_END( engine );
			PROFILE_BEGIN( env );
			environmentalUpdate( mComplex.get() );
			for (BrakeList::iterator itBrake = mBrakes.begin(); itBrake != mBrakes.end(); ++itBrake)
			{
				BrakeEntry* pBrake = itBrake->second.get();
				YAKE_ASSERT( pBrake );
				if (pBrake->amount > 0.)
				{
					for (WheelList::iterator itWheel = pBrake->wheels.begin(); itWheel != pBrake->wheels.end(); ++itWheel)
					{
						Wheel* pWheel = itWheel->first;
						YAKE_ASSERT( pWheel );
						real ratio = itWheel->second;
						YAKE_ASSERT( ratio > 0. );

						real torque = -1;
						pWheel->applyBrakeTq( Vector3(ratio * pBrake->amount * torque,0,0) );
					}
				}
			}
			PROFILE_END( env );
			PROFILE_BEGIN( complex );
			mComplex->update( timeElapsed );
			PROFILE_END( complex );
		}
	};

} // vehicle

} // complex
} // model
} // yake

#endif