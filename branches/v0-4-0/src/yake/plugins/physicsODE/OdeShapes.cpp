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
#include <yake/plugins/physicsODE/OdeShapes.h>

namespace yake
{
	namespace physics
	{

		//-----------------------------------------------------
		//	OdeGeom
		//-----------------------------------------------------

		//-----------------------------------------------------
		OdeGeom::OdeGeom(OdeActor* pOwner) :
				mOdeGeomID( 0 ),
				mOdeSpace( 0 ),
				mOdeGeom( 0 ),
				mOwner( pOwner ),
				mMaterial( 0 )
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
		OdeMaterial* OdeGeom::getMaterial() const
		{
			return mMaterial;
		}

		//-----------------------------------------------------
		void OdeGeom::setMaterial( OdeMaterial* pMaterial )
		{
			mMaterial = pMaterial;
		}

		//-----------------------------------------------------
		void OdeGeom::setPosition( Vector3 const& rPosition )
		{
			if (getType() == ST_PLANE)
				return;
			dGeomSetPosition( mOdeGeomID, rPosition.x, rPosition.y, rPosition.z );
		}

		//-----------------------------------------------------
		Vector3 OdeGeom::getPosition() const
		{
			if (getType() == ST_PLANE)
				return Vector3::kZero;
			const dReal* pos = dGeomGetPosition( mOdeGeomID );
			return Vector3( real(pos[0]), real(pos[1]), real(pos[2]) );
		}

		//-----------------------------------------------------
		void OdeGeom::setOrientation( Quaternion const& rOrientation )
		{
			if (getType() == ST_PLANE)
				return;
			const dQuaternion q = { rOrientation.w, rOrientation.x, rOrientation.y, rOrientation.z };
			dGeomSetQuaternion( mOdeGeomID, q );
		}

		//-----------------------------------------------------
		Quaternion OdeGeom::getOrientation() const
		{
			if (getType() == ST_PLANE)
				return Quaternion::kIdentity;

			dQuaternion q;
			dGeomGetQuaternion( mOdeGeomID, q );

			return Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) );
		}

		//-----------------------------------------------------
		Vector3 OdeGeom::getDerivedPosition() const
		{
			dBodyID bodyId = dGeomGetBody( mOdeGeomID );
			const dReal* pos = dBodyGetPosition( bodyId );
			return Vector3( real(pos[0]), real(pos[1]), real(pos[2]) );
		}

		//-----------------------------------------------------
		Quaternion OdeGeom::getDerivedOrientation() const
		{
			dBodyID bodyId = dGeomGetBody( mOdeGeomID );
			const dReal* q = dBodyGetQuaternion( bodyId );
			return Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) );
		}

		//-----------------------------------------------------
		bool operator == (const std::pair<const String,boost::any>& lhs, const std::pair<const String,boost::any>& rhs)
		{
			return (lhs.first == rhs.first);
		}

		//-----------------------------------------------------
		Vector3 OdeGeom::getPropertyVector3(const String& id) const
		{
			PropMap::const_iterator itFind = mProps.find( id );
			YAKE_ASSERT(itFind != mProps.end());
			if (itFind == mProps.end())
				return Vector3();
			return boost::any_cast<Vector3>(itFind->second);
		}

		//-----------------------------------------------------
		real OdeGeom::getPropertyReal(const String& id) const
		{
			PropMap::const_iterator itFind = mProps.find( id );
			YAKE_ASSERT(itFind != mProps.end());
			if (itFind == mProps.end())
				return real(0.);
			return boost::any_cast<real>(itFind->second);
		}

		//-----------------------------------------------------
		//	OdeTriMesh
		//-----------------------------------------------------

		//-----------------------------------------------------
		OdeTriMesh::OdeTriMesh( dSpace* pSpace, OdeActor* pOwner, dTriMeshDataID meshDataId ) :
				OdeGeom( pOwner )
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
		//-----------------------------------------------------
		void OdeTriMesh::buildMeshData(	MeshData& dataToFill,
		                                TriangleMeshDesc::VertexVector const& rVertices,
		                                TriangleMeshDesc::IndexVector const& rIndices,
		                                TriangleMeshDesc::NormalVector const& rNormals )
		{
			YAKE_ASSERT( !rVertices.empty() ).error( "Wont create empty mesh!");
			YAKE_ASSERT( !rIndices.empty() ).error( "Vertices without indices are meaningless!" );
			// Performing conversion to ODE internal format
			dReal*		pVertices;
			uint32*		pIndices;
			dReal*		pNormals;

			// stuffing vertices
			pVertices = new dReal[ 3*rVertices.size() ];
			uint32 elementCounter = 0;

			ConstVectorIterator<TriangleMeshDesc::VertexVector> itV( rVertices.begin(), rVertices.end() );
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

			ConstVectorIterator<TriangleMeshDesc::IndexVector> itI( rIndices.begin(), rIndices.end() );
			while ( itI.hasMoreElements() )
			{
				pIndices[ elementCounter++] = itI.getNext();
			}

			// spice-up with normals
			if ( !rNormals.empty() ) // according to taste (normals are optional)
			{
				pNormals = new dReal[ 3*rNormals.size() ];
				elementCounter  = 0;

				ConstVectorIterator<TriangleMeshDesc::NormalVector> itN( rNormals.begin(), rNormals.end() );
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

			dataToFill.normals = pNormals;
			dataToFill.vertices = pVertices;
			dataToFill.indices = pIndices;
			dataToFill.id = dataId;
		}

		//-----------------------------------------------------
		//	OdeSphere
		//-----------------------------------------------------

		//-----------------------------------------------------
		OdeSphere::OdeSphere( dSpace* pSpace, OdeActor* pOwner, real radius ) :
				OdeGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );

			mOdeSpace = pSpace;
			YAKE_ASSERT( radius > 0 ).warning( "Radius may be invalid!" );

			mOdeGeom = new dSphere( mOdeSpace->id(), static_cast<dReal>( radius ) );
			YAKE_ASSERT( mOdeGeom );
			mOdeGeomID = mOdeGeom->id();

			_setData( this );

			mProps["radius"] = radius;
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
				OdeGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );

			mOdeSpace = pSpace;

			YAKE_ASSERT( radius > 0 ).error( "Radius should be invalid!" );
			YAKE_ASSERT( length > 0 ).error( "Length should be positive!" );

			mOdeGeom = new dCCylinder( mOdeSpace->id(), static_cast<dReal>( radius ), static_cast<dReal>( length ) );

			mOdeGeomID = mOdeGeom->id();

			_setData( this );

			mProps["radius"] = radius;
			mProps["length"] = length;
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
				OdeGeom( pOwner )
		{
			YAKE_ASSERT( pSpace ).debug( "Need a valid space!" );

			mOdeSpace = pSpace;

			mOdeGeom = new dBox( mOdeSpace->id(), sizex, sizey, sizez );

			mOdeGeomID = mOdeGeom->id();

			_setData( this );

			mProps["dimensions"] = Vector3(sizex,sizey,sizez);
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
				OdeGeom( pOwner )
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

			mProps["normal"] = Vector3(a,b,c);
			mProps["d"] = d;
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
				OdeGeom( pOwner ),
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
			return mAttachedGeom ? mAttachedGeom->getType() : ST_OTHER;
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

			mProps = mAttachedGeom->mProps;
		}

		//-----------------------------------------------------
		OdeGeom* OdeTransformGeom::getAttachedGeom() const
		{
			return mAttachedGeom;
		}

		//-----------------------------------------------------
		Vector3 OdeTransformGeom::getDerivedPosition() const
		{
			return (mAttachedGeom ?
				(getPosition() + getDerivedOrientation() * mAttachedGeom->getPosition()) :
				getPosition() );
		}

		//-----------------------------------------------------
		Quaternion OdeTransformGeom::getDerivedOrientation() const
		{
			return (mAttachedGeom ?
				mAttachedGeom->getDerivedOrientation() :
				getOrientation() );
		}

		//-----------------------------------------------------
		void OdeTransformGeom::setPosition( Vector3 const& rPosition )
		{
			if (getType() == ST_PLANE)
				return;
			YAKE_ASSERT( mAttachedGeom );
			const dReal* pos = dGeomGetPosition( mOdeGeomID );
			mAttachedGeom->setPosition( rPosition - Vector3(real(pos[0]), real(pos[1]), real(pos[2])) );
			//dGeomSetPosition( mOdeGeomID, rPosition.x, rPosition.y, rPosition.z );
		}

		//-----------------------------------------------------
		Vector3 OdeTransformGeom::getPosition() const
		{
			if (getType() == ST_PLANE)
				return Vector3::kZero;
			YAKE_ASSERT( mAttachedGeom );

			const dReal* pos = dGeomGetPosition( mOdeGeomID );
			dQuaternion q;
			dGeomGetQuaternion( mOdeGeomID, q );
			return Vector3( real(pos[0]), real(pos[1]), real(pos[2]) ) + 
				Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) ) * mAttachedGeom->getPosition();
		}

		//-----------------------------------------------------
		void OdeTransformGeom::setOrientation( Quaternion const& rOrientation )
		{
			if (getType() == ST_PLANE)
				return;
			YAKE_ASSERT( mAttachedGeom );
			const dQuaternion q = { rOrientation.w, rOrientation.x, rOrientation.y, rOrientation.z };
			dGeomSetQuaternion( mOdeGeomID, q );
		}

		//-----------------------------------------------------
		Quaternion OdeTransformGeom::getOrientation() const
		{
			if (getType() == ST_PLANE)
				return Quaternion::kIdentity;
			YAKE_ASSERT( mAttachedGeom );
			dQuaternion q;
			dGeomGetQuaternion( mOdeGeomID, q );

			YAKE_ASSERT( mAttachedGeom );

			return mAttachedGeom->getOrientation() * Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) );
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
		// 		String OdeCollisionGeomBase::meshGetName() const
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
