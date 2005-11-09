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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakeShapeNx.h>
#include <yake/plugins/physicsNX/yakeActorNx.h>
#include <yake/plugins/physicsNX/yakePhysicsTrimeshNx.h>
#include <yake/plugins/physicsNX/yakeMaterialNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	ShapeNx::ShapeNx( NxActor* pActor, ActorNx* rActorNx, MaterialNx* pMaterial ) :
		mpNxShape(0),
		mType( ST_OTHER ),
		mNxActor( pActor ),
		mActor( rActorNx ),
		mTrimeshId( kTriangleMeshIdNone ),
		mMaterial( pMaterial )
	{
	}
	ShapeNx::~ShapeNx()
	{
		if (mpNxShape)
			detachAndDestroy_();
		mpNxShape = 0;
		if (mType == ST_TRIANGLE_MESH)
		{
			YAKE_ASSERT(mTrimeshId != kTriangleMeshIdNone);
		}
		if (mTrimeshId != kTriangleMeshIdNone)
			TrimeshNxManager::releaseMesh( mTrimeshId );
	}
	ShapeType ShapeNx::getType() const
	{ 
		return mType; 
	}
	void ShapeNx::setMaterial( IMaterial* pMaterial )
	{
		YAKE_ASSERT( pMaterial );
		if (!pMaterial)
			return;
		MaterialNx* pMatNx = static_cast<MaterialNx*>(pMaterial);
		YAKE_ASSERT( pMatNx );
		YAKE_ASSERT( mpNxShape );
		mpNxShape->setMaterial( pMatNx->_getNxMatIndex() );
	}
	void ShapeNx::attachAndCreate_(const NxShapeDesc & rkDesc)
	{
		YAKE_ASSERT( rkDesc.isValid() );
		YAKE_ASSERT( mNxActor );
		mpNxShape = mNxActor->createShape( rkDesc );
		YAKE_ASSERT( mpNxShape );

		YAKE_ASSERT( mMaterial ).warning("No material specified! Using default material!");
		if (mMaterial)
			mpNxShape->setMaterial( mMaterial->_getNxMatIndex() );
		else
			mpNxShape->setMaterial( 0 );

		switch (mType)
		{
		case ST_SPHERE:
			YAKE_ASSERT( mpNxShape->isSphere() );
			YAKE_ASSERT( mpNxShape->getType() == NX_SHAPE_SPHERE );
			break;
		case ST_BOX:
			YAKE_ASSERT( mpNxShape->isBox() );
			YAKE_ASSERT( mpNxShape->getType() == NX_SHAPE_BOX );
			break;
		default:
			break;
		};
	}
	void ShapeNx::detachAndDestroy_()
	{
		YAKE_ASSERT( mNxActor );
		YAKE_ASSERT( mActor );
		if (mNxActor->getNbShapes() == 1)
			mActor->onLastShapeIsAboutToDestroy();

		YAKE_ASSERT( mpNxShape );
		mNxActor->releaseShape( *mpNxShape );
		mpNxShape = 0;
		mType = ST_OTHER;
	}
	void ShapeNx::createAsSphere_( const real radius )
	{
		YAKE_ASSERT( !mpNxShape ).error("Shape was already finalized!"); // only as long this object isn't finalized!

		mType = ST_SPHERE;
		NxSphereShapeDesc desc;
		desc.radius = toNx( radius );
		YAKE_ASSERT( desc.isValid() );

		attachAndCreate_(desc);

		mProps["radius"] = radius;
	}
	void ShapeNx::createAsBox_( const Vector3 & rkDimensions )
	{
		YAKE_ASSERT( !mpNxShape ).error("Shape was already finalized!"); // only as long this object isn't finalized!

		mType = ST_BOX;
		NxBoxShapeDesc desc;
		desc.dimensions = toNx( rkDimensions );
		YAKE_ASSERT( desc.isValid() );

		attachAndCreate_(desc);

		mProps["dimensions"] = rkDimensions;
	}
	void ShapeNx::createAsPlane_( const Vector3 & rkNormal, const real d )
	{
		YAKE_ASSERT( !mpNxShape ).error("Shape was already finalized!"); // only as long this object isn't finalized!

		mType = ST_PLANE;
		NxPlaneShapeDesc desc;
		desc.normal = toNx( rkNormal );
		desc.d = toNx( d );
		YAKE_ASSERT( desc.isValid() );

		attachAndCreate_(desc);

		mProps["normal"] = rkNormal;
		mProps["d"] = d;
	}
	void ShapeNx::createAsCapsule_( const real height, const real radius )
	{
		YAKE_ASSERT( !mpNxShape ).error("Shape was already finalized!"); // only as long this object isn't finalized!

		mType = ST_CAPSULE;
		NxCapsuleShapeDesc desc;
		desc.height = toNx( height );
		desc.radius = toNx( radius );
		YAKE_ASSERT( desc.isValid() );

		attachAndCreate_(desc);

		mProps["height"] = height;
		mProps["radius"] = radius;
	}
	void ShapeNx::createAsMesh_( const TriMeshDesc& desc )
	{
		YAKE_ASSERT( !mpNxShape ).error("Shape was already finalized!"); // only as long this object isn't finalized!

		TriangleMeshId id = desc.trimeshId_;
		if (id == kTriangleMeshIdNone)
		{
			// create new
			TrimeshNxManager::createMesh( desc.trimesh_, id );
			YAKE_ASSERT( id != kTriangleMeshIdNone );
		}

		// now find existing
		NxTriangleMeshShapeDesc meshShapeDesc;
		meshShapeDesc.meshData = TrimeshNxManager::getById( id );
		YAKE_ASSERT( meshShapeDesc.isValid() );

		YAKE_ASSERT( meshShapeDesc.meshData );
		if (!meshShapeDesc.meshData)
			return;

		// finish
		mTrimeshId = id;
		mType = ST_TRIANGLE_MESH;
		attachAndCreate_(meshShapeDesc);
	}
	void ShapeNx::setPosition( const Vector3 & rkPosition )
	{
		YAKE_ASSERT( mpNxShape ).error("Cannot access property as long as object isn't finalized!");
		mpNxShape->setLocalPosition( toNx(rkPosition) );
	}
	Vector3 ShapeNx::getPosition() const
	{
		YAKE_ASSERT( mpNxShape ).error("Cannot access property as long as object isn't finalized!");
		return fromNx( mpNxShape->getLocalPosition() );
	}
	void ShapeNx::setOrientation( const Quaternion & rkOrientation )
	{
		YAKE_ASSERT( mpNxShape ).error("Cannot access property as long as object isn't finalized!");
		mpNxShape->setLocalOrientation( toNx(rkOrientation) );
	}
	Quaternion ShapeNx::getOrientation() const
	{
		YAKE_ASSERT( mpNxShape ).error("Cannot access property as long as object isn't finalized!");
		return fromNx( mpNxShape->getLocalOrientation() );
	}
	Vector3 ShapeNx::getPropertyVector3(const String& id) const
	{
		PropMap::const_iterator itFind = mProps.find( id );
		YAKE_ASSERT(itFind != mProps.end());
		if (itFind == mProps.end())
			return Vector3();
		return boost::any_cast<Vector3>(itFind->second);
	}
	real ShapeNx::getPropertyReal(const String& id) const
	{
		PropMap::const_iterator itFind = mProps.find( id );
		YAKE_ASSERT(itFind != mProps.end());
		if (itFind == mProps.end())
			return real(0.);
		return boost::any_cast<real>(itFind->second);
	}

}
}