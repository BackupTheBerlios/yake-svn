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
	OdeActor::OdeShapesMap	OdeActor::mOdeShapes; 

	//-----------------------------------------------------
	OdeActor::OdeActor( OdeWorld* pWorld ) :
				mOdeWorld( pWorld )
	{
	}

	//-----------------------------------------------------
	OdeActor::~OdeActor()
	{
		// destroy all shapes
		for( IActor::ShapePtrVector::iterator victim = mShapes.begin(); victim != mShapes.end(); ++victim )
		{
			YAKE_SAFE_DELETE( *victim );
		}
	}
	
	//-----------------------------------------------------
	OdeGeom* OdeActor::createTransformGeomIfNeeded(	OdeGeom* pGeom,
														const Vector3& rOffset,
														const Quaternion& rRelOrientation )
	{
		if ( rOffset == Vector3::kZero && rRelOrientation == Quaternion::kIdentity )
			return pGeom;
			
		OdeTransformGeom* pTrans = new OdeTransformGeom( mOdeWorld->_getOdeSpace() );
		pTrans->attachGeom( pGeom );
		
		pTrans->setPosition( rOffset );
		pTrans->setOrientation( rRelOrientation );
		
		return pTrans;
	}

	//-----------------------------------------------------
	IShape* OdeActor::createShapeFromDesc( IShape::Desc const& rShapeDesc )
	{
		IShape::Desc* pShapeDesc = &const_cast<IShape::Desc&>( rShapeDesc );
		
		IMaterial* pMaterial = pShapeDesc->pMaterial;
		OdeMaterial* pOdeMaterial = NULL;
		
		if ( pMaterial != NULL )
		{
			pOdeMaterial = dynamic_cast<OdeMaterial*>( pMaterial );
		}
	
		OdeGeom* result = NULL;
		
		if ( IShape::SphereDesc* pSphereDesc = dynamic_cast<IShape::SphereDesc*>( pShapeDesc ) )
		{
			OdeSphere* pSphere = new OdeSphere( mOdeWorld->_getOdeSpace(), pSphereDesc->radius );
			
			result = createTransformGeomIfNeeded( pSphere, rShapeDesc.position, rShapeDesc.orientation );
		}
		else if ( IShape::BoxDesc* pBoxDesc = dynamic_cast<IShape::BoxDesc*>( pShapeDesc ) )
		{
			OdeBox* pBox = new OdeBox( mOdeWorld->_getOdeSpace(), 
										pBoxDesc->dimensions.x, 
										pBoxDesc->dimensions.y,
										pBoxDesc->dimensions.z  );
			result = createTransformGeomIfNeeded( pBox, rShapeDesc.position, rShapeDesc.orientation );
		}
		else if ( IShape::PlaneDesc* pPlaneDesc = dynamic_cast<IShape::PlaneDesc*>( pShapeDesc ) )
		{
			/// We are not making transform geom here.
			/// Calculating absolute coordinates instead...
			Vector3 normal( pPlaneDesc->orientation*pPlaneDesc->normal );
			
			normal.normalise();
			
 			real d = pPlaneDesc->d + normal.dotProduct( pPlaneDesc->position ); 
			
			OdePlane* pPlane = new OdePlane(	mOdeWorld->_getOdeSpace(), 
												normal.x,
												normal.y,
												normal.z,
												d );
			result = pPlane;
		}
		else if ( IShape::CapsuleDesc* pCapsuleDesc = dynamic_cast<IShape::CapsuleDesc*>( pShapeDesc ) )
		{
			OdeCCylinder* pCapsule = new OdeCCylinder( mOdeWorld->_getOdeSpace(), 
										pCapsuleDesc->radius,
										pCapsuleDesc->height  );
			result = createTransformGeomIfNeeded( pCapsule, rShapeDesc.position, rShapeDesc.orientation );
		}
 		else if ( IShape::TriMeshDesc* pTriMeshDesc = dynamic_cast<IShape::TriMeshDesc*>( pShapeDesc ) )
 		{
 			dTriMeshDataID dataId = mOdeWorld->getMeshDataById( pTriMeshDesc->meshId );
			
			OdeTriMesh* pMesh = new OdeTriMesh( mOdeWorld->_getOdeSpace(), dataId );
			
			YAKE_ASSERT( pMesh ).error( "Mesh with such id wasn't found!" );
			
 			result = createTransformGeomIfNeeded( pMesh, rShapeDesc.position, rShapeDesc.orientation );
 		}

		YAKE_ASSERT( result != NULL ).error( "Unsupported shape type!" );
	
		/// setting material if any
		if ( pOdeMaterial != NULL )
			result->setMaterial( *pOdeMaterial );
	
		return dynamic_cast<IShape*>( result );
	}

	//-----------------------------------------------------
/*	SharedPtr<IShape> OdeActor::createShape( IShape::Desc const& rShapeDesc )
	{
		IShape* pShape = createShapeFromDesc( rShapeDesc );
		// TODO Check if it works with OdeGeom
		OdeMovableGeom* pGeom = dynamic_cast<OdeMovableGeom*>( pShape );
		
		mShapes.push_back( pShape );
		mOdeShapes.insert( OdeShapesMap::value_type( pGeom->_getOdeGeomID(), pGeom ) ); 
		
		return SharedPtr<IShape>( pShape );
	}
	
	//-----------------------------------------------------
	void OdeActor::destroyShape( SharedPtr<IShape>& rShape )
	{
		ShapePtrVector<IShape*>::iterator victim = std::find( mShapes.begin(), mShapes.end(), rShape.get() );
		
		//TODO is this correct?
		YAKE_SAFE_DELETE( *victim );
		
		mShapes.erase( victim );
	}
*/
	//-----------------------------------------------------
	const IActor::ShapePtrVector OdeActor::getShapes() const
	{
		return mShapes;
	}

 	//-----------------------------------------------------
 	void OdeActor::subscribeToCollisionEnteredSignal( const IActor::SignalCollision::slot_type & slot )
 	{
 		mEnterCollisionSignal.connect( slot );
 	}

	//-----------------------------------------------------
 	void OdeActor::subscribeToCollisionExitedSignal( const IActor::SignalCollision::slot_type & slot )
 	{
 		mExitCollisionSignal.connect( slot );
 	}

	//-----------------------------------------------------
// 	void OdeActor::postStep( real timeElapsed )
// 	{
// 		base::templates::Vector< OdeActor* > its;
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
// 		base::templates::VectorIterator< base::templates::Vector< OdeActor* > > itErase(
// 			its.begin(), its.end() );
// 		while (itErase.hasMoreElements())
// 		{
// 			CollisionList::iterator itFind = mCollisions.find( itErase.getNext() );
// 			if (itFind != mCollisions.end())
// 				mCollisions.erase( itFind );
// 		}
// 		its.clear();
// 	}

	//-----------------------------------------------------
	void OdeActor::_collide( OdeActor* pOther, dGeomID geomA, dGeomID geomB, dJointGroup* contactJointGroup )
	{
 		YAKE_ASSERT( pOther ).debug( "Need the other object participating in the collision!" );
		YAKE_ASSERT( contactJointGroup ).error( "Need a joint group for contact joints!" );

		
		/// FIXME Is this needed ?
		if ( ( dGeomGetClass( geomA ) == dRayClass ) || ( dGeomGetClass( geomB ) == dRayClass ) )
			return;

		OdeShapesMap::iterator iA = mOdeShapes.find( geomA );
		OdeShapesMap::iterator iB = mOdeShapes.find( geomB );

		YAKE_ASSERT( !( iA == mOdeShapes.end() || iB == mOdeShapes.end() ) ).error( "No corresponding OdeGeom found!" );
		
		OdeGeom* pShapeA = iA->second;
		OdeGeom* pShapeB = iB->second;
		
		const OdeMaterial& rMatA = pShapeA->getMaterial();
		const OdeMaterial& rMatB = pShapeB->getMaterial();
		
		/// FIXME Implement materials!
		float softness =  rMatA.mSoftness + rMatB.mSoftness; //this->getSoftness() + pOther->getSoftness();

		/// FIXME what's this??? Magic number!
		const int N = 40;
		dContact contact[N];

		// initiating collision detection
		int n = dCollide( geomA, geomB, N, &contact[0].geom, sizeof( dContact ) );

		if ( n > 0 )
		{
			// fire collision events
			CollisionCache::iterator itFind = mCollisions.find( pOther );
			if ( itFind == mCollisions.end() )
			{
				// new collision
				CollisionInfo info = 0;
				mCollisions.insert( CollisionCache::value_type( pOther, info ) );
				/// FIXME  collision signal!  //mEnterCollisionSignal( this, pOther );
			}
			else
			{
				// still colliding
				itFind->second = 0;
			}
			
			// create contact joints
			for ( int i = 0; i < n; ++i ) 
			{
				contact[i].surface.mode = //dContactSlip1
										//dContactSlip2
										dContactSoftERP
										//|dContactSoftCFM
										//|dContactBounce
										|dContactApprox1
										//|dContactMu2
										;
				if ( softness > 0 )
				{
					contact[i].surface.mode |= dContactSoftCFM;
					contact[i].surface.soft_cfm = softness;
				}
				
				///FIXME Implement materials!
				contact[i].surface.mu = 0; //std::min( this->getFriction(), pOther->getFriction());
				float friction2 = std::min( rMatA.mFriction2, rMatB.mFriction2 ); //std::min(this->getFriction2(), pOther->getFriction2());
				if ( friction2 > 0.f )
				{
					contact[i].surface.mode |= dContactMu2;
					contact[i].surface.mu2 = friction2; // 0 or 10
				}
				contact[i].surface.slip1 = 0; // 0.1
				contact[i].surface.slip2 = 0.1;

				// FIXME put that stuff in material: if ( mLateralSlip && mSlipNormalSource )
				{
					// velocities & coefficients
// 					Vector3 linVel = mBody->getLinearVelocity();
// 					Vector3 angVel = mBody->getAngularVelocity();

					// contact normal
					Vector3 normal = Vector3( contact[i].geom.normal[0],
											contact[i].geom.normal[1],
											contact[i].geom.normal[2] );
					normal.normalise();

					Vector3 mSlipNormal = Vector3( 0, 1, 0 ); // FIXME Bad hack
					//mSlipNormal = mSlipNormalSource->getLateralSlipNormal();
					
					real mSlipLinearCoeff; /// FIXME bad hack
					real mSlipAngularCoeff; /// FIXME bad hack 
					
					/// FIXME Error tolerance is hardcoded!
					if ( (mSlipNormal - normal).length() < 0.01 )
					{
						std::cout << " warning: OdeActor::_collide: normal = slipNormal" << std::endl;
					}
					else
					{
						real dot = normal.dotProduct( mSlipNormal );
						/// FIXME Error tolerance hardcoded
						if (dot < 0.9 && dot > -0.9)
						{
							// find the fdir1 direction
							Vector3 perpVec = mSlipNormal.crossProduct( normal );
							perpVec.normalise();

							// set contact info
							contact[i].surface.mode |= dContactFDir1|dContactSlip2;

							//  (kLinearVel*f)*linearVel + (kAngularVel*f)*angularVel
//							contact[i].surface.slip2 =	mSlipLinearCoeff * linVel.length() +
//													mSlipAngularCoeff * angVel.length();

							contact[i].fdir1[0] = perpVec.x;
							contact[i].fdir1[1] = perpVec.y;
							contact[i].fdir1[2] = perpVec.z;
						}
					}
				}
				contact[i].surface.motion1 = 0;
				contact[i].surface.motion2 = 0;

				contact[i].surface.soft_erp = 0.9; // 0.8
				//contact[i].surface.bounce = 0.05; //0.05
				//contact[i].surface.bounce_vel = 0.025; //0.025
				
				//for (int j=1; j<=mNumJointsPerContact; ++j)
				{
					dJointID c = dJointCreateContact (
									mOdeWorld->_getOdeID(),
									contactJointGroup->id(),
									contact + i );
					dJointAttach ( c, dGeomGetBody( geomA ), dGeomGetBody( geomB ) );
				}
			}
		}
	}
	
	//-----------------------------------------------------
	//	OdeStaticActor
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	OdeStaticActor::OdeStaticActor( OdeWorld* pWorld ) :
				OdeActor( pWorld )
	{
	}

	//-----------------------------------------------------
	OdeStaticActor::~OdeStaticActor()
	{
	}
	
	//-----------------------------------------------------
	SharedPtr<IShape> OdeStaticActor::createShape( const IShape::Desc& rShapeDesc )
	{
		IShape* pShape = createShapeFromDesc( rShapeDesc );
		
		OdeGeom* pGeom = dynamic_cast<OdeGeom*>( pShape );
		
		mShapes.push_back( pShape );
		mOdeShapes.insert( OdeShapesMap::value_type( pGeom->_getOdeGeomID(), pGeom ) ); 
		
		pGeom->_setData( this );
		
		return SharedPtr<IShape>( pShape );
	}

 	//-----------------------------------------------------
 	void OdeStaticActor::subscribeToCollisionEnteredSignal( const IActor::SignalCollision::slot_type & slot )
 	{
		OdeActor::subscribeToCollisionEnteredSignal( slot );
 	}

	//-----------------------------------------------------
 	void OdeStaticActor::subscribeToCollisionExitedSignal( const IActor::SignalCollision::slot_type & slot )
 	{
		OdeActor::subscribeToCollisionExitedSignal( slot );
 	}

	//-----------------------------------------------------
	//	OdeMovableActor
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	OdeMovableActor::OdeMovableActor( OdeWorld* pWorld ) :
				OdeActor( pWorld )
	{
	}
	
	//-----------------------------------------------------
	OdeMovableActor::~OdeMovableActor()
	{
	}
	
	//-----------------------------------------------------
	SharedPtr<IShape> OdeMovableActor::createShape( const IShape::Desc& rShapeDesc )
	{
		if ( const IShape::PlaneDesc* pPlaneDesc = dynamic_cast<const IShape::PlaneDesc*>( &rShapeDesc ) )
			YAKE_ASSERT( false ).error( "Attempted to attach immovable plane shape to movable actor!" );
			
		IShape* pShape = createShapeFromDesc( rShapeDesc );
		
		OdeMovableGeom* pGeom = dynamic_cast<OdeMovableGeom*>( pShape );
		
		mShapes.push_back( pShape );
		mOdeShapes.insert( OdeShapesMap::value_type( pGeom->_getOdeGeomID(), pGeom ) ); 
		
		pGeom->_setData( this );
		
		return SharedPtr<IShape>( pShape );
	}
	
	//-----------------------------------------------------
	void OdeMovableActor::setPosition( const Vector3& rPosition )
	{
		mPosition = rPosition;
		
		for( ShapePtrVector::iterator i = mShapes.begin(); i != mShapes.end(); ++i )
		{
			OdeMovableGeom* pGeom = dynamic_cast<OdeMovableGeom*>( *i );
			
			pGeom->setPosition( mPosition );
		}
	}
	
	//-----------------------------------------------------
	void OdeMovableActor::setOrientation( const Quaternion& rOrientation )
	{
		mOrientation = rOrientation;
		
		for( ShapePtrVector::iterator i = mShapes.begin(); i != mShapes.end(); ++i )
		{
			OdeMovableGeom* pGeom = dynamic_cast<OdeMovableGeom*>( *i );
			
			pGeom->setOrientation( mOrientation );
		}
	}
	
	//-----------------------------------------------------
	Vector3 OdeMovableActor::getPosition() const
	{
		return mPosition;
	}
	
	//-----------------------------------------------------
	Quaternion OdeMovableActor::getOrientation() const
	{
		return mOrientation;
	}
	
 	//-----------------------------------------------------
 	void OdeMovableActor::subscribeToCollisionEnteredSignal( const IActor::SignalCollision::slot_type & slot )
 	{
		OdeActor::subscribeToCollisionEnteredSignal( slot );
 	}

	//-----------------------------------------------------
 	void OdeMovableActor::subscribeToCollisionExitedSignal( const IActor::SignalCollision::slot_type & slot )
 	{
		OdeActor::subscribeToCollisionExitedSignal( slot );
 	}

	//-----------------------------------------------------
	//	OdeDynamicActor
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	OdeDynamicActor::OdeDynamicActor( OdeWorld* pWorld ) :
				OdeActor( pWorld )
	{
		mBody = new OdeBody( mOdeWorld, *this );
		
		YAKE_ASSERT( mBody ).error( "Failed to create body!" );
	}

	//-----------------------------------------------------
	OdeDynamicActor::~OdeDynamicActor()
	{
		YAKE_SAFE_DELETE( mBody );
	}
	
	//-----------------------------------------------------
	IBody& OdeDynamicActor::getBody() const
	{
		YAKE_ASSERT( mBody );
		return *mBody;
	}
	
	//-----------------------------------------------------
	void OdeDynamicActor::setPosition( Vector3 const& rPosition )
	{
		YAKE_ASSERT( mBody );
		mBody->setPosition( rPosition );
	}
	
	//-----------------------------------------------------
	void OdeDynamicActor::setOrientation( Quaternion const& rOrientation )
	{
		YAKE_ASSERT( mBody );
		mBody->setOrientation( rOrientation );
	}
	
	//-----------------------------------------------------
	Vector3 OdeDynamicActor::getPosition() const
	{
		YAKE_ASSERT( mBody );
		return mBody->getPosition();
	}
	
	//-----------------------------------------------------
	Quaternion OdeDynamicActor::getOrientation() const
	{
		YAKE_ASSERT( mBody );
		return mBody->getOrientation();
	}
	
	//-----------------------------------------------------
	SharedPtr<IShape> OdeDynamicActor::createShape( const IShape::Desc& rShapeDesc )
	{
		YAKE_ASSERT( mBody );
		if ( const IShape::PlaneDesc* pPlaneDesc = dynamic_cast<const IShape::PlaneDesc*>( &rShapeDesc ) )
			YAKE_ASSERT( false ).error( "Attempted to attach immovable plane shape to movable actor!" );
			
		IShape* pShape = createShapeFromDesc( rShapeDesc );
		YAKE_ASSERT( pShape );
		
		OdeMovableGeom* pGeom = dynamic_cast<OdeMovableGeom*>( pShape );
		YAKE_ASSERT( pGeom );
		
		mShapes.push_back( pShape );
		mOdeShapes.insert( OdeShapesMap::value_type( pGeom->_getOdeGeomID(), pGeom ) ); 
		
		pGeom->_setData( this );
		
		dGeomSetBody( pGeom->_getOdeGeomID(), mBody->_getOdeBody()->id() );
		
		return SharedPtr<IShape>( pShape );
	}
	
 	//-----------------------------------------------------
 	void OdeDynamicActor::subscribeToCollisionEnteredSignal( const IActor::SignalCollision::slot_type & slot )
 	{
		OdeActor::subscribeToCollisionEnteredSignal( slot );
 	}

	//-----------------------------------------------------
 	void OdeDynamicActor::subscribeToCollisionExitedSignal( const IActor::SignalCollision::slot_type & slot )
 	{
		OdeActor::subscribeToCollisionExitedSignal( slot );
 	}
} // physics
} // yake
