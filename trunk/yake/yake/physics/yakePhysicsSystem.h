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
#ifndef YAKE_PHYSICS_PHYSICSSYSTEM_H
#define YAKE_PHYSICS_PHYSICSSYSTEM_H

#include <yake/physics/yakePhysicsPrerequisites.h>

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
			+ BodyGroup:
				+ operator + (pBody)
				+ operator - (pBody)
			+ GravityZone/AffectorZone/Affectors
	*/

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
		virtual ~IMaterial() {}

		virtual void setRollingFriction(const real friction) = 0;
		//@todo virtual void setRestitution(const real ...) = 0;
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
		ST_TRANSFORM_CONTAINER,
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

	/** A shape is what we formerly called "collision geom" in yake::physics. */
	class IShape //: public IPropertyQueryHandler
	{
	public:
		struct Desc
		{
			Desc(	const ShapeType type_,
					const Vector3 & rkPosition = Vector3::kZero, 
					const Quaternion & rkOrientation = Quaternion::kIdentity,
					IMaterial* material_ = 0) :
				type( type_ ),
				position( rkPosition ),
				orientation( rkOrientation ),
				pMaterial( material_ )
			{}
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
		typedef Vector<Vector3> Vector3Vector;
		typedef Vector3Vector VertexVector;
		typedef Vector3Vector NormalVector;
		typedef uint32 IndexType;
		typedef Vector<IndexType> IndexVector;

		struct TriMeshDesc : Desc
		{
			TriMeshDesc(	const String & rkMeshFilename,
							// base class:
							const Vector3 & rkPosition = Vector3::kZero, 
							const Quaternion & rkOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_TRIANGLE_MESH, rkPosition, rkOrientation ),
				meshFilename( rkMeshFilename )
			{}
			TriMeshDesc(	const VertexVector & rkVertices,
							const IndexVector & rkIndices,
							const NormalVector & rkNormals = NormalVector(),
							// base class:
							const Vector3 & rkPosition = Vector3::kZero, 
							const Quaternion & rkOrientation = Quaternion::kIdentity
						) :
				Desc( ST_TRIANGLE_MESH, rkPosition, rkOrientation ),
				vertices( rkVertices ),
				indices( rkIndices ),
				normals( rkNormals )
			{
			}
			String			meshFilename;
			// or alternatively:
			VertexVector	vertices;
			NormalVector	normals;
			IndexVector		indices;
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

		virtual void setMass(const real mass) = 0;
		virtual real getMass() const = 0;

		//@todo could move this into IActor... maybe more appropriate there?
		virtual void setLinearVelocity(const Vector3 & rkVelocity) = 0;
		virtual Vector3 getLinearVelocity() const = 0;
		virtual void setAngularVelocity(const Vector3 & rkVelocity) = 0;
		virtual Vector3 getAngularVelocity() const = 0;

		//@todo addForce, addTorque, addForceAtRel, addRelForceAtRel ...
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

		virtual bool hasBody() const = 0;
		virtual bool createBody() = 0;
		virtual IBody* getBody() const = 0;

		virtual SharedPtr<IShape> createShape( const IShape::Desc & rkShapeDesc ) = 0;
		virtual void destroyShape( SharedPtr<IShape> & rShape ) = 0;
		virtual const Deque<IShape*> getShapes() const = 0;

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
				type(type_),
				actor1(rFirst),
				actor2(rSecond)
			{
			}
			JointType type; // not really needed...
			IActor& actor1;
			IActor& actor2;
		};
		struct DescFixed : DescBase
		{
			DescFixed(	IActor & rFirst,
						IActor & rSecond ) :
				DescBase(JT_FIXED,rFirst,rSecond)
			{}
		};
		struct DescHinge : DescBase
		{
			DescHinge(	IActor & rFirst,
						IActor & rSecond,
						const Vector3 & rkAxis0) :
				DescBase(JT_HINGE,rFirst,rSecond),
				axis0(rkAxis0)
			{}
			Vector3		axis0;
		};
		struct DescHinge2 : DescBase
		{
			DescHinge2(	IActor & rFirst,
						IActor & rSecond,
						const Vector3 & rkAxis0, 
						const Vector3 & rkAxis1) :
				DescBase(JT_HINGE2,rFirst,rSecond),
				axis0(rkAxis0),
				axis1(rkAxis1)
			{}
			Vector3		axis0;
			Vector3		axis1;
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

	class IAvatar;

	class IWorld// : public IPropertyQueryHandler
	{
	public:
		virtual ~IWorld() {}

		virtual SharedPtr<IJoint> createJoint( const IJoint::DescBase & rkJointDesc ) = 0;
		virtual SharedPtr<IActor> createActor( const IActor::Desc & rkActorDesc = IActor::Desc() ) = 0;
		/*
		virtual SharedPtr<IMaterial> createMaterial( cosnt IMaterial::Desc & rkMatDesc ) = 0;
		virtual SharedPtr<IAvatar> createAvatar( const IAvatar::Desc & rkAvatarDesc ) = 0;
		*/

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

