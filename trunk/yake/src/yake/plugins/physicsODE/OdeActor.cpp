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
	//-----------------------------------------------------
	OdeActor::OdeActor( OdeWorld* pWorld, bool bDynamic ) :
			mOdeWorld(pWorld),
			mBody(0)
	{
		if (bDynamic)
		{
			mBody = new OdeBody( mOdeWorld, *this );
			YAKE_ASSERT( mBody ).error( "Failed to create body!" );
			mBody->setPosition( Vector3::kZero );
			mBody->setOrientation( Quaternion::kIdentity );
		}
		this->setPosition( Vector3::kZero );
		this->setOrientation( Quaternion::kIdentity );
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
	OdeGeom* OdeActor::createTransformGeomIfNeeded(	OdeGeom* pGeom,
														const Vector3& rOffset,
														const Quaternion& rRelOrientation )
	{
		if ( rOffset == Vector3::kZero && rRelOrientation == Quaternion::kIdentity )
			return pGeom;
			
		OdeTransformGeom* pTrans = new OdeTransformGeom( mOdeWorld->_getOdeSpace(), this );
		pTrans->attachGeom( pGeom );
		
		pTrans->setPosition( rOffset );
		pTrans->setOrientation( rRelOrientation );
		
		return pTrans;
	}

	//-----------------------------------------------------
	OdeGeom* OdeActor::createShapeFromDesc( IShape::Desc const& rShapeDesc )
	{
		IShape::Desc* pShapeDesc = &const_cast<IShape::Desc&>( rShapeDesc );
		
		IMaterial* pMaterial = pShapeDesc->pMaterial;
		OdeMaterial* pOdeMaterial = 0;
		
		if (pMaterial)
			pOdeMaterial = dynamic_cast<OdeMaterial*>( pMaterial );
	
		OdeGeom* result = 0;
		
		if ( IShape::SphereDesc* pSphereDesc = dynamic_cast<IShape::SphereDesc*>( pShapeDesc ) )
		{
			OdeSphere* pSphere = new OdeSphere( mOdeWorld->_getOdeSpace(), this, pSphereDesc->radius );
			
			result = createTransformGeomIfNeeded( pSphere, rShapeDesc.position, rShapeDesc.orientation );
		}
		else if ( IShape::BoxDesc* pBoxDesc = dynamic_cast<IShape::BoxDesc*>( pShapeDesc ) )
		{
			OdeBox* pBox = new OdeBox( mOdeWorld->_getOdeSpace(), 
										this,
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
												this,
												normal.x,
												normal.y,
												normal.z,
												d );
			result = pPlane;
		}
		else if ( IShape::CapsuleDesc* pCapsuleDesc = dynamic_cast<IShape::CapsuleDesc*>( pShapeDesc ) )
		{
			OdeCCylinder* pCapsule = new OdeCCylinder( mOdeWorld->_getOdeSpace(), 
										this,
										pCapsuleDesc->radius,
										pCapsuleDesc->height  );
			result = createTransformGeomIfNeeded( pCapsule, rShapeDesc.position, rShapeDesc.orientation );
		}
 		else if ( IShape::TriMeshDesc* pTriMeshDesc = dynamic_cast<IShape::TriMeshDesc*>( pShapeDesc ) )
 		{
			OdeTriMesh::MeshData data = mOdeWorld->getMeshDataById( pTriMeshDesc->meshId );
			
			OdeTriMesh* pMesh = new OdeTriMesh( mOdeWorld->_getOdeSpace(), this, data.id );
			
			YAKE_ASSERT( pMesh ).error( "Mesh with such id wasn't found!" );
			
 			result = createTransformGeomIfNeeded( pMesh, rShapeDesc.position, rShapeDesc.orientation );
 		}

		YAKE_ASSERT( result != 0 ).error( "Unsupported shape type!" );
	
		/// setting material if any
		if ( pOdeMaterial != 0 )
			result->setMaterial( *pOdeMaterial );

		if (result)
			mShapes.push_back( SharedPtr<OdeGeom>(result) );

		return result;
	}

	//-----------------------------------------------------
	bool operator == (const SharedPtr<OdeGeom>& lhs, const OdeGeom* rhs)
	{
		return (lhs.get() == rhs);
	}
	void OdeActor::destroyShape( IShape* pShape )
	{
		OdeGeom* pGeom = dynamic_cast<OdeGeom*>( pShape );
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
			ret.push_back( dynamic_cast<IShape*>(it->get()) );
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

	//-----------------------------------------------------
	void OdeActor::_collide( OdeActor* pOther, dGeomID geomA, dGeomID geomB, dJointGroup* contactJointGroup )
	{
 		YAKE_ASSERT( pOther ).debug( "Need the other object participating in the collision!" );
		YAKE_ASSERT( contactJointGroup ).error( "Need a joint group for contact joints!" );

		
		/// FIXME Is this needed ?
		if ( ( dGeomGetClass( geomA ) == dRayClass ) || ( dGeomGetClass( geomB ) == dRayClass ) )
			return;

		OdeGeom* pShapeA = reinterpret_cast<OdeGeom*>(dGeomGetData( geomA ));
		OdeGeom* pShapeB = reinterpret_cast<OdeGeom*>(dGeomGetData( geomB ));
		
		OdeActor* pActorA = pShapeA->getOwner();
		OdeActor* pActorB = pShapeB->getOwner();

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
				// still colliding, so reset timeout
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

#ifdef DEAD
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
#endif
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
	IShape* OdeActor::createShape( const IShape::Desc& rShapeDesc, real massOrDensity, IBody::quantityType type )
	{
		if ( mBody && dynamic_cast<const IShape::PlaneDesc*>( &rShapeDesc ) )
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
	void OdeActor::setPosition( const Vector3& rPosition )
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
	void OdeActor::setOrientation( const Quaternion& rOrientation )
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
	Vector3 OdeActor::getPosition() const
	{
		if (mBody)
			return mBody->getPosition();
		return mPosition;
	}
	
	//-----------------------------------------------------
	Quaternion OdeActor::getOrientation() const
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
