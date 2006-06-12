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
	void OdeGeom::setMaterial( IMaterial* pMaterial )
	{
		mMaterial = checked_cast<OdeMaterial*>( pMaterial );
		YAKE_ASSERT( mMaterial != NULL ).warning( "assigned empty material" );
	}

	//-----------------------------------------------------
	void OdeGeom::setPosition( math::Vector3 const& rPosition )
	{
		if (getType() == ST_PLANE)
			return;

		dGeomSetPosition( mOdeGeomID, rPosition.x, rPosition.y, rPosition.z );
	}

	//-----------------------------------------------------
	math::Vector3 OdeGeom::getPosition() const
	{
		if (getType() == ST_PLANE)
			return math::Vector3::kZero;

		const dReal* pos = dGeomGetPosition( mOdeGeomID );
		return math::Vector3( real(pos[0]), real(pos[1]), real(pos[2]) );
	}

	//-----------------------------------------------------
	void OdeGeom::setOrientation( math::Quaternion const& rOrientation )
	{
		if (getType() == ST_PLANE)
			return;

		const dQuaternion q = { rOrientation.w, rOrientation.x, rOrientation.y, rOrientation.z };
		dGeomSetQuaternion( mOdeGeomID, q );
	}

	//-----------------------------------------------------
	math::Quaternion OdeGeom::getOrientation() const
	{
		if (getType() == ST_PLANE)
			return math::Quaternion::kIdentity;

		dQuaternion q;
		dGeomGetQuaternion( mOdeGeomID, q );

		return math::Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) );
	}

	//-----------------------------------------------------
	math::Vector3 OdeGeom::getDerivedPosition() const
	{
		// return just usual geom position. 
		// It is equal to body position if geom was attached to one
		return getPosition();
	}

	//-----------------------------------------------------
	math::Quaternion OdeGeom::getDerivedOrientation() const
	{
		// return just usual geom orientation. 
		// It is equal to body orientation if geom was attached to one
		return getOrientation();
	}

	//-----------------------------------------------------
	bool operator == (const std::pair<const String,boost::any>& lhs, const std::pair<const String,boost::any>& rhs)
	{
		return (lhs.first == rhs.first);
	}

	//-----------------------------------------------------
	math::Vector3 OdeGeom::getPropertyVector3(const String& id) const
	{
		PropMap::const_iterator itFind = mProps.find( id );
		YAKE_ASSERT(itFind != mProps.end());
		if (itFind == mProps.end())
			return math::Vector3();
		return boost::any_cast<math::Vector3>(itFind->second);
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
		YAKE_LOG_INFORMATION("physicsOde: enabled sphere+box collisions for trimesh");

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
		dReal*		pVertices = 0;
		uint32*		pIndices = 0;
		dReal*		pNormals = 0;

		// stuffing vertices
		pVertices = new dReal[ 3*rVertices.size() ];
		ConstVectorIterator< TriangleMeshDesc::VertexVector > itV( rVertices );
		dReal* pCurrOrd = pVertices;
		while (itV.hasMoreElements())
		{
			const math::Vector3& v = *itV.peekNextPtr();
			*pCurrOrd++ = v.x;
			*pCurrOrd++ = v.y;
			*pCurrOrd++ = v.z;
			itV.moveNext();
		}

		// pouring indices
		pIndices = new uint32[ rIndices.size() ];
		std::copy( rIndices.begin(), rIndices.end(), pIndices );

		// spice-up with normals
		if ( !rNormals.empty() ) // according to taste (normals are optional)
		{
			pNormals = new dReal[ 3*rNormals.size() ];
			ConstVectorIterator< TriangleMeshDesc::VertexVector > itN( rNormals );
			pCurrOrd = pNormals;
			while (itN.hasMoreElements())
			{
				const math::Vector3& n = *itN.peekNextPtr();
				*pCurrOrd++ = n.x;
				*pCurrOrd++ = n.y;
				*pCurrOrd++ = n.z;
				itN.moveNext();
			}
		}

		// serving up
		dTriMeshDataID dataId = dGeomTriMeshDataCreate();

#ifdef dDOUBLE
		if ( !rNormals.empty() )
		{
			dGeomTriMeshDataBuildDouble1( dataId,
				                            pVertices,
				                            sizeof( dReal )*3,
				                            int(rVertices.size()),
				                            pIndices,
				                            int(rIndices.size()),
				                            sizeof( uint32 )*3,
				                            pNormals );
		}
		else
		{
			dGeomTriMeshDataBuildDouble(  dataId,
				                            pVertices,
				                            sizeof( dReal )*3,
				                            int(rVertices.size()),
				                            pIndices,
				                            int(rIndices.size()),
				                            sizeof( uint32 )*3 );
		}
#endif
#ifdef dSINGLE
		if ( !rNormals.empty() )
		{
			dGeomTriMeshDataBuildSingle1( dataId,
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
			dGeomTriMeshDataBuildSingle(  dataId,
				                            pVertices,
				                            sizeof( dReal )*3,
				                            rVertices.size(),
				                            pIndices,
				                            rIndices.size(),
				                            sizeof( uint32 )*3 );
		}
#endif

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

		mOdeGeom = new dCapsule( mOdeSpace->id(), static_cast<dReal>( radius ), static_cast<dReal>( length ) );

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

		mProps["dimensions"] = math::Vector3(sizex,sizey,sizez);
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

		math::Vector3 normal = math::Vector3( a, b, c );

		if ( normal.squaredLength() < 0.02 )
			normal = math::Vector3( 0, 1, 0 );
		else
			normal.normalise();

		mOdeGeom = new dPlane( mOdeSpace->id(), normal.x, normal.y, normal.z, d );
		YAKE_ASSERT( mOdeGeom );
		mOdeGeomID = mOdeGeom->id();

		_setData( this );

		mProps["normal"] = math::Vector3(a,b,c);
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
	math::Vector3 OdeTransformGeom::getPosition() const
	{
		return	mAttachedGeom ?	mAttachedGeom->getPosition() : OdeGeom::getPosition();
	}

	//-----------------------------------------------------
	math::Quaternion OdeTransformGeom::getOrientation() const
	{
		return mAttachedGeom ? mAttachedGeom->getOrientation() : OdeGeom::getOrientation();
	}

	//-----------------------------------------------------
	void OdeTransformGeom::setPosition( const math::Vector3& rPosition )
	{
		if (getType() == ST_PLANE)
		{
			YAKE_ASSERT( false ).warning( "You are trying to set position for non-placeable geom!!" );
			return;
		}
		
		YAKE_ASSERT( mAttachedGeom );
		
		mAttachedGeom->setPosition( rPosition );
	}

	//-----------------------------------------------------
	math::Vector3 OdeTransformGeom::getDerivedPosition() const
	{
		if (getType() == ST_PLANE)
			return math::Vector3::kZero;

		YAKE_ASSERT( mAttachedGeom );

		math::Vector3 this_pos = OdeGeom::getPosition();
		math::Quaternion this_orientation = OdeGeom::getOrientation();

		return this_pos + this_orientation * mAttachedGeom->getPosition();
	}

	//-----------------------------------------------------
	void OdeTransformGeom::setOrientation( const math::Quaternion& rOrientation )
	{
		if (getType() == ST_PLANE)
		{
			YAKE_ASSERT( false ).warning( "You are trying to set orientation for non-placeable geom!!" );
			return;
		}

		if ( mAttachedGeom != NULL )
		    mAttachedGeom->setOrientation( rOrientation );
		else
		    OdeGeom::setOrientation( rOrientation );
	}

	//-----------------------------------------------------
	Quaternion OdeTransformGeom::getDerivedOrientation() const
	{
		if (getType() == ST_PLANE)
			return Quaternion::kIdentity;

		YAKE_ASSERT( mAttachedGeom );

		math::Quaternion this_orientation = OdeGeom::getOrientation();

		Quaternion childOrientation = mAttachedGeom->getOrientation();
		Quaternion result = this_orientation * childOrientation;
		return  result;
	}

} // physics
} // yake

