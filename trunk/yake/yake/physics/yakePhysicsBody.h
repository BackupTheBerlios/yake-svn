/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef YAKE_PHYSICS_BODY_H
#define YAKE_PHYSICS_BODY_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif
#include <yake/base/yakeTaggedListenerManager.h>

namespace yake {
	using namespace ::yake::math;
namespace physics {

	enum ReferenceFrame {
		RF_LOCAL,
		RF_GLOBAL
	};

	struct YAKE_PHYSICS_API Force
	{
		Vector3			force;
		real				duration;
		ReferenceFrame	frameType;

		Force() :
			duration(0)
		{}
		Force( const Vector3& f, const real t, const ReferenceFrame rf = RF_GLOBAL ) :
			force(f),
			duration(t),
			frameType(rf)
		{}

		Force& operator = (const Force& rhs)
		{
			if (&rhs == this)
				return *this;
			force = rhs.force;
			duration = rhs.duration;
			frameType = rhs.frameType;
			return *this;
		}
		Force& operator += (const Force& rhs)
		{
			force += rhs.force;
			duration += rhs.duration;
			return *this;
		}
		bool operator == (const Force& rhs)
		{
			return (frameType == rhs.frameType && force == rhs.force && duration == rhs.duration);
		}
	};

	class YAKE_PHYSICS_API IBodyListener
	{
	public:
		virtual ~IBodyListener() {}
		
		virtual void onSleeping(const bool sleeping) = 0;
	};
	class IBody;
	YAKE_PHYSICS_COMMON_POINTERS( IBody );

	class IActor;
	class YAKE_PHYSICS_API IBody : public ListenerManager<IBodyListener>
	{
	public:
		enum quantityType { QT_MASS, QT_DENSITY };
		struct Desc
		{
			Desc()
			{
				reset();
			}
			void reset()
			{
				linVelocity = Vector3::kZero;
				angVelocity = Vector3::kZero;
				massOffset = Vector3::kZero;
				totalMass = real(1.);
			}
			Vector3		linVelocity;
			Vector3		angVelocity;
			real		totalMass;
			Vector3		massOffset;
		};

		struct MassDesc
		{
			MassDesc( real massOrDensity, const Vector3& rOffset = Vector3::kZero, quantityType type = QT_MASS ) :
					quantity( massOrDensity ),
					offset( rOffset ),
					qType( type )
			{}

			virtual ~MassDesc() {}

			real 			quantity; /// For all of them. You can set total mass via setMass
			Vector3		offset; /// Mass can be offseted. TODO Add rotation "offset"?
			quantityType qType;
		};

		struct SphereMassDesc : public MassDesc
		{
			SphereMassDesc(	real radiusValue,
								real shapeDensity,
								const Vector3& rOffset = Vector3::kZero,
								quantityType type = QT_MASS ) :
									MassDesc( shapeDensity, rOffset, type ),
							 		radius( radiusValue )
			{}

			real radius;
		};

		struct BoxMassDesc : public MassDesc
		{
			BoxMassDesc(	real boxSizeX,
							real boxSizeY,
							real boxSizeZ,
							real shapeDensity,
							const Vector3& rOffset = Vector3::kZero,
							quantityType type = QT_MASS ) :
								MassDesc( shapeDensity, rOffset, type ),
								sizeX( boxSizeX ),
								sizeY( boxSizeY ),
								sizeZ( boxSizeZ )
			{}

			real	sizeX;
			real sizeY;
			real sizeZ;
		};

		struct CapsuleMassDesc : public MassDesc /// Corresponds to ODE CappedCylinder
		{
			CapsuleMassDesc(	real capRadius,
								real capLength,
								real shapeDensity,
								const Vector3& rOffset = Vector3::kZero,
								quantityType type = QT_MASS ) :
									MassDesc( shapeDensity, rOffset, type ),
									radius( capRadius ),
									length( capLength )
			{}

			real radius;
			real length;
		};

		struct CylinderMassDesc : public MassDesc
		{
			CylinderMassDesc(	real cylRadius,
								real cylLength,
								real shapeDensity,
								const Vector3& rOffset = Vector3::kZero,
								quantityType type = QT_MASS ) :
									MassDesc( shapeDensity, rOffset, type ),
									radius( cylRadius ),
									length( cylLength )
			{}

			real radius;
			real length;
		};

	public:
		virtual ~IBody() {}

		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(bool), OnSleeping )
	public:

		/** Returns a reference to the actor this body belongs to. A body always belongs to exactly
			one actor.
		*/
		virtual IActor& getActor() const = 0;

		/** Sets the mass of the body. The unit can be freely chosen by the application
			but should be consistent within the simulation, of course.
			@Remarks It may be that certain physics engine implementations restrict the freedom to choose a
			unit but up to now we have no knowledge of that.
		*/
		virtual void setMass( const real mass ) = 0;

		/** Sets the mass of the body based on appropriate MassDesc.
		*/
		virtual void setMass( const MassDesc& rDesc ) = 0;

		/** Adds the mass to the body based on appropriate MassDesc.
		*/
		virtual void addMass( const MassDesc& rDesc ) = 0;

		/** Return the mass of the body. The unit can be freely chosen by the application
			but should be consistent within the simulation, of course.
			@Remarks It may be that certain physics engine implementations restrict the freedom to choose a
			unit but up to now we have no knowledge of that.
		*/
		virtual real getMass() const = 0;

		/** Sets the linear velocity of the body/actor directly, i.e. with immediate effect.
		*/
		virtual void setLinearVelocity( const Vector3& rVelocity ) = 0;

		/** Returns the current velocity of the body.
		*/
		virtual Vector3 getLinearVelocity() const = 0;

		/** Sets the angular velocity [rad/s] of the body/actor directly, i.e. with immediate effect.
		*/
		virtual void setAngularVelocity( const Vector3& rVelocity ) = 0;

		/** Returns the angular velocity of the body in [rad/s].
		*/
		virtual Vector3 getAngularVelocity() const = 0;

		/** Adds a force. The specifics are detailed in the Force object.
		*/
		virtual void addForce( const Force& force ) = 0;

		/** Adds a force defined in the global reference frame. 
		*/
		virtual void addForce( const Vector3& rForce ) = 0;

		/** Adds a force defined in the global reference frame, acting at a position defined
			in the global reference frame.
		*/
		virtual void addForceAtPos( const Vector3& rForce, const Vector3& rPos ) = 0;

		/** Adds a force defined in the global reference frame, acting at a position defined
			in the local reference frame.
		*/
		virtual void addForceAtLocalPos( const Vector3& rForce, const Vector3& rPos ) = 0;

		/** Adds a force defined in the local reference frame.
		*/
		virtual void addLocalForce( const Vector3& rForce ) = 0;

		/** Adds a force defined in the local reference frame, acting at a position defined
			in the local reference frame (relative to the actor the body object belongs to).
		*/
		virtual void addLocalForceAtLocalPos( const Vector3& rForce, const Vector3& rPos ) = 0;

		/** Adds a force defined in the local reference frame, acting at a position defined
			in the global reference frame.
		*/
		virtual void addLocalForceAtPos( const Vector3& rForce, const Vector3& rPos ) = 0;

		/** Adds a torque defined in the global reference frame.
		*/
		virtual void addTorque( const Vector3& rTorque ) = 0;

		/** Adds a torque defined in the local reference frame.
		*/
		virtual void addLocalTorque( const Vector3& rTorque ) = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( IBody );

}
}
#endif
