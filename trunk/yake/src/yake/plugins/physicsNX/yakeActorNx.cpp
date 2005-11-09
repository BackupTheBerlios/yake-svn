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
#include <yake/plugins/physicsNX/yakeBodyNx.h>
#include <yake/plugins/physicsNX/yakeActorNx.h>
#include <yake/plugins/physicsNX/yakeWorldNx.h>
#include <yake/plugins/physicsNX/yakeMaterialNx.h>

namespace yake {
	using namespace base;
	using namespace math;
namespace physics {

	ActorNx::ActorNx( NxScene & rScene, WorldNx& rWorld, bool bDynamic ) : 
		mpNxActor(0), 
		mNxScene(rScene),
		mWorld(rWorld),
		mpBody( 0 ),
		mpNxDefaultShape( 0 )
	{
		// create Nx actor

//WORKAROUND: Nx actors need at least one shape.
#define EMPTY_ACTOR_WORKAROUND

#ifdef  EMPTY_ACTOR_WORKAROUND
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions		= NxVec3(float(0.1), float(0.1), float(0.1));
		YAKE_ASSERT( boxDesc.isValid() );
#endif

		NxBodyDesc bodyDesc;
		bodyDesc.angularDamping	= 0.5f;
		bodyDesc.mass = 1;
		bodyDesc.massSpaceInertia.magnitude();
		//bodyDesc.maxAngularVelocity	= 10.0f;
		YAKE_ASSERT( bodyDesc.isValid() ).debug("invalid NX body desc");

		NxActorDesc actorDesc;
		actorDesc.userData		= 0;
#ifdef  EMPTY_ACTOR_WORKAROUND
		actorDesc.shapes.pushBack(&boxDesc);
#endif
		actorDesc.body			= bDynamic ? &bodyDesc : 0;
#if !defined(EMPTY_ACTOR_WORKAROUND)
		actorDesc.flags			|= NX_ADT_SHAPELESS;
#endif
		actorDesc.density		= bDynamic ? 0 : 10.0f;
		actorDesc.globalPose.t  = NxVec3(0,0,0);
		YAKE_ASSERT( actorDesc.isValid() ).debug("invalid NX actor desc");

		mpNxActor = mNxScene.createActor( actorDesc );
		YAKE_ASSERT( mpNxActor );

#ifdef  EMPTY_ACTOR_WORKAROUND
		mpNxDefaultShape = mpNxActor->getShapes()[0];
		YAKE_ASSERT( mpNxDefaultShape );
#endif

		mWorld.regActor_(mpNxActor,this);

		// create the body
		YAKE_ASSERT( mpNxActor ).error("Actor is invalid! Cannot create body without a valid actor.");

		if (bDynamic)
		{
			mpBody = new BodyNx( mpNxActor, this );
			YAKE_ASSERT( mpBody );
		}
	}
	ActorNx::~ActorNx()
	{
		mWorld.unregActor_(mpNxActor);

		YAKE_SAFE_DELETE( mpBody );

		mShapes.clear(); // destroy them before the Nx actor is destroyed!
		if (mpNxActor)
			mNxScene.releaseActor( *mpNxActor );
		mpNxActor = 0;
		mpNxDefaultShape = 0;
	}
	IWorld* ActorNx::getCreator() const
	{
		return &mWorld;
	}
	void ActorNx::setEnabled(const bool enabled)
	{
		YAKE_ASSERT( 0 && "NOT IMPLEMENTED!" );
	}
	bool ActorNx::isEnabled() const
	{
		return true;
	}
	IBody& ActorNx::getBody() const
	{
		YAKE_ASSERT( mpBody );
		return *mpBody;
	}
	IBody* ActorNx::getBodyPtr() const
	{
		return mpBody;
	}
	void ActorNx::destroyShape( IShape* pShape )
	{
		YAKE_ASSERT( pShape );
		if (pShape)
			delete pShape;
	}
	void ActorNx::onLastShapeIsAboutToDestroy()
	{
		//WORKAROUND: (we need at least one shape per actor)
		YAKE_ASSERT( ! mpNxDefaultShape );
		if (mpNxDefaultShape)
			return;
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions = NxVec3(float(0.1), float(0.1), float(0.1));
		YAKE_ASSERT( boxDesc.isValid() );
		mpNxDefaultShape = mpNxActor->createShape( boxDesc );
		YAKE_ASSERT( mpNxDefaultShape );
	}
	IShapePtrList ActorNx::getShapes() const
	{
		IShapePtrList out;
		ConstDequeIterator<Deque<SharedPtr<ShapeNx> > > it(mShapes.begin(), mShapes.end());
		while (it.hasMoreElements())
		{
			out.push_back( it.getNext().get() );
		}
		return out;
	}
	IShape* ActorNx::createShape( const IShape::Desc & rkShapeDesc, real massOrDensity, IBody::quantityType type )
	{
		YAKE_ASSERT( mpNxActor ).error("Actor is invalid! Cannot create shape without a valid actor.");
		if (!mpNxActor)
			return 0;

		MaterialNx* pMaterial = static_cast<MaterialNx*>(this->mWorld.getMaterial(rkShapeDesc.material));
		YAKE_ASSERT( pMaterial ).warning("No material specified => using default material!");

		ShapeNx* pShape = new ShapeNx( mpNxActor, this, pMaterial );
		YAKE_ASSERT( pShape );

		switch (rkShapeDesc.type)
		{
		case ST_SPHERE:
			{
				const IShape::SphereDesc& rkSphere = static_cast<const IShape::SphereDesc&>(rkShapeDesc);
				pShape->createAsSphere_( rkSphere.radius );
			}
			break;
		case ST_BOX:
			{
				const IShape::BoxDesc& rkBox = static_cast<const IShape::BoxDesc&>(rkShapeDesc);
				pShape->createAsBox_( rkBox.dimensions );
			}
			break;
		case ST_PLANE:
			{
				const IShape::PlaneDesc& rkPlane = static_cast<const IShape::PlaneDesc&>(rkShapeDesc);
				pShape->createAsPlane_( rkPlane.normal, rkPlane.d );
			}
			break;
		case ST_CAPSULE:
			{
				const IShape::CapsuleDesc& rkCapsule = static_cast<const IShape::CapsuleDesc&>(rkShapeDesc);
				pShape->createAsCapsule_( rkCapsule.height, rkCapsule.radius );
			}
			break;
		case ST_TRIANGLE_MESH:
			{
				const IShape::TriMeshDesc& rkTrimesh = static_cast<const IShape::TriMeshDesc&>(rkShapeDesc);
				pShape->createAsMesh_( rkTrimesh );
			}
			break;
		default:
			YAKE_ASSERT( 1==0 )(rkShapeDesc.type).error("Unsupported shape type!");
			delete pShape;
			pShape = 0;
			return 0;
		}
		mShapes.push_back( SharedPtr<ShapeNx>(pShape) );

		pShape->setPosition( rkShapeDesc.position );
		pShape->setOrientation( rkShapeDesc.orientation );

		//WORKAROUND: now remove the work-a-round default shape
		if (mpNxDefaultShape)
		{
			mpNxActor->releaseShape( *mpNxDefaultShape );
			mpNxDefaultShape = 0;
		}

		YAKE_ASSERT( mpBody || !(massOrDensity) ).warning( "Attempt to set mass on nonexistant body!" );

		if (mpBody)
		{
			if (type == IBody::QT_MASS)
				mpNxActor->updateMassFromShapes( 0.0f, mpBody->getMass() + massOrDensity );
			else
			{
				YAKE_LOG_WARNING( "Adding density in Novodex doesn't work very well." );
				mpNxActor->updateMassFromShapes(  massOrDensity, 0.0f );
			}
		}

		return mShapes.back().get();
	}
	void ActorNx::setPosition( const Vector3 & rkPosition )
	{
		YAKE_ASSERT( mpNxActor ).error("Cannot access property as long as object isn't finalized!");
		mpNxActor->setGlobalPosition( toNx( rkPosition ) );
	}
	Vector3 ActorNx::getPosition() const
	{
		YAKE_ASSERT( mpNxActor ).error("Cannot access property as long as object isn't finalized!");
		return fromNx( mpNxActor->getGlobalPosition() );
	}
	void ActorNx::setOrientation( const Quaternion & rkOrientation )
	{
		YAKE_ASSERT( mpNxActor ).error("Cannot access property as long as object isn't finalized!");
		mpNxActor->setGlobalOrientationQuat( toNx( rkOrientation ) );
	}
	Quaternion ActorNx::getOrientation() const
	{
		YAKE_ASSERT( mpNxActor ).error("Cannot access property as long as object isn't finalized!");
		return fromNx( mpNxActor->getGlobalOrientationQuat() );
	}

}
}