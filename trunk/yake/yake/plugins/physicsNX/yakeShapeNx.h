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
#ifndef YAKE_PHYSICS_SHAPENX_H
#define YAKE_PHYSICS_SHAPENX_H

namespace yake {
namespace physics {

	class ActorNx;
	class MaterialNx;
	class ShapeNx : public IMovableShape
	{
	public:
		ShapeNx( NxActor & rActor, ActorNx & rActorNx, MaterialNx* pMaterial );
		virtual ~ShapeNx();

		virtual ShapeType getType() const;

		virtual void setPosition( const Vector3 & rkPosition );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & rkOrientation );
		virtual Quaternion getOrientation() const;

		// helpers

		void createAsSphere_( const real radius = real(1.) );
		void createAsBox_( const Vector3 & rkDimensions );
		void createAsPlane_( const Vector3 & rkNormal, const real d );
		void createAsCapsule_( const real height, const real radius );
		void createAsMesh_( const TriangleMeshId id );

	private:
		void attachAndCreate_(const NxShapeDesc & rkDesc);
		void detachAndDestroy_();
	private:
		ShapeType		mType;

		NxShape*		mpNxShape;

		NxActor&		mNxActor;
		ActorNx&		mActor;

		TriangleMeshId	mTrimeshId;

		MaterialNx*		mMaterial;
	};

}
}

#endif