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
#ifndef YAKE_PHYSICS_SHAPE_H
#define YAKE_PHYSICS_SHAPE_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

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
		ST_OTHER
	};

	class IMaterial;
	/** A shape is what we formerly called "collision geom" in yake::physics. */
	class YAKE_PHYSICS_API IShape : public Movable
	{
	public:
		struct Desc
		{
			Desc(	const ShapeType type_,
					const Vector3& rPosition = Vector3::kZero, 
					const Quaternion& rOrientation = Quaternion::kIdentity,
					const String& material_ = "" ) :
				type( type_ ),
				position( rPosition ),
				orientation( rOrientation ),
				material( material_ )
			{}

			virtual ~Desc() {}
			//virtual Desc* clone() const = 0;

			ShapeType	type; // superfluous as shape type is determined by dynamic_cast on Desc struct...
			Vector3		position;
			Quaternion	orientation;
			String		material;
		};
		struct SphereDesc : Desc
		{
			SphereDesc(	real radius_ = real(1.),
						// base class:
						const String& material_ = "",
						const Vector3& rPosition = Vector3::kZero, 
						const Quaternion& rOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_SPHERE, rPosition, rOrientation, material_ ),
				radius( radius_ )
			{}
			real		radius;
		};
		struct BoxDesc : Desc
		{
			BoxDesc(	const Vector3& rDimensions = Vector3(1,1,1),
						// base class:
						const String& material_ = "",
						const Vector3& rPosition = Vector3::kZero, 
						const Quaternion& rOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_BOX, rPosition, rOrientation, material_ ),
				dimensions( rDimensions )
			{}
			Vector3		dimensions;
		};
		struct PlaneDesc : Desc
		{
			PlaneDesc(	const Vector3& rNormal = Vector3(0,1,0),
						const real d_ = real(1.),
						// base class:
						const String& material_ = "",
						const Vector3& rPosition = Vector3::kZero, 
						const Quaternion& rOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_PLANE, rPosition, rOrientation, material_ ),
				normal( rNormal ),
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
						const String& material_ = "",
						const Vector3& rPosition = Vector3::kZero, 
						const Quaternion& rOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_CAPSULE, rPosition, rOrientation, material_ ),
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
							const String& material_ = "",
							const Vector3& rPosition = Vector3::kZero, 
							const Quaternion& rOrientation = Quaternion::kIdentity
						 ) :
				Desc( ST_TRIANGLE_MESH, rPosition, rOrientation, material_ ),
				meshId( meshId_ )
			{}
			TriangleMeshId	meshId;
		};
	public:
		virtual ~IShape() {}

		virtual ShapeType getType() const = 0;

		//virtual Vector3 getDerivedPosition() const = 0;
		//virtual Quaternion getDerivedOrientation() const = 0;

		virtual Vector3 getPropertyVector3(const String&) const = 0;
		virtual real getPropertyReal(const String&) const = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( IShape );

}
}
#endif
