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
#ifndef YAKE_PHYSICS_PHYSICSSYSTEM_H
#define YAKE_PHYSICS_PHYSICSSYSTEM_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

#ifndef YAKE_PHYSICS_AVATAR_H
#	include <yake/physics/yakePhysicsAvatar.h>
#endif

	/** The description structs are used for creating elements.

		Why is this necessary?
			Physics engines differ in the way they define and create physics objects.
			Often the order of setting parameters is crucial, e.g. in ODE as soon as a fixed joint
			is attached to two objects, the anchor point cannot be set again (workaround is destroying
			the joint, recreating it, setting the new anchor point and reattaching the objects).

			Furthermore we give the concrete implementation of yake::physics the freedom to
			use certain optimizations and at least avoid sub-optimal creation paths.

			Normally, most objects won't be massively modified once created, therefore this approach
			should work fine. Nevertheless modification is still possible. Depending on the restrictions
			of the underlying physics system modifications may be sub-optimal in certain situations (see
			the example of fixed joints in ODE above).
	*/

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace base::math;
namespace physics {

	/** @todo
			+ GravityZone/AffectorZone
	*/
	class IBody;

	class BodyGroup
	{
	public:
        typedef Vector<SharedPtr<IBody> > BodyVector;
		typedef BodyVector::iterator iterator;
		typedef BodyVector::const_iterator const_iterator;
		typedef BodyVector::value_type value_type;

		BodyGroup( const BodyGroup & rkBodyGroup );
		BodyGroup( const BodyVector & rkBodies );
		BodyGroup();
		~BodyGroup();

		BodyGroup& operator += (SharedPtr<IBody> & rBody);
		BodyGroup& operator -= (SharedPtr<IBody> & rBody);

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
	private:
		BodyVector	mBodies;
	};

	/* not used (yet?)
	enum PhysicsErrorCode
	{
		kPEC_NONE = 10000,
		kPEC_ERROR = 10001,
		kPEC_INVALID_AXIS = 10002,
		kPEC_INVALID_PARAMETER = 10003,
		kPEC_ALREADY_EXISTS = 10004
	};
	*/

	class IMaterial
	{
	public:
		struct Desc
		{
			Desc( real friction = 0.1, real restitution = 0.5, real staticFriction = 0.3, real rollingFriction = 0.2 ) :
				mFriction( friction ),
				mRestitution( restitution ),
				mStaticFriction( staticFriction ),
				mRollingFriction( rollingFriction )
			{}
			
			virtual ~Desc() {}
			
			real mFriction;
			real mRestitution;
			real mStaticFriction;
			real mRollingFriction;
		};

		virtual ~IMaterial() {}

		virtual void setRollingFriction(const real friction) = 0;
		virtual void setRestitution(const real restitution) = 0;
		virtual void setStaticFriction(const real friction) = 0;
		virtual void setStaticFrictionV(const real friction) = 0;
		virtual void setStaticFrictionVEnabled(bool enabled) = 0;

		//@todo add more properties that can quite easily be generalised...
	};

	/**
		YAKE         ODE          NX
		Ball         Ball         Spherical
		Hinge        Hinge        Revolute
		Fixed        Fixed        [simulated] (using Prismatic ?]
		Hinge2       Hinge2       ? (has to be simulated?)
		Slider       Slider       Cylindrical(?)(or Slider?) (Prismatic="single translational DOF", Cylindrical="sliding joint, permits one translational and one rotational DOF")
		Universal    Universal    ? (has to be simulated?)

	*/
	enum JointType
	{
		JT_BALL,
		JT_HINGE,
		JT_HINGE2,
		JT_FIXED,
		JT_SLIDER,
		JT_UNIVERSAL,
		JT_OTHER
	};

	/**
		YAKE         ODE               NX
		Plane        Plane             Plane
		Box          Box               Box
		Sphere       Sphere            Sphere
		Capsule      Capped Cylinder   Capsule
		Cylinder     ?                 ?           (could be approximated with a shorter capsule...)
		Tri Mesh     TriMesh           Mesh
		Voxel Set    ?                 ?
		Transform    ?                 ?           (could be simulated)
	*/
	enum ShapeType
	{
		ST_PLANE,
		ST_BOX,
		ST_SPHERE,
		ST_CAPSULE,
		ST_CYLINDER,
		ST_TRIANGLE_MESH,
		ST_VOXEL_SET,
		ST_TRANSFORM_CONTAINER, //@todo superfluous?
		ST_OTHER
	};

	struct Property
	{
		Property();
		Property( const String & n, const boost::any & v );

		String		name;
		boost::any	value;
	};
	typedef Deque<String> PropertyNameList;

	class YAKE_PHYSICS_API IPropertyQueryHandler
	{
	public:
		virtual ~IPropertyQueryHandler() {}
		virtual const PropertyNameList& getPropertyNames() const = 0;
		virtual void setProperty( const String & rkName, const boost::any & rkValue ) = 0;
	};

	typedef uint32 TriangleMeshId;
	const TriangleMeshId kTriangleMeshIdNone = 0xFFFFFFFF;
	struct TriangleMeshDesc
	{
		/** Important Notice: Vector3Vector, VertexVector, NormalVector and IndexVector HAVE to
			be vectors because they may be accessed like C arrays ( e.g. &arr[0] ).
			Is this OK? Today, it's practically OK and tomorrow it'll be even more as it's part
			of the first Technical Corrigendum.
		*/
		typedef Vector<Vector3> Vector3Vector;
		typedef Vector3Vector VertexVector;
		typedef Vector3Vector NormalVector;
		typedef uint32 IndexType;
		typedef Vector<IndexType> IndexVector;

		TriangleMeshDesc()
		{
		}
		TriangleMeshDesc(	const VertexVector & rkVertices,
							const IndexVector & rkIndices ) :
				vertices( rkVertices ),
				indices( rkIndices )
		{
		}
		TriangleMeshDesc(	const VertexVector & rkVertices,
							const IndexVector & rkIndices,
							const NormalVector & rkNormals ) :
				vertices( rkVertices ),
				indices( rkIndices ),
				normals( rkNormals )
		{
		}
		VertexVector	vertices;
		NormalVector	normals;
		IndexVector		indices;
	};

	/** A shape is what we formerly called "collision geom" in yake::physics. */
	class IShape //: public IPropertyQueryHandler
	{
	public:
		struct Desc
		{
			Desc(	const ShapeType type_,
					const Vector3 & rkPosition = Vector3::kZero, 
					const Quaternion & rkOrientation = Quaternion::kIdentity,
					IMaterial* material_ = 0 ) :
				type( type_ ),
				position( rkPosition ),
				orientation( rkOrientation ),
				pMaterial( material_ )
			{}

			virtual ~Desc() {}

			ShapeType	type; // superfluous as shape type is determined by dynamic_cast on Desc struct...
			Vector3		position;
			Quaternion	orientation;
			IMaterial*	pMaterial;
		};
		struct SphereDesc : Desc
		{
			SphereDesc(	real radius_ = real(1.),
						// base class:
						const Vector3 & rkPosition = Vector3::kZero, 
						const Quaternion & rkOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_SPHERE, rkPosition, rkOrientation ),
				radius( radius_ )
			{}
			real		radius;
		};
		struct BoxDesc : Desc
		{
			BoxDesc(	const Vector3 & rkDimensions = Vector3(1,1,1),
						// base class:
						const Vector3 & rkPosition = Vector3::kZero, 
						const Quaternion & rkOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_BOX, rkPosition, rkOrientation ),
				dimensions( rkDimensions )
			{}
			Vector3		dimensions;
		};
		struct PlaneDesc : Desc
		{
			PlaneDesc(	const Vector3 & rkNormal = Vector3(0,1,0),
						const real d_ = real(1.),
						// base class:
						const Vector3 & rkPosition = Vector3::kZero, 
						const Quaternion & rkOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_PLANE, rkPosition, rkOrientation ),
				normal( rkNormal ),
				d( d_ )
			{}
			Vector3		normal;
			real		d;
		};
		struct CapsuleDesc : Desc
		{
			CapsuleDesc(const real height_ = real(2.),
						const real radius_ = real(1.),
						// base class:
						const Vector3 & rkPosition = Vector3::kZero, 
						const Quaternion & rkOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_CAPSULE, rkPosition, rkOrientation ),
				height( height_ ),
				radius( radius_ )
			{}
			real		height;
			real		radius;
		};

		struct TriMeshDesc : Desc
		{
			TriMeshDesc(	const TriangleMeshId meshId_,
							// base class:
							const Vector3 & rkPosition = Vector3::kZero, 
							const Quaternion & rkOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_TRIANGLE_MESH, rkPosition, rkOrientation ),
				meshId( meshId_ )
			{}
			TriangleMeshId	meshId;
		};
	public:
		virtual ~IShape() {}

		virtual ShapeType getType() const = 0;
	};

	/** "Static" shapes (IShape) can be positioned and rotated at creation time only.
		Movable shapes on the other hand can be moved and rotated at any time.

		If they are attached to an Actor, position and orientation are relative to their parent
		(usually the Actor or a special Transform Shape (ST_TRANSFORM_CONTAINER)).
	*/
	class IMovableShape : public IShape, public base::Movable
	{
	};

	class IActor;
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
	public:
		virtual ~IBody() {}

		/** Returns a reference to the actor this body belongs to. A body always belongs to exactly
			one actor.
		*/
		virtual IActor& getActor() const = 0;

		/** Sets the mass of the body. The unit can be freely chosen by the application
			but should be consistent within the simulation, of course.
			@Remarks It may be that certain physics engine implementations restrict the freedom to choose a
			unit but up to now we have no knowledge of that.
		*/
		virtual void setMass(const real mass) = 0;

		/** Return the mass of the body. The unit can be freely chosen by the application
			but should be consistent within the simulation, of course.
			@Remarks It may be that certain physics engine implementations restrict the freedom to choose a
			unit but up to now we have no knowledge of that.
		*/
		virtual real getMass() const = 0;

		/** Sets the linear velocity of the body/actor directly, i.e. with immediate effect. 
		*/
		virtual void setLinearVelocity(const Vector3 & rkVelocity) = 0;

		/** Returns the current velocity of the body.
		*/
		virtual Vector3 getLinearVelocity() const = 0;

		/** Sets the angular velocity [rad/s] of the body/actor directly, i.e. with immediate effect. 
		*/
		virtual void setAngularVelocity(const Vector3 & rkVelocity) = 0;

		/** Returns the angular velocity of the body in [rad/s].
		*/
		virtual Vector3 getAngularVelocity() const = 0;

		/** Adds a force defined in the global reference frame. 
		*/
		virtual void addForce( const Vector3 & rkForce ) = 0;

		/** Adds a force defined in the global reference frame, acting at a position defined
			in the global reference frame. 
		*/
		virtual void addForceAtPos( const Vector3 & rkForce, const Vector3 & rkPos ) = 0;

		/** Adds a force defined in the local reference frame.
		*/
		virtual void addLocalForce( const Vector3 & rkForce ) = 0;

		/** Adds a force defined in the local reference frame, acting at a position defined
			in the local reference frame (relative to the actor the body object belongs to).
		*/
		virtual void addLocalForceAtLocalPos( const Vector3 & rkForce, const Vector3 & rkPos ) = 0;

		/** Adds a force defined in the local reference frame, acting at a position defined
			in the global reference frame. 
		*/
		virtual void addLocalForceAtPos( const Vector3 & rkForce, const Vector3 & rkPos ) = 0;

		/** Adds a torque defined in the global reference frame. 
		*/
		virtual void addTorque( const Vector3 & rkTorque ) = 0;

		/** Adds a torque defined in the local reference frame. 
		*/
		virtual void addLocalTorque( const Vector3 & rkTorque ) = 0;
	};

	class IActor : public Movable
	{
	public:
		struct Desc
		{
			Deque< SharedPtr<IShape::Desc> >	shapes;
		};
	public:
		virtual ~IActor() {}

		typedef Vector<IShape*> ShapePtrVector;

		virtual bool hasBody() const = 0;
		virtual bool createBody() = 0;
		virtual IBody* getBody() const = 0;

		virtual SharedPtr<IShape> createShape( const IShape::Desc & rkShapeDesc ) = 0;
		virtual void destroyShape( SharedPtr<IShape> & rShape ) = 0;
		virtual const ShapePtrVector getShapes() const = 0;

		//virtual void updateMassFromShapes() = 0;

		typedef Signal0<void> SignalCollisionEntered;
		virtual void subscribeToCollisionEnteredSignal( const SignalCollisionEntered::slot_type & rkSlot ) = 0;
		virtual void subscribeToCollisionExitedSignal( const SignalCollisionEntered::slot_type & rkSlot ) = 0;
	};

	class IJoint
	{
	public:
		struct DescBase
		{
			DescBase(	const JointType type_, 
						IActor & rFirst,
						IActor & rSecond ) :
				type( type_ ),
				actor1( rFirst ),
				actor2( rSecond )
			{
			}

			virtual ~DescBase() {}

			JointType type; // not really needed...
			IActor& actor1;
			IActor& actor2;
		};

		struct DescFixed : DescBase
		{
			DescFixed(	IActor & rFirst,
						IActor & rSecond ) :
				DescBase( JT_FIXED,rFirst,rSecond )
			{}
		};

		struct DescHinge : DescBase
		{
			DescHinge(	IActor & rFirst,
						IActor & rSecond,
						const Vector3 & rAxis0,
						const Vector3 & rAnchor ) :
				DescBase( JT_HINGE,rFirst,rSecond ),
				axis0( rAxis0 ),
				anchor( rAnchor )
			{}
			Vector3		axis0;
			Vector3		anchor;
		};

		struct DescHinge2 : DescBase
		{
			DescHinge2(	IActor & rFirst,
						IActor & rSecond,
						const Vector3 & rAxis0, 
						const Vector3 & rAxis1,
						const Vector3 & rAnchor ) :
				DescBase( JT_HINGE2,rFirst,rSecond ),
				axis0( rAxis0 ),
				axis1( rAxis1 ),
				anchor( rAnchor )
			{}
			Vector3		axis0;
			Vector3		axis1;
			Vector3		anchor;
		};

		struct DescBall : DescBase
		{
			DescBall(	IActor& rFirst,
						IActor& rSecond,
						Vector3 const& rAnchor ) :
				DescBase( JT_BALL, rFirst, rSecond ),
				anchor( rAnchor )
			{}
			Vector3		anchor;
		};
	
		struct DescSlider : DescBase
		{
			DescSlider(	IActor& rFirst,
						IActor& rSecond,
						Vector3 const& rAxis ) :
				DescBase( JT_SLIDER, rFirst, rSecond ),
				axis( rAxis )
			{}
			Vector3		axis;
		};
	
		struct DescUniversal : DescBase
		{
			DescUniversal(	IActor& rFirst,
							IActor& rSecond,
							Vector3 const& rAxis0, 
							Vector3 const& rAxis1,
							Vector3 const& rAnchor ) :
				DescBase( JT_UNIVERSAL, rFirst, rSecond ),
				axis0( rAxis0 ),
				axis1( rAxis1 ),
				anchor( rAnchor )
 			{}
 			Vector3		axis0;
 			Vector3		axis1;
			Vector3		anchor;
 		};
	public:
		virtual ~IJoint() {}

		virtual IActor& getAttachedActor(bool bFirst) = 0;

		virtual JointType getType() const = 0;
		virtual size_t getNumAxis() const = 0;
		virtual void setAxis(size_t axisIndex, const Vector3 & rkAxis) = 0;
		virtual size_t getNumAnchors() const = 0;
		virtual void setAnchor(size_t anchorIndex, const Vector3 & rkAnchor) = 0;
		virtual void setMotor(size_t axisIndex, real velocityTarget, real maximumForce) = 0;
		virtual void setLimits(size_t axisIndex, real low, real high) = 0;
		virtual void setBreakable(bool enabled) = 0;
		virtual void setBreakableForce(real force) = 0;
		virtual void setBreakableTorque(real torque) = 0;
		virtual void setConnectedBodiesCollide(bool enabled) = 0;
		///setLow/HiStop
		//setSpring/Damper
	};

	class IWorld// : public IPropertyQueryHandler
	{
	public:
		virtual ~IWorld() {}

		virtual SharedPtr<IJoint> createJoint( const IJoint::DescBase & rkJointDesc ) = 0;
		virtual SharedPtr<IActor> createActor( const IActor::Desc & rkActorDesc = IActor::Desc() ) = 0;
		virtual SharedPtr<IAvatar> createAvatar( const IAvatar::Desc & rkAvatarDesc ) = 0;
		virtual SharedPtr<IMaterial> createMaterial( const IMaterial::Desc & rkMatDesc ) = 0;

		virtual TriangleMeshId createTriangleMesh( const TriangleMeshDesc & rkTrimeshDesc ) = 0;

		virtual Deque<ShapeType> getSupportedShapes(bool bStatic = true, bool bDynamic = true) const = 0;
		virtual Deque<JointType> getSupportedJoints() const = 0;
		virtual Deque<String> getSupportedSolvers() const = 0;
		virtual bool useSolver( const String & rkSolver ) = 0;
		virtual String getCurrentSolver() const = 0;
		virtual const PropertyNameList& getCurrentSolverParams() const = 0;
		virtual void setCurrentSolverParam( const String & rkName, const boost::any & rkValue ) = 0;

		virtual void step(const real timeElapsed) = 0;
	};

	class YAKE_PHYSICS_API IPhysicsSystem
	{
		YAKE_DECLARE_REGISTRY_0( IPhysicsSystem, base::String );
	public:
		virtual ~IPhysicsSystem() {}
		virtual SharedPtr<IWorld> createWorld() = 0;
	};

}
}

#endif

