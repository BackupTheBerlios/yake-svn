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
#include <yake/plugins/physicsODE/OdeCollisionGeometry.h>

//TEMP:FIXME
#include <yake/data/yakeData.h>

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		void OdeCollisionGeomBase::tfAttachGeom( ICollisionGeometry* pGeom )
		{ YAKE_ASSERT(1==0).error("not implemented"); }
		//-----------------------------------------------------
		ICollisionGeometry* OdeCollisionGeomBase::tfGetAttachedGeom() const
		{ return 0; }
		//-----------------------------------------------------
		Vector3 OdeCollisionGeomBase::planeGetNormal() const
		{ return Vector3::kZero; }
		//-----------------------------------------------------
		base::String OdeCollisionGeomBase::meshGetName() const
		{ return ""; }
		//-----------------------------------------------------
		real OdeCollisionGeomBase::planeGetDistance() const
		{ return 0; }
		//-----------------------------------------------------
		real OdeCollisionGeomBase::sphereGetRadius() const
		{ return 0; }
		//-----------------------------------------------------
		void OdeCollisionGeomBase::sphereSetRadius(const real radius)
		{}
		//-----------------------------------------------------
		Vector3 OdeCollisionGeomBase::boxGetDimensions() const
		{ return Vector3::kZero; }
		//-----------------------------------------------------
		Vector3 OdeCollisionGeomBase::rayGetOrigin() const
		{ return Vector3::kZero; }
		//-----------------------------------------------------
		Quaternion OdeCollisionGeomBase::rayGetOrientation() const
		{ return Quaternion::kIdentity; }

		//-----------------------------------------------------
		OdeCollisionGeomBase::OdeCollisionGeomBase()
		{
			mOdeGeomID = 0;
		}

		//-----------------------------------------------------
		OdeCollisionGeomBase::~OdeCollisionGeomBase()
		{
			YAKE_ASSERT( mOdeGeomID == 0 );
		}

		//-----------------------------------------------------
		OdeCollisionGeom::~OdeCollisionGeom()
		{
			if (mOdeGeom)
			{
				YAKE_SAFE_DELETE( mOdeGeom );
			}
			else if (mOdeGeomID)
				dGeomDestroy( mOdeGeomID );
			mOdeGeomID = 0;
		}

		//-----------------------------------------------------
		OdeCollisionGeomBase::CollisonGeomType OdeCollisionGeomBase::getType() const
		{
			return mType;
		}

		//-----------------------------------------------------
		void OdeCollisionGeomBase::setPosition(const Vector3 & position)
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			if (dGeomGetClass(mOdeGeomID) == dPlaneClass) 
				return;// ODE planes are not placeable!
			dGeomSetPosition( mOdeGeomID, position.x, position.y, position.z );
		}

		//-----------------------------------------------------
		Vector3 OdeCollisionGeomBase::getPosition() const
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			if (dGeomGetClass(mOdeGeomID) == dPlaneClass) 
				return Vector3::kZero;// ODE planes are not placeable!
			const dReal* pos = dGeomGetPosition(mOdeGeomID);
			return Vector3(pos[0],pos[1],pos[2]);
		}

		//-----------------------------------------------------
		void OdeCollisionGeomBase::setOrientation(const Quaternion & orientation)
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			if (dGeomGetClass(mOdeGeomID) == dPlaneClass) 
				return;// ODE planes are not placeable!
			const dQuaternion q = { orientation.w, orientation.x, orientation.y, orientation.z };
			dGeomSetQuaternion( mOdeGeomID, q );
		}

		//-----------------------------------------------------
		Quaternion OdeCollisionGeomBase::getOrientation() const
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			if (dGeomGetClass(mOdeGeomID) == dPlaneClass) 
				return Quaternion::kIdentity;// ODE planes are not placeable!
			dQuaternion q;
			dGeomGetQuaternion( mOdeGeomID, q );
			return Quaternion(q[0],q[1],q[2],q[3]);
		}

		//-----------------------------------------------------
		void OdeCollisionGeomBase::_setData(void * data)
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			dGeomSetData( mOdeGeomID, data );
		}

		//-----------------------------------------------------
		void* OdeCollisionGeomBase::_getData() const
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			return dGeomGetData( mOdeGeomID );
		}

		//-----------------------------------------------------
		OdeCollisionGeomSphere::OdeCollisionGeomSphere(dSpace* space, float radius)
		{
			mOdeSpace = space;
			YAKE_ASSERT( radius > 0 ).warning("Radius may be invalid!");
			mType = CGT_SPHERE;
			mOdeGeom = new dSphere( (*space).id(), static_cast<dReal>(radius) );
			mOdeGeomID = mOdeGeom->id(); // don't forget!!
		}

		//-----------------------------------------------------
		real OdeCollisionGeomSphere::sphereGetRadius() const
		{
			return static_cast<dSphere*>(mOdeGeom)->getRadius();
		}

		//-----------------------------------------------------
		void OdeCollisionGeomSphere::sphereSetRadius(const real radius)
		{
			static_cast<dSphere*>(mOdeGeom)->setRadius(radius);
		}

		//-----------------------------------------------------			
		OdeCollisionGeomBox::OdeCollisionGeomBox(dSpace* space, float lx, float ly, float lz)
		{
			mOdeSpace = space;
			/*YAKE_ASSERT( ( lx == 0. && ly == 0. && lz != 0. ) ||
							( lx != 0. && ly == 0. && lz == 0. ) ||
							( lx == 0. && ly != 0. && lz == 0. ) )(lx)(ly)(lz).warning("Dimensions may be invalid!");*/
			mType = CGT_BOX;
			mOdeGeom = new dBox( *space, lx, ly, lz );
			mOdeGeomID = mOdeGeom->id(); // don't forget!!
		}

		//-----------------------------------------------------
		Vector3 OdeCollisionGeomBox::boxGetDimensions() const
		{
			dVector3 lengths;
			static_cast<dBox*>(mOdeGeom)->getLengths(lengths);
			return Vector3( lengths[0], lengths[1], lengths[2] );
		}

		//-----------------------------------------------------
		OdeCollisionGeomPlane::OdeCollisionGeomPlane(dSpace* space, float a, float b, float c, float d)
		{
			mOdeSpace = space;
			mType = CGT_PLANE;

			// make sure we have a valid normal for the plane equation
			//YAKE_ASSERT( a != 0. && b != 0. && c != 0. ).warning("Invalid plane equation.");
			Vector3 normal = Vector3(a,b,c);
			if (normal.squaredLength() == 0.)
				normal = Vector3(0,1,0);
			else
				normal.normalise();

			mOdeGeom = new dPlane( *space, normal.x, normal.y, normal.z, d );
			mOdeGeomID = mOdeGeom->id(); // don't forget!!
		}

		//-----------------------------------------------------
		Vector3 OdeCollisionGeomPlane::planeGetNormal() const
		{
			dVector4 params;
			static_cast<dPlane*>(mOdeGeom)->getParams(params);
			return Vector3( params[0], params[1], params[2] );
		}
		//-----------------------------------------------------
		real OdeCollisionGeomPlane::planeGetDistance() const
		{
			dVector4 params;
			static_cast<dPlane*>(mOdeGeom)->getParams(params);
			return params[3];
		}

		//-----------------------------------------------------
		OdeCollisionGeomTransform::OdeCollisionGeomTransform(dSpace* space) :
				mWrappedGeom(0)
		{
			mOdeSpace = space;
			mType = CGT_TRANSFORM;
			YAKE_ASSERT( space ).debug("Need a valid space.");

			mOdeGeom = new dGeomTransform( (*space).id() );
			mOdeGeomID = mOdeGeom->id();
			dGeomTransformSetCleanup( mOdeGeomID, 1 );
			dGeomTransformSetInfo( mOdeGeomID, 1 );
		}
		//-----------------------------------------------------
		void OdeCollisionGeomTransform::_setData(void * data)
		{
			YAKE_ASSERT( mOdeGeomID ).debug("Need a valid ODE geom id!");
			YAKE_ASSERT( mOdeGeom ).debug("Need a valid ODE geom!");
			//dGeomSetData( mOdeGeomID, data );
			mOdeGeom->setData( data );
			if (mWrappedGeom)
				mWrappedGeom->_setData( data );
		}
		//-----------------------------------------------------
		void OdeCollisionGeomTransform::tfAttachGeom( ICollisionGeometry* pGeom )
		{ 
			YAKE_ASSERT( pGeom );
			mWrappedGeom = static_cast<OdeCollisionGeomBase*>(pGeom);
			YAKE_ASSERT( mWrappedGeom ).debug("Need a valid collision geometry object in order to encapsulate it.");
			mWrappedGeom->_setData( this->_getData() );

			dSpace* pSpace = mWrappedGeom->_getOdeSpace();
			YAKE_ASSERT( pSpace );

			pSpace->remove( mWrappedGeom->_getOdeGeomID() );
			//static_cast<dGeomTransform*>(mOdeGeom)->setGeom( mWrappedGeom->_getOdeGeomID() );
			dGeomTransformSetGeom( mOdeGeomID, mWrappedGeom->_getOdeGeomID() );
			//dGeomSetPosition( mWrappedGeom->_getOdeGeomID(), 2, 1, 0 );
			//dGeomSetBody( mWrappedGeom->_getOdeGeomID(), dGeomGetBody(mOdeGeomID) );
			dGeomSetBody( mWrappedGeom->_getOdeGeomID(), 0 );
		}
		//-----------------------------------------------------
		ICollisionGeometry* OdeCollisionGeomTransform::tfGetAttachedGeom() const
		{ 
			return mWrappedGeom;
		}

		//-----------------------------------------------------
		// OdeCollisionGeomTriMesh
		//-----------------------------------------------------

		using namespace yake::base::templates;
		using namespace yake::data;

		class PhysicsMeshLoader
		{
		public:
			PhysicsMeshLoader();
			~PhysicsMeshLoader();

			void load( const base::String & meshfile );

			typedef base::templates::Vector<Vector3> PositionList;
			typedef base::templates::Vector<uint32> IndexList;
			typedef base::templates::Vector<Vector3> NormalList;

			const PositionList& getPositions() const;
			const IndexList& getIndices() const;
			const NormalList& getNormals() const;
		private:
			SharedPtr<data::dom::INode>		mDocNode;

			void parseMesh( const SharedPtr<data::dom::INode> & pMeshNode );
			void parseSubMeshes( const SharedPtr<data::dom::INode> & pSubMeshesNode );
			void parseSubMesh( const SharedPtr<data::dom::INode> & pSubMeshNode );
			void parseFaces( const SharedPtr<data::dom::INode> & pFacesNode, const uint32 indexOffset );
				void readFace( const SharedPtr<data::dom::INode> & pFaceNode, const uint32 indexOffset );
					void readNormal( const SharedPtr<data::dom::INode> & pNormalNode );
			void parseGeometry( const SharedPtr<data::dom::INode> & pGeometryNode );
				void readVertex( const SharedPtr<data::dom::INode> & pVertexNode );

			PositionList	mVertices;
			IndexList		mIndices;
			NormalList		mNormals;
		};
		PhysicsMeshLoader::PhysicsMeshLoader()
		{
		}
		PhysicsMeshLoader::~PhysicsMeshLoader()
		{
		}
		const PhysicsMeshLoader::PositionList& PhysicsMeshLoader::getPositions() const
		{ return mVertices; }
		const PhysicsMeshLoader::IndexList& PhysicsMeshLoader::getIndices() const
		{ return mIndices; }
		const PhysicsMeshLoader::NormalList& PhysicsMeshLoader::getNormals() const
		{ return mNormals; }
		void PhysicsMeshLoader::load( const base::String & meshfile )
		{
			data::dom::xml::XmlSerializer ser;
			ser.parse( meshfile, false );
			mDocNode = ser.getDocumentNode();
			data::dom::NodeList nodes = mDocNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it(nodes.begin(),nodes.end());
			/*
			base::String name = varGet<String>(mDocNode->getValue("name"));
			while (it.hasMoreElements())
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				//if (varGet<String>(pNode->getValue("name")) == "mesh")
					parseMesh( pNode );
			}
			*/
			parseMesh( mDocNode );
		}
		void PhysicsMeshLoader::parseMesh( const SharedPtr<data::dom::INode> & pMeshNode )
		{
			data::dom::NodeList nodes = pMeshNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it(nodes.begin(),nodes.end());
			while (it.hasMoreElements())
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				data::dom::INode::ValueType value = pNode->getValue("name");  
				if (varGet<String>( value ) == "submeshes")
					parseSubMeshes( pNode );
			}
		}
		void PhysicsMeshLoader::parseSubMeshes( const SharedPtr<data::dom::INode> & pSubMeshesNode )
		{
			data::dom::NodeList nodes = pSubMeshesNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it(nodes.begin(),nodes.end());
			while (it.hasMoreElements())
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				data::dom::INode::ValueType value = pNode->getValue("name"); 
				if (varGet<String>( value ) == "submesh")
					parseSubMesh( pNode );
			}
		}
		void PhysicsMeshLoader::parseSubMesh( const SharedPtr<data::dom::INode> & pSubMeshNode )
		{
			uint32 indexOffset = mIndices.size();
			data::dom::NodeList nodes = pSubMeshNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it(nodes.begin(),nodes.end());
			while (it.hasMoreElements())
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				data::dom::INode::ValueType value = pNode->getValue("name"); 
				base::String name = varGet<String>( value );
				if (name == "faces")
					parseFaces( pNode, indexOffset );
				else if (name == "geometry")
					parseGeometry( pNode );
			}
		}
		void PhysicsMeshLoader::parseFaces( const SharedPtr<data::dom::INode> & pFacesNode, const uint32 indexOffset )
		{
			data::dom::NodeList nodes = pFacesNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it(nodes.begin(),nodes.end());
			while (it.hasMoreElements())
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				data::dom::INode::ValueType value = pNode->getValue("name"); 
				base::String name = varGet<String>( value );
				YAKE_ASSERT( name == "face" );
				readFace( pNode, indexOffset );
			}
		}
		void PhysicsMeshLoader::readFace( const SharedPtr<data::dom::INode> & pFaceNode, const uint32 indexOffset )
		{
			YAKE_ASSERT( pFaceNode->getAttributeValue( "v1" ).which() == data::dom::INode::VTID_STRING );
			data::dom::INode::ValueType v1 = pFaceNode->getAttributeValue( "v1" );  
			String a = varGet<String>( v1 );
			mIndices.push_back( indexOffset + atoi(a.c_str()) );
			
			data::dom::INode::ValueType v2 = pFaceNode->getAttributeValue( "v2" );
			a = varGet<String>( v2 );
			mIndices.push_back( indexOffset + atoi(a.c_str()) );
			
			data::dom::INode::ValueType v3 = pFaceNode->getAttributeValue( "v3" );
			a = varGet<String>( v3 );
			mIndices.push_back( indexOffset + atoi(a.c_str()) );

			data::dom::NodeList nodes = pFaceNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it( nodes.begin(), nodes.end() );
			while ( it.hasMoreElements() )
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				data::dom::INode::ValueType value = pNode->getValue( "name" );
				base::String name = varGet<String>( value );
				YAKE_ASSERT( name == "normal" );
				readNormal( pNode );
			}
		}
		void PhysicsMeshLoader::parseGeometry(const SharedPtr<data::dom::INode> & pGeometryNode)
		{
			data::dom::NodeList nodes = pGeometryNode->getNodes();
			ConstVectorIterator<data::dom::NodeList> it(nodes.begin(),nodes.end());
			while (it.hasMoreElements())
			{
				SharedPtr<data::dom::INode> const& pNode = it.getNext();
				data::dom::INode::ValueType value = pNode->getValue( "name" );
				base::String name = varGet<String>( value );
				YAKE_ASSERT( name == "vertex" );
				readVertex( pNode );
			}
		}
		void PhysicsMeshLoader::readVertex( const SharedPtr<data::dom::INode> & pVertexNode )
		{
			SharedPtr<data::dom::INode> pPositionNode = pVertexNode->getNodeByName( "position" );
			YAKE_ASSERT( pPositionNode->getAttributeValue( "x" ).which() == data::dom::INode::VTID_STRING );

			Vector3 position;

			data::dom::INode::ValueType x = pPositionNode->getAttributeValue( "x" );
			String a = varGet<String>( x );
			position.x = StringUtil::toReal( a );

			data::dom::INode::ValueType y = pPositionNode->getAttributeValue( "y" );
			a = varGet<String>( y );
			position.y = StringUtil::toReal( a );

			data::dom::INode::ValueType z = pPositionNode->getAttributeValue( "z" );
			a = varGet<String>( z );
			position.z = StringUtil::toReal( a );

			mVertices.push_back( position );
		}
		void PhysicsMeshLoader::readNormal( const SharedPtr<data::dom::INode>& pNormalNode )
		{
			Vector3 normal;
			
			data::dom::INode::ValueType x = pNormalNode->getAttributeValue( "x" );
			String a = varGet< String >( x );
			normal.x = StringUtil::toReal( a );
			
			data::dom::INode::ValueType y = pNormalNode->getAttributeValue( "y" );
			a = varGet< String >( y );
			normal.y = StringUtil::toReal( a );
			
			data::dom::INode::ValueType z = pNormalNode->getAttributeValue( "z" );
			a = varGet< String >( z );
			normal.z = StringUtil::toReal( a );
			
			mNormals.push_back( normal );
		}

		//-----------------------------------------------------
		OdeCollisionGeomTriMesh::OdeCollisionGeomTriMesh(dSpace* space, const base::String & meshfile ) :
			mpVertices( 0 ),
			mpIndices( 0 ),
			mpNormals( 0 )
		{
			mType = CGT_MESH;
			mOdeGeomID = 0;
			YAKE_ASSERT( space );
			mSpaceId = space->id();
			_loadMesh( meshfile, true );
			_build( space );
		}
		//-----------------------------------------------------
		OdeCollisionGeomTriMesh::~OdeCollisionGeomTriMesh()
		{
			dGeomTriMeshDataDestroy( mDataId );
			dGeomDestroy( mOdeGeomID );
			YAKE_SAFE_DELETE( mpNormals );
			YAKE_SAFE_DELETE( mpVertices );
			YAKE_SAFE_DELETE( mpIndices );
		}
		//-----------------------------------------------------
		void OdeCollisionGeomTriMesh::_loadMesh(const base::String & meshfile, bool bAppend )
		{
			if (!bAppend)
			{
				mVertices.clear();
				mIndices.clear();
				mNormals.clear();
			}

			PhysicsMeshLoader loader;
			loader.load( meshfile );
			const PhysicsMeshLoader::IndexList & indices = loader.getIndices();
			const PhysicsMeshLoader::PositionList & positions = loader.getPositions();
			const PhysicsMeshLoader::NormalList & normals = loader.getNormals();

			uint32 indexOffset = mIndices.size();
			ConstVectorIterator<PhysicsMeshLoader::IndexList> itI(indices.begin(),indices.end());
			while (itI.hasMoreElements())
			{
				mIndices.push_back( indexOffset + itI.getNext() );
			}

			ConstVectorIterator<PhysicsMeshLoader::PositionList> itP(positions.begin(),positions.end());
			while (itP.hasMoreElements())
			{
				Vector3 const& position = itP.getNext();
				mVertices.push_back( CMVertex(position.x, position.y, position.z) );
			}

			ConstVectorIterator<PhysicsMeshLoader::NormalList> itN(normals.begin(),normals.end());
			while (itN.hasMoreElements())
			{
				Vector3 const& normal = itN.getNext();
				mNormals.push_back( normal );
			}
/*
			dReal dim = 100;

			CMVertex v(-dim,0,-dim);
			mVertices.push_back( v );
			v = CMVertex( dim,0, dim);
			mVertices.push_back( v );
			v = CMVertex( -dim,0,dim);
			mVertices.push_back( v );
			mIndices.push_back( 0 );
			mIndices.push_back( 2 );
			mIndices.push_back( 1 );

			v = CMVertex(-dim,0,-dim);
			mVertices.push_back( v );
			v = CMVertex( dim,0,-dim);
			mVertices.push_back( v );
			v = CMVertex( dim,0, dim);
			mVertices.push_back( v );
			mIndices.push_back( 3 );
			mIndices.push_back( 5 );
			mIndices.push_back( 4 );
*/
		}
		//-----------------------------------------------------
		void OdeCollisionGeomTriMesh::_build(dSpace* space)
		{
			YAKE_ASSERT( mVertices.size() > 0 );
			YAKE_ASSERT( mIndices.size() > 0 );

			YAKE_SAFE_DELETE( mpVertices );
			mpVertices = new dReal[ 3*mVertices.size() ];
			uint32 i = 0;
			base::templates::ConstVectorIterator<VertexList> itV( mVertices.begin(), mVertices.end() );
			while (itV.hasMoreElements())
			{
				CMVertex const& v = itV.getNext();
				mpVertices[i*3+0] = v.x;
				mpVertices[i*3+1] = v.y;
				mpVertices[i*3+2] = v.z;
				++i;
			}

			YAKE_SAFE_DELETE( mpIndices );
			mpIndices = new uint32[ mIndices.size() ];
			i = 0;
			base::templates::ConstVectorIterator<IndexList> itI( mIndices.begin(), mIndices.end() );
			while (itI.hasMoreElements())
			{
				mpIndices[i++] = itI.getNext();
			}

			YAKE_SAFE_DELETE( mpNormals );
			if (mNormals.size() > 0) // normals are optional
			{
				mpNormals = new dReal[ 3*mNormals.size() ];
				uint32 i = 0;
				base::templates::ConstVectorIterator<NormalList> itN( mNormals.begin(), mNormals.end() );
				while (itN.hasMoreElements())
				{
					const Vector3& v = itN.getNext();
					mpNormals[i*3+0] = v.x;
					mpNormals[i*3+1] = v.y;
					mpNormals[i*3+2] = v.z;
					++i;
				}
			}

			mDataId = dGeomTriMeshDataCreate();

			dGeomTriMeshDataBuildDouble1( mDataId,
				mpVertices,
				sizeof(dReal)*3,
				mVertices.size(),
				mpIndices,
				mIndices.size(),
				sizeof(uint32)*3,
				mpNormals );

			this->mOdeGeomID = dCreateTriMesh( mSpaceId, mDataId, 0, 0, 0 );
			dGeomTriMeshEnableTC( mOdeGeomID, dSphereClass, 1 );
			dGeomTriMeshEnableTC( mOdeGeomID, dBoxClass, 1 );
		}


	}
}
