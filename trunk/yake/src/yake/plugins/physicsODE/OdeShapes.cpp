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
#include <yake/plugins/physicsODE/OdeShapes.h>

namespace yake {
namespace physics {

		//-----------------------------------------------------
		//	OdeGeom
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		OdeGeom::OdeGeom(OdeActor* pOwner) : 
			mOdeGeomID( 0 ), 
			mOdeSpace( 0 ), 
			mOdeGeom( 0 ),
			mOwner( pOwner )
		{
			YAKE_ASSERT( mOwner );
		}
		
		//-----------------------------------------------------
		OdeGeom::~OdeGeom()
		{
			if ( mOdeGeom != NULL )
			{
				YAKE_SAFE_DELETE( mOdeGeom );
			}
			else if ( mOdeGeomID != 0 )
			{
				dGeomDestroy( mOdeGeomID );
				mOdeGeomID = 0;
			}
		}
		
		//-----------------------------------------------------
		dGeomID OdeGeom::_getOdeGeomID() const 
		{
			return mOdeGeomID; 
		}
		
		//-----------------------------------------------------
		dSpace* OdeGeom::_getOdeSpace() const	
		{ 
			return mOdeSpace;
		}
		
		//-----------------------------------------------------
		void OdeGeom::_setData( void* pData )
		{
			dGeomSetData( mOdeGeomID, pData );
		}
		
		//-----------------------------------------------------
 		void* OdeGeom::_getData() const
		{
			return dGeomGetData( mOdeGeomID );
		}
		
		//-----------------------------------------------------
		dGeom* OdeGeom::_getOdeGeomPtr() 
		{ 
			return mOdeGeom;
		}
		
		//-----------------------------------------------------
		const OdeMaterial& OdeGeom::getMaterial() const
		{
			return mMaterial;
		}
		
		//-----------------------------------------------------
		void OdeGeom::setMaterial( OdeMaterial const& rMaterial )
		{
			mMaterial = rMaterial;
		}
			
		//-----------------------------------------------------
		//	OdeMovableGeom
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		 void OdeMovableGeom::setPosition( Vector3 const& rPosition )
		 {
 			dGeomSetPosition( mOdeGeomID, rPosition.x, rPosition.y, rPosition.z );
		 }
		 
		//-----------------------------------------------------
		Vector3 OdeMovableGeom::getPosition() const
		{
			const dReal* pos = dGeomGetPosition( mOdeGeomID );
			return Vector3( pos[0], pos[1], pos[2] );
		}
		
		//-----------------------------------------------------
		void OdeMovableGeom::setOrientation( Quaternion const& rOrientation )
		{
			const dQuaternion q = { rOrientation.w, rOrientation.x, rOrientation.y, rOrientation.z };
			dGeomSetQuaternion( mOdeGeomID, q );
		}
		
		//-----------------------------------------------------
		Quaternion OdeMovableGeom::getOrientation() const
		{
			dQuaternion q;
			dGeomGetQuaternion( mOdeGeomID, q );
			
			return Quaternion( q[0], q[1], q[2], q[3] );
		}

		//-----------------------------------------------------
		//	OdeTriMesh
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		OdeTriMesh::OdeTriMesh( dSpace* pSpace, OdeActor* pOwner, dTriMeshDataID meshDataId ) :
			OdeMovableGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );
			
			mOdeSpace = pSpace;

			// enjoy!
			mOdeGeomID = dCreateTriMesh( mOdeSpace->id(), meshDataId, 0, 0, 0 );
			
			// enabling collisions with spheres and boxes
			dGeomTriMeshEnableTC( mOdeGeomID, dSphereClass, 1 );
			dGeomTriMeshEnableTC( mOdeGeomID, dBoxClass, 1 );

			_setData( this );
		}

		//-----------------------------------------------------
		ShapeType OdeTriMesh::getType() const
		{
			return ST_TRIANGLE_MESH;
		}
		
		//-----------------------------------------------------
		OdeTriMesh::~OdeTriMesh()
		{
			dGeomDestroy( mOdeGeomID );
			
		}

		//-----------------------------------------------------
		dTriMeshDataID OdeTriMesh::buildMeshData( TriangleMeshDesc::VertexVector const& rVertices,
						 		  	  				TriangleMeshDesc::IndexVector const& rIndices,
						 		  	 				TriangleMeshDesc::NormalVector const& rNormals )
		{
			YAKE_ASSERT( !rVertices.empty() ).error( "Wont create empty mesh!");
			YAKE_ASSERT( !rIndices.empty() ).error( "Vertices without indices are meaningless!" );

			// Performing conversion to ODE internal format
			dReal*		pVertices = 0;
 			uint32*		pIndices = 0;
 			dReal*		pNormals = 0;
 			
			// stuffing vertices
 			pVertices = new dReal[ 3*rVertices.size() ];
 			uint32 elementCounter = 0;

 			base::templates::ConstVectorIterator<TriangleMeshDesc::VertexVector> itV( rVertices.begin(), rVertices.end() );
 			while ( itV.hasMoreElements() )
 			{
 				Vector3 const& v = itV.getNext();
 				pVertices[ elementCounter*3 + 0 ] = v.x;
 				pVertices[ elementCounter*3 + 1 ] = v.y;
 				pVertices[ elementCounter*3 + 2 ] = v.z;
 				++elementCounter;
 			}
 			
			// pouring indices
			pIndices = new uint32[ rIndices.size() ];
			elementCounter = 0;
 			
			base::templates::ConstVectorIterator<TriangleMeshDesc::IndexVector> itI( rIndices.begin(), rIndices.end() );
 			while ( itI.hasMoreElements() )
 			{
 				pIndices[ elementCounter++] = itI.getNext();
 			}

			// spice-up with normals
 			if ( !rNormals.empty() ) // according to taste (normals are optional)
 			{
 				pNormals = new dReal[ 3*rNormals.size() ];
 				elementCounter  = 0;
 				
				base::templates::ConstVectorIterator<TriangleMeshDesc::NormalVector> itN( rNormals.begin(), rNormals.end() );
 				while ( itN.hasMoreElements() )
 				{
 					Vector3 const& v = itN.getNext();
 					pNormals[ elementCounter*3 + 0 ] = v.x;
 					pNormals[ elementCounter*3 + 1 ] = v.y;
 					pNormals[ elementCounter*3 + 2 ] = v.z;
 					++elementCounter;
 				}
 			}
 
			// serving up
 			dTriMeshDataID dataId = dGeomTriMeshDataCreate();
 
 			if ( !rNormals.empty() )
 			{
 				dGeomTriMeshDataBuildDouble1( dataId,
 												 pVertices,
 												 sizeof( dReal )*3,
 												 rVertices.size(),
 												 pIndices,
 												 rIndices.size(),
 												 sizeof( uint32 )*3,
 												 pNormals );
 			}
 			else
 			{
 				dGeomTriMeshDataBuildDouble(  dataId,
 												pVertices,
 												sizeof( dReal )*3,
 												rVertices.size(),
 												pIndices,
 												rIndices.size(),
 												sizeof( uint32 )*3 );
 			}
			
			YAKE_SAFE_DELETE( pNormals );
			YAKE_SAFE_DELETE( pVertices );
			YAKE_SAFE_DELETE( pIndices );
			
			return dataId;
		}
		
		//-----------------------------------------------------
		//	OdeSphere
		//-----------------------------------------------------

		//-----------------------------------------------------
		OdeSphere::OdeSphere( dSpace* pSpace, OdeActor* pOwner, real radius ) :
			OdeMovableGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );

			mOdeSpace = pSpace;
			YAKE_ASSERT( radius > 0 ).warning( "Radius may be invalid!" );

			mOdeGeom = new dSphere( mOdeSpace->id(), static_cast<dReal>( radius ) );
			YAKE_ASSERT( mOdeGeom );
			mOdeGeomID = mOdeGeom->id();

			_setData( this );
		}
		
		//-----------------------------------------------------
		ShapeType OdeSphere::getType() const
		{
			return ST_SPHERE;
		}

		//-----------------------------------------------------
		//	OdeCCylinder
		//-----------------------------------------------------

		//-----------------------------------------------------			
		OdeCCylinder::OdeCCylinder( dSpace* pSpace, OdeActor* pOwner, real radius, real length ) :
			OdeMovableGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );
			
			mOdeSpace = pSpace;
			
			YAKE_ASSERT( radius > 0 ).error( "Radius should be invalid!" );
			YAKE_ASSERT( length > 0 ).error( "Length should be positive!" );

			mOdeGeom = new dCCylinder( mOdeSpace->id(), static_cast<dReal>( radius ), static_cast<dReal>( length ) );
			
			mOdeGeomID = mOdeGeom->id();

			_setData( this );
		}
		
		//-----------------------------------------------------
		ShapeType OdeCCylinder::getType() const
		{
			return ST_CYLINDER;
		}
		
		//-----------------------------------------------------
		//	OdeBox
		//-----------------------------------------------------
		
		//-----------------------------------------------------			
		OdeBox::OdeBox( dSpace* pSpace, OdeActor* pOwner, real sizex, real sizey, real sizez) : 
			OdeMovableGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );
			
			mOdeSpace = pSpace;
			
			mOdeGeom = new dBox( mOdeSpace->id(), sizex, sizey, sizez );
			
			mOdeGeomID = mOdeGeom->id();

			_setData( this );
		}

		//-----------------------------------------------------
		ShapeType OdeBox::getType() const
		{
			return ST_BOX;
		}

		//-----------------------------------------------------
		//	OdePlane
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		OdePlane::OdePlane( dSpace* pSpace, OdeActor* pOwner, real a, real b, real c, real d ) :
			OdeStaticGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );
			
			mOdeSpace = pSpace;

			YAKE_ASSERT( a != 0. || b != 0. || c != 0. ).error( "Invalid plane equation!" );
			
			Vector3 normal = Vector3( a, b, c );
			
			if ( normal.squaredLength() < 0.02 )
				normal = Vector3( 0, 1, 0 );
			else
				normal.normalise();

			mOdeGeom = new dPlane( mOdeSpace->id(), normal.x, normal.y, normal.z, d );
			YAKE_ASSERT( mOdeGeom );
			mOdeGeomID = mOdeGeom->id();

			_setData( this );
		}
		
		//-----------------------------------------------------
		ShapeType OdePlane::getType() const
		{
			return ST_PLANE;
		}
		
		//-----------------------------------------------------
		//	OdeTransformGeom
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		OdeTransformGeom::OdeTransformGeom( dSpace* pSpace, OdeActor* pOwner ) : 
			OdeMovableGeom( pOwner ),
			mAttachedGeom( NULL )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );
			
			mOdeSpace = pSpace;
			
			mOdeGeom = new dGeomTransform( mOdeSpace->id() );
			YAKE_ASSERT( mOdeGeom );
			mOdeGeomID = mOdeGeom->id();
			
			dGeomTransformSetCleanup( mOdeGeomID, 1 );
			dGeomTransformSetInfo( mOdeGeomID, 1 );

			_setData( this );
		}
		
		//-----------------------------------------------------
		ShapeType OdeTransformGeom::getType() const
		{
			return ST_OTHER;
		}

		//-----------------------------------------------------
		void OdeTransformGeom::attachGeom( OdeGeom* pGeom )
		{ 
			YAKE_ASSERT( pGeom ).error( "Attachment failed due to invalid geom passed!" );
			
			mAttachedGeom = pGeom;
			
			mAttachedGeom->_setData( this->_getData() );

			dSpace* pSpace = mAttachedGeom->_getOdeSpace();
			YAKE_ASSERT( pSpace ).error( "Attached geom doesn't contain valid ODE space!" );

			pSpace->remove( mAttachedGeom->_getOdeGeomID() );
			dGeomTransformSetGeom( mOdeGeomID, mAttachedGeom->_getOdeGeomID() );
		}
		
		//-----------------------------------------------------
		OdeGeom* OdeTransformGeom::getAttachedGeom() const
		{ 
			return mAttachedGeom;
		}
		
// /*		//-----------------------------------------------------
// 		void OdeCollisionGeomBase::tfAttachGeom( ICollisionGeometry* pGeom )
// 		{ YAKE_ASSERT(1==0).error("not implemented"); }
// 		//-----------------------------------------------------
// 		ICollisionGeometry* OdeCollisionGeomBase::tfGetAttachedGeom() const
// 		{ return 0; }
// 		//-----------------------------------------------------
// 		Vector3 OdeCollisionGeomBase::planeGetNormal() const
// 		{ return Vector3::kZero; }
// 		//-----------------------------------------------------
// 		base::String OdeCollisionGeomBase::meshGetName() const
// 		{ return ""; }
// 		//-----------------------------------------------------
// 		real OdeCollisionGeomBase::planeGetDistance() const
// 		{ return 0; }
// 		//-----------------------------------------------------
// 		real OdeCollisionGeomBase::sphereGetRadius() const
// 		{ return 0; }
// 		//-----------------------------------------------------
// 		void OdeCollisionGeomBase::sphereSetRadius(const real radius)
// 		{}
// 		//-----------------------------------------------------
// 		Vector3 OdeCollisionGeomBase::boxGetDimensions() const
// 		{ return Vector3::kZero; }
// 		//-----------------------------------------------------
// 		Vector3 OdeCollisionGeomBase::rayGetOrigin() const
// 		{ return Vector3::kZero; }
// 		//-----------------------------------------------------
// 		Quaternion OdeCollisionGeomBase::rayGetOrientation() const
// 		{ return Quaternion::kIdentity; }
// 
// 		//-----------------------------------------------------
// 		real OdeCollisionGeomSphere::sphereGetRadius() const
// 		{
// 			return static_cast<dSphere*>(mOdeGeom)->getRadius();
// 		}
// 
// 		//-----------------------------------------------------
// 		void OdeCollisionGeomSphere::sphereSetRadius(const real radius)
// 		{
// 			static_cast<dSphere*>(mOdeGeom)->setRadius(radius);
// 		}
// 
// 		//-----------------------------------------------------
// 		Vector3 OdeCollisionGeomBox::boxGetDimensions() const
// 		{
// 			dVector3 lengths;
// 			static_cast<dBox*>(mOdeGeom)->getLengths(lengths);
// 			return Vector3( lengths[0], lengths[1], lengths[2] );
// 		}
// 
// 
// 		//-----------------------------------------------------
// 		Vector3 OdeCollisionGeomPlane::planeGetNormal() const
// 		{
// 			dVector4 params;
// 			static_cast<dPlane*>(mOdeGeom)->getParams(params);
// 			return Vector3( params[0], params[1], params[2] );
// 		}
// 		//-----------------------------------------------------
// 		real OdeCollisionGeomPlane::planeGetDistance() const
// 		{
// 			dVector4 params;
// 			static_cast<dPlane*>(mOdeGeom)->getParams(params);
// 			return params[3];
// 		}

} // physics
} // yake
