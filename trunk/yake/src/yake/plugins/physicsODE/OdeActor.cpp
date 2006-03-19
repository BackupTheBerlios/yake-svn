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
#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeJoint.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeActor.h>
#include <yake/plugins/physicsODE/OdeShapes.h>

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		//	OdeActor
		//-----------------------------------------------------

		//-----------------------------------------------------
		OdeActor::OdeActor( OdeWorld* pWorld, bool bDynamic ) :
	mOdeWorld(pWorld),
		mBody(0)
	{
		if (bDynamic)
		{
			mBody = new OdeBody( mOdeWorld, *this );
			YAKE_ASSERT( mBody ).error( "Failed to create body!" );
			mBody->setPosition( math::Vector3::kZero );
			mBody->setOrientation( math::Quaternion::kIdentity );
		}
		this->setPosition( math::Vector3::kZero );
		this->setOrientation( math::Quaternion::kIdentity );
	}

	//-----------------------------------------------------
	OdeActor::~OdeActor()
	{
		YAKE_SAFE_DELETE( mBody );
		// destroy all shapes
		mShapes.clear();
	}

	//-----------------------------------------------------
	IWorld* OdeActor::getCreator() const
	{
		return mOdeWorld;
	}

	//-----------------------------------------------------
	void OdeActor::setEnabled(const bool enabled)
	{
		YAKE_ASSERT( 0 && "NOT IMPLEMENTED" );
	}

	//-----------------------------------------------------
	bool OdeActor::isEnabled() const
	{
		return true;
	}

	//-----------------------------------------------------
	OdeGeom* OdeActor::createTransformGeom(	OdeGeom* pGeom,
		const math::Vector3& rOffset,
		const math::Quaternion& rRelOrientation )
	{
		OdeTransformGeom* pTrans = new OdeTransformGeom( mOdeWorld->_getOdeSpace(), this );
		pTrans->attachGeom( pGeom );

		pGeom->setPosition( rOffset );
		pGeom->setOrientation( rRelOrientation );

		return pTrans;
	}

	//-----------------------------------------------------
	OdeGeom* OdeActor::createShapeFromDesc( const IShape::Desc& rShapeDesc )
	{
		YAKE_LOG_INFORMATION( String( "OdeActor | creating geom. Position " ) << rShapeDesc.position  );
		const IShape::Desc* pShapeDesc = &( rShapeDesc );

		IMaterial* pMaterial = this->getCreator()->getMaterial( pShapeDesc->material );

		if ( !pMaterial )
			pMaterial = this->getCreator()->getMaterial( "default" );

		OdeMaterial* pOdeMaterial = pMaterial ? checked_cast<OdeMaterial*>( pMaterial ) : 0;
		YAKE_ASSERT( pOdeMaterial );

		OdeGeom* result = 0;

		if ( const IShape::SphereDesc* pSphereDesc = checked_cast<const IShape::SphereDesc*>( pShapeDesc ) )
		{
			OdeSphere* pSphere = new OdeSphere( mOdeWorld->_getOdeSpace(), this, pSphereDesc->radius );

			result = createTransformGeom( pSphere, rShapeDesc.position, rShapeDesc.orientation );
		}
		else if ( const IShape::BoxDesc* pBoxDesc = checked_cast<const IShape::BoxDesc*>( pShapeDesc ) )
		{
			OdeBox* pBox = new OdeBox( mOdeWorld->_getOdeSpace(), 
				this,
				pBoxDesc->dimensions.x, 
				pBoxDesc->dimensions.y,
				pBoxDesc->dimensions.z  );
			result = createTransformGeom( pBox, rShapeDesc.position, rShapeDesc.orientation );
		}
		else if ( const IShape::PlaneDesc* pPlaneDesc = checked_cast<const IShape::PlaneDesc*>( pShapeDesc ) )
		{
			/// We are not making transform geom here.
			/// Calculating absolute coordinates instead...
			math::Vector3 normal( pPlaneDesc->orientation*pPlaneDesc->normal );
			normal.normalise();

			real d = pPlaneDesc->d + normal.dotProduct( pPlaneDesc->position ); 

			OdePlane* pPlane = new OdePlane(	mOdeWorld->_getOdeSpace(), 
				this,
				normal.x,
				normal.y,
				normal.z,
				d );
			result = pPlane;
		}
		else if ( const IShape::CapsuleDesc* pCapsuleDesc = checked_cast<const IShape::CapsuleDesc*>( pShapeDesc ) )
		{
			OdeCCylinder* pCapsule = new OdeCCylinder( mOdeWorld->_getOdeSpace(), 
				this,
				pCapsuleDesc->radius,
				pCapsuleDesc->height  );
			result = createTransformGeom( pCapsule, rShapeDesc.position, rShapeDesc.orientation );
		}
		else if ( const IShape::TriMeshDesc* pTriMeshDesc = checked_cast<const IShape::TriMeshDesc*>( pShapeDesc ) )
		{
			TriangleMeshId id = pTriMeshDesc->trimeshId_;
			if (id == kTriangleMeshIdNone)
			{
				id = mOdeWorld->createTriangleMesh( pTriMeshDesc->trimesh_ );
				YAKE_ASSERT( id != kTriangleMeshIdNone );
			}
			OdeTriMesh::MeshData data = mOdeWorld->getMeshDataById( id );

			OdeTriMesh* pMesh = new OdeTriMesh( mOdeWorld->_getOdeSpace(), this, data.id );

			YAKE_ASSERT( pMesh ).error( "Mesh with such id wasn't found!" );

			result = createTransformGeom( pMesh, rShapeDesc.position, rShapeDesc.orientation );
		}

		YAKE_ASSERT( result != 0 ).error( "Unsupported shape type!" );

		/// setting material if any

		YAKE_ASSERT( pOdeMaterial );
		result->setMaterial( pOdeMaterial );

		if ( result )
		{
			mShapes.push_back( SharedPtr<OdeGeom>(result) );
			// plane is not a movable geom. Its position is determined on creation!
			// if (result->getType() != ST_PLANE)
			// {
			//     result->setPosition( pShapeDesc->position );
			//     result->setOrientation( pShapeDesc->orientation );
			// }
		}

		return result;
	}

	//-----------------------------------------------------
	bool operator == (const SharedPtr<OdeGeom>& lhs, const OdeGeom* rhs)
	{
		return (lhs.get() == rhs);
	}

	//-----------------------------------------------------
	void OdeActor::destroyShape( IShape* pShape )
	{
		OdeGeom* pGeom = checked_cast<OdeGeom*>( pShape );
		ShapeList::iterator victim = std::find( mShapes.begin(), mShapes.end(), pGeom );
		mShapes.erase( victim );
	}

	//-----------------------------------------------------
	IShapePtrList OdeActor::getShapes() const
	{
		IShapePtrList ret;
		//ret.reserve( mShapes.size() );
		//std::copy( mShapes.begin(), mShapes.end(), std::back_inserter( mShapes ) );
		for (ShapeList::const_iterator it = mShapes.begin(); it != mShapes.end(); ++it)
			ret.push_back( checked_cast<IShape*>(it->get()) );
		return ret;
	}

	//-----------------------------------------------------
	// 	void OdeActor::postStep( real timeElapsed )
	// 	{
	// 		Vector< OdeActor* > its;
	// 		for (CollisionList::iterator it = mCollisions.begin(); it != mCollisions.end(); ++it)
	// 		{
	// 			CollisionInfo& info = it->second;
	// 			info.time += timeElapsed;
	// 			if (info.time >= 0.1)
	// 			{
	// 				mLeaveCollisionSignal( this, it->first );
	// 				its.push_back( it->first );
	// 			}
	// 		}
	// 		VectorIterator< Vector< OdeActor* > > itErase(
	// 			its.begin(), its.end() );
	// 		while (itErase.hasMoreElements())
	// 		{
	// 			CollisionList::iterator itFind = mCollisions.find( itErase.getNext() );
	// 			if (itFind != mCollisions.end())
	// 				mCollisions.erase( itFind );
	// 		}
	// 		its.clear();
	// 	}

//#define DEBUG_COLLISIONS

	//-----------------------------------------------------
	void OdeActor::_collide( OdeActor* pOther, dGeomID geomA, dGeomID geomB, dJointGroup* contactJointGroup )
	{
		YAKE_ASSERT( pOther ).debug( "Need the other object participating in the collision!" );
		YAKE_ASSERT( contactJointGroup ).error( "Need a joint group for contact joints!" );

		// filtering out useless geom combinations...
		if ( ( dGeomGetClass( geomA ) == dRayClass ) || ( dGeomGetClass( geomB ) == dRayClass ) )
			return;

		OdeGeom* pShapeA = reinterpret_cast<OdeGeom*>( dGeomGetData( geomA ) );
		OdeGeom* pShapeB = reinterpret_cast<OdeGeom*>( dGeomGetData( geomB ) );
		YAKE_ASSERT( pShapeA != NULL && pShapeB != NULL ).error( "shapes are corrupt!" );

#ifdef DEBUG_COLLISIONS
		YAKE_LOG( "Actor collision function entered..." );
#endif 

		dContact contact[ MAX_CONTACTS ];

		// initiating collision detection
		int numCollided = dCollide( geomA, geomB, MAX_CONTACTS, &contact[0].geom, sizeof( dContact ) );
		if (numCollided == 0)
		{
#ifdef DEBUG_COLLISIONS
			YAKE_LOG( "No collisions..." );
#endif
			return;
		}

#ifdef DEBUG_COLLISIONS
		YAKE_LOG( "Firing collision signals" );
#endif 
		// collisions are fired...
		this->mEnterCollisionSignal( ActorCollisionInfo( this, pShapeA, pOther, pShapeB ) );
		pOther->mEnterCollisionSignal( ActorCollisionInfo( this, pShapeA, pOther, pShapeB ) );

		const OdeMaterial* pMatA = pShapeA->getMaterial();
		const OdeMaterial* pMatB = pShapeB->getMaterial();

		// if materials are not supposed to produce any contact forces
		// This is the case if we just want some collision detection and without
		// any contact joints...
		if ( pMatA == NULL || pMatB == NULL )
		{
#ifdef DEBUG_COLLISIONS
			YAKE_LOG( "collision detection without materials..." );
#endif
			return;
		}

		//HACK FIXME implement collision cache!!!
		// fire collision events
		//CollisionCache::iterator itFind = mCollisions.find( pOther );
		//	if ( itFind == mCollisions.end() )
		//	{
		//	    // new collision
		//	    CollisionInfo info = 0;
		//	    mCollisions.insert( CollisionCache::value_type( pOther, info ) );
		//
		//	    //mEnterCollisionSignal( this, pOther );
		//	}
		//	else
		//	{
		//	    // still colliding, so reset timeout
		//	    itFind->second = 0;
		//	}

		// calculate soft cfm parameter[0..10^-6] from softness [0..1]
		float softness = ( pMatA->mSoftness + pMatB->mSoftness ) / 1000000.0f;

#ifdef DEBUG_COLLISIONS
		YAKE_LOG( String("Collision: softness was set to ") << softness );
#endif

		// create contact joints
		for ( int i = 0; i < numCollided; ++i )
		{
			contact[i].surface.mode =
				dContactSoftERP
				|dContactSoftCFM
				|dContactBounce
				|dContactApprox1;

			// Setting contact softness...
			if ( softness > 0 )
			{
				contact[i].surface.mode |= dContactSoftCFM;
				contact[i].surface.soft_cfm = softness;
			}
			else
				contact[i].surface.soft_cfm = 0.;

			// determine surface friction coefficients
			// frictionCoefficient [0..1]
			real frictionCoefficient = std::min( pMatA->mFriction, pMatB->mFriction );

#ifdef DEBUG_COLLISIONS
			YAKE_LOG( String("Collision: frictionCoefficient was set to ") << frictionCoefficient );
#endif

			contact[i].surface.mu = frictionCoefficient; 

			float friction2 = std::min( pMatA->mFriction2, pMatB->mFriction2 ); 
			if ( friction2 > 0.f )
			{
				contact[i].surface.mode |= dContactMu2;
				contact[i].surface.mu2 = friction2; // 0 or 10
			}

			contact[i].surface.slip1 = 0.1; //FIXME need to set this right
			contact[i].surface.slip2 = 0.1; //FIXME need to set this right
			contact[i].surface.motion1 = 0; //
			contact[i].surface.motion2 = 0;

			//FIXME soft erp parameter should be correct
			contact[i].surface.soft_erp = 0.9; 

			real restitution = std::max( pMatA->mRestitution, pMatB->mRestitution );
			contact[i].surface.bounce = restitution; 

#ifdef DEBUG_COLLISIONS
			YAKE_LOG( String("Collision: restitution was set to ") << restitution );
#endif

			//FIXME bounce velocity needs to be tuned
			contact[i].surface.bounce_vel = 0.1;

			dJointID contactJointID = dJointCreateContact(	mOdeWorld->_getOdeID(),
				contactJointGroup->id(),
				contact + i );
			dJointAttach ( contactJointID, dGeomGetBody( geomA ), dGeomGetBody( geomB ) );
		} // for each contact
	}

	//-----------------------------------------------------
	IShape* OdeActor::createShape( const IShape::Desc& rShapeDesc, real massOrDensity, IBody::quantityType type )
	{
		if ( mBody && checked_cast<const IShape::PlaneDesc*>( &rShapeDesc ) )
			YAKE_ASSERT( false ).error( "Attempted to attach immovable plane shape to movable actor!" );

		OdeGeom* pShape = createShapeFromDesc( rShapeDesc );
		YAKE_ASSERT( pShape );
		YAKE_ASSERT( mBody || !(massOrDensity) ).warning( "Attempt to set mass on nonexistant body!" );

		if (mBody)
		{
			dGeomSetBody( pShape->_getOdeGeomID(), mBody->_getOdeBody()->id() );

			if (massOrDensity)
			{
				mBody->_applyMassDescFromShapeDesc( rShapeDesc, massOrDensity, type );
			}
		}

		return pShape;
	}

	//-----------------------------------------------------
	void OdeActor::setPosition( const math::Vector3& rPosition )
	{
		mPosition = rPosition;
		if (mBody)
			mBody->setPosition( rPosition );
		else
		{
			for( ShapeList::iterator i = mShapes.begin(); i != mShapes.end(); ++i )
			{
				i->get()->setPosition( mPosition );
			}
		}
	}

	//-----------------------------------------------------
	void OdeActor::setOrientation( const math::Quaternion& rOrientation )
	{
		mOrientation = rOrientation;

		if (mBody)
			mBody->setOrientation( rOrientation );
		else
		{
			for( ShapeList::iterator i = mShapes.begin(); i != mShapes.end(); ++i )
			{
				i->get()->setOrientation( mOrientation );
			}
		}
	}

	//-----------------------------------------------------
	math::Vector3 OdeActor::getPosition() const
	{
		if (mBody)
			return mBody->getPosition();

		return mPosition;
	}

	//-----------------------------------------------------
	math::Quaternion OdeActor::getOrientation() const
	{
		if (mBody)
			return mBody->getOrientation();

		return mOrientation;
	}

	//-----------------------------------------------------
	IBody& OdeActor::getBody() const
	{
		YAKE_ASSERT( mBody );
		return *mBody;
	}

	//-----------------------------------------------------
	IBody* OdeActor::getBodyPtr() const
	{
		return mBody;
	}

	} // physics
} // yake

