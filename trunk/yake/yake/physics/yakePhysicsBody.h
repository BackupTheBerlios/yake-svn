/**------------------------------------------------------------------------------------
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
*/
#ifndef YAKE_PHYSICS_BODY_H
#define YAKE_PHYSICS_BODY_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace base::math;
namespace physics {

	class IDynamicActor;
	class IBody
	{
	public:
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
			MassDesc( real shapeDensity, const Vector3& rOffset = Vector3::kZero ) : 
					density( shapeDensity ), 
					offset( rOffset )
			{}
			
			virtual ~MassDesc() {}
			
			real 			density; /// For all of them. You can set total mass via setMass
			Vector3		offset; /// Mass can be offseted. TODO Add rotation "offset"?
		};
		
		struct SphereMassDesc : public MassDesc
		{
			SphereMassDesc(	real radiusValue,
								real shapeDensity, 
								const Vector3& rOffset = Vector3::kZero ) :
									MassDesc( shapeDensity, rOffset ),
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
							const Vector3& rOffset = Vector3::kZero  ) :
								MassDesc( shapeDensity, rOffset ),
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
								const Vector3& rOffset = Vector3::kZero ) :
									MassDesc( shapeDensity, rOffset ),
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
								const Vector3& rOffset = Vector3::kZero ) :
									MassDesc( shapeDensity, rOffset ),
									radius( cylRadius ),
									length( cylLength )
			{}
			
			real radius;
			real length;
		};

	public:
		virtual ~IBody() {}

		/** Returns a reference to the actor this body belongs to. A body always belongs to exactly
			one actor.
		*/
		virtual IDynamicActor& getActor() const = 0;

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

}
}
#endif