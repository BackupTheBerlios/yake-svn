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
*/
#ifndef YAKE_PHYSICSSYSTEM_H
#define YAKE_PHYSICSSYSTEM_H

#include <yake/physics/yakePhysics.h>

namespace yake {

	using namespace base::math;

	namespace physics {
		class IBody;
		class IJointGroup;
		class IWorld;
		class PhysicsSystem;

		/**
		*/
		class YAKE_PHYSICS_API PhysicsPlugin : public ::yake::base::Plugin
		{
		public:
			virtual PhysicsSystem* createSystem() = 0;
		};

		//-----------------------------------------------------
		// General physics objects
		//-----------------------------------------------------

		/** A group of bodies. Can be used for various purposes, e.g. for grouping
			bodies which are affected by BodyAffectors.
			\see BodyAffector
			\see IBody
		*/
		class BodyGroup
		{
		public:
			typedef std::list< IBody* > BodyList;
		protected:
			BodyList		mBodies;
		public:
			BodyGroup();
			~BodyGroup();

			/** Add a body to the group. */
			void add( IBody * pBody )
			{
				if (pBody)
					mBodies.push_back( pBody );
			}

			/** Remove a body from this group. */
			void remove( IBody * pBody )
			{
				BodyList::iterator itFind = std::find( mBodies.begin(), mBodies.end(), pBody );
				if (itFind != mBodies.end())
					mBodies.erase( itFind );
			}

			/** Return a const reference to the internal list of bodies. */
			const BodyList & getBodyList() const
			{ return mBodies; }
		};


		//-----------------------------------------------------
		// System interfaces
		//-----------------------------------------------------

		/** One IJoint interface to rule them all.
			Interface is remarkably similar to ODE's, isn't it ?
		*/
		class YAKE_PHYSICS_API IJoint
		{
		protected:
			IJoint() {}
			IJoint( const IJoint & );
		public:
			/** Type of joint.
			*/
			enum JointType {
				JT_BALL,
				JT_HINGE2,
				JT_HINGE,
				JT_FIXED
			};

			virtual ~IJoint() {}

			virtual JointType getType() const = 0;

			virtual void attach(IBody* body1, IBody* body2) = 0;

			virtual void setAnchor(const Vector3 & anchorPoint) = 0;
			virtual Vector3 getAnchor() const = 0;

/*			virtual void setAxis(int axis, const Vector3 & axisDirection) = 0;
			virtual Vector3 getAxis(int axis) const = 0;
			virtual int getAxisCount() const = 0;

			virtual void setMotorVelocity(int motor, real vel) = 0;
			virtual void setMotorMaximumForce(int motor, real force) = 0;
			virtual int getMotorCount() const = 0;*/

			virtual void setAxis1(const Vector3 & axis) = 0;
			virtual void setAxis2(const Vector3 & axis) = 0;
			virtual Vector3 getAxis1() const = 0;
			virtual Vector3 getAxis2() const = 0;

			virtual void setMotor1Velocity(real vel) = 0;
			virtual void setMotor1MaximumForce(real force) = 0;

			virtual void setMotor2Velocity(real vel) = 0;
			virtual void setMotor2MaximumForce(real force) = 0;

			virtual void setLowStop(real stop) = 0;
			virtual void setHighStop(real stop) = 0;

			virtual void setSpring(real spring) = 0;
			virtual void setDamping(real damping) = 0;
			virtual real getSpring() const = 0;
			virtual real getDamping() const = 0;
		};
		//-----------------------------------------------------

		/** IBody
			\remarks THE PHYSICS INTERFACES ARE TO BE OVERHAULED.
		*/
		class YAKE_PHYSICS_API IBody : public base::Movable
		{
		public:
			/** Type of mass of a body.*/
			enum MassType {
				MT_BOX,
				MT_SPHERE
			};

			virtual ~IBody() {}

			virtual void setLinearVelocity(const Vector3 & velocity) = 0;
			virtual Vector3 getLinearVelocity() const = 0;
			virtual void setAngularVelocity(const Vector3 & velocity) = 0;
			virtual Vector3 getAngularVelocity() const = 0;
			
			virtual void setMassBox(real lx, real ly, real lz, real density = 1.) = 0;
			virtual void setMassSphere(real r, real density = 1.) = 0;
			virtual void setMass(real mass) = 0;
			virtual real getMass() const = 0;
			virtual IBody::MassType getType() const = 0;
			virtual void translateMass( const Vector3 & d ) = 0;

			virtual void addForce( const Vector3 & force ) = 0;
			virtual void addRelForce( const Vector3 & force ) = 0;
			virtual void addTorque( const Vector3 & torque ) = 0;
			virtual void addRelTorque( const Vector3 & torque ) = 0;
			virtual void addForceAtPosition( const Vector3 & force, const Vector3 & position = Vector3(0,0,0) ) = 0;
			virtual void addRelForceAtRelPosition( const Vector3 & force, const Vector3 & relPosition = Vector3(0,0,0) ) = 0;

			virtual Vector3 getTorque() const = 0;

			virtual void setEnabled( bool enabled ) = 0;
		protected:
		};

		//-----------------------------------------------------
		class IAffector;
		class IComplexObject;
		class ICollisionGeometry;

		/** IWorld represents a physically simulated world.
			It is possible to have any number of worlds.
			\remarks THE PHYSICS INTERFACES ARE TO BE OVERHAULED.
		*/
		class YAKE_PHYSICS_API IWorld
		{
		protected:
			IWorld() {}
		public:
			virtual ~IWorld() {}

			virtual void update( const real timeElapsed ) = 0;

			/// Sets the global directional gravity acceleration. Every body
			/// in this world is affected by it!
			virtual void setGlobalGravity( const Vector3 & acceleration ) = 0;

			virtual IBody* createBody() = 0;
			virtual IJoint* createJoint(IJoint::JointType type, IJointGroup* group = 0) = 0;

			// dynamics objects
			virtual IComplexObject* createPlane(const Vector3 & n, real d) = 0;
			virtual IComplexObject* createSphere(real radius) = 0;
			virtual IComplexObject* createBox(real lx, real ly, real lz) = 0;
			virtual IComplexObject* createMesh(const base::String & mesh) = 0;
			virtual IComplexObject* createRay(const Vector3 & origin, const Quaternion & orientation) = 0;
			virtual IComplexObject* createEmptyPhysicsObject() = 0;

			// collision geometry
			virtual ICollisionGeometry* createCollisionGeomSphere(real radius) = 0;
			virtual ICollisionGeometry* createCollisionGeomBox(real lx, real ly, real lz) = 0;
			virtual ICollisionGeometry* createCollisionGeomMesh( const base::String & collisionMeshResourceName ) = 0;
		};

		/** Represents a collision geometry object which can be attached to
			a body.
			\remarks THE PHYSICS INTERFACES ARE TO BE OVERHAULED.
		*/
		class YAKE_PHYSICS_API ICollisionGeometry : public base::Movable
		{
		protected:
			ICollisionGeometry() {}
			ICollisionGeometry( const ICollisionGeometry & );
		public:
			/** Type of collision geometry. */
			enum CollisonGeomType {
				CGT_PLANE,
				CGT_SPHERE,
				CGT_BOX,
				CGT_CYLINDER,
				CGT_CCYLINDER,
				CGT_MESH
			};

			virtual ~ICollisionGeometry() {}
			
			virtual ICollisionGeometry::CollisonGeomType getType() const = 0;

			virtual Vector3 planeGetNormal() const = 0;
			virtual real planeGetDistance() const = 0;
			virtual real sphereGetRadius() const = 0;
			virtual Vector3 boxGetDimensions() const = 0;
			virtual base::String meshGetName() const = 0;
			virtual Vector3 rayGetOrigin() const = 0;
			virtual Quaternion rayGetOrientation() const = 0;
		};

		//-----------------------------------------------------
		class YAKE_PHYSICS_API DynamicsObjectListener
		{
		private:
			DynamicsObjectListener(const DynamicsObjectListener &);
		public:
			DynamicsObjectListener();

			virtual void prePhysics(real fTimeElapsed) = 0;
			virtual void postPhysics() = 0;
		};

		/** Represents a complex physics object.
			It can have a body (if it is dynamic) and collision
			geometry can be attached to it.
			Additionally one can control certain physical properties
			of the underlying body directly through this interface.
			\remarks THE PHYSICS INTERFACES ARE TO BE OVERHAULED.
		*/
		class YAKE_PHYSICS_API IComplexObject : public base::Movable
		{
		public:
			class YAKE_PHYSICS_API ISlipNormalSource
			{
			protected:
				ISlipNormalSource() {}
			public:
				virtual ~ISlipNormalSource() {}
				virtual Vector3 getLateralSlipNormal() const = 0;
			};
			virtual ~IComplexObject() {}

			virtual IBody* getBody() const = 0;
			virtual void setBody(IBody* body) = 0;

			virtual void addCollisionGeometry(ICollisionGeometry* geom) = 0;
			virtual base::templates::Vector<ICollisionGeometry*> getCollisionGeometryVector() const = 0;

			//virtual void addListener(CollidableListener* listener) = 0;

			// part of collidable or body and geom,too ?
			virtual void setFriction(real f) = 0;
			virtual void setFriction2(real f) = 0;
			virtual void setSoftness(real s) = 0;
			virtual real getFriction() const = 0;
			virtual real getFriction2() const = 0;
			virtual real getSoftness() const = 0;

			virtual void setLateralSlipEnabled( bool enabled ) = 0;
			//virtual void setSlipNormal( const Vector3 & slipNormal ) = 0;
			virtual void setSlipNormalSource( ISlipNormalSource* pSource ) = 0;
			virtual void setSlipCoefficients( const real linear, const real angular ) = 0;

		protected:
			typedef Signal1< void ( IComplexObject*, IComplexObject* ) > EnterCollisionSignal;
			typedef Signal1< void ( IComplexObject*, IComplexObject* ) > LeaveCollisionSignal;
		public:
			virtual void subscribeToEnterCollision( const EnterCollisionSignal::slot_type & slot ) = 0;
			virtual void subscribeToLeaveCollision( const LeaveCollisionSignal::slot_type & slot ) = 0;
		};

		/** PhysicsSystem.
			Manages creation of physical worlds and other administrative tasks.
		*/
		class YAKE_PHYSICS_API PhysicsSystem// : public System
		{
		public:
			virtual ~PhysicsSystem() {}

			virtual IWorld* createWorld() = 0;
		};

		//-----------------------------------------------------
		/*
		class ICollisionMesh
		{
		public:
			virtual ~ICollisionMesh();

			virtual base::String getName() = 0;
		};
		*/

		/**
		*/
		class YAKE_PHYSICS_API ICollisionTriMeshManager
		{
		public:
			virtual ICollisionGeometry* getByName(const base::String & name) = 0;
		};

	}
}

#endif

