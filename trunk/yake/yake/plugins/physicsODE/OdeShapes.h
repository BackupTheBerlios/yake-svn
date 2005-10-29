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
#ifndef YAKE_ODESHAPES_H
#define YAKE_ODESHAPES_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>
#include <yake/plugins/physicsODE/OdeMaterial.h>

namespace yake {
namespace physics {

		class OdeActor;
		class OdeTransformGeom;
		class YAKE_PHYSICSODE_API OdeGeom : public IShape
		{
			friend class OdeTransformGeom;
			OdeGeom();
			//OdeGeom(const OdeGeom&);
		public:
			OdeGeom(OdeActor* pOwner);
			virtual ~OdeGeom();
			
			virtual dGeomID _getOdeGeomID() const;
			virtual dSpace* _getOdeSpace() const;
			virtual dGeom* _getOdeGeomPtr();

			OdeMaterial* getMaterial() const;
			void setMaterial( OdeMaterial* pMaterial );

			OdeActor* getOwner() const
			{ return mOwner; }

			virtual void setPosition( Vector3 const& rPosition);
			virtual Vector3 getPosition() const;
			virtual void setOrientation( Quaternion const& rOrientation );
			virtual Quaternion getOrientation() const;

			virtual Vector3 getDerivedPosition() const;
			virtual Quaternion getDerivedOrientation() const;

			virtual Vector3 getPropertyVector3(const String&) const;
			virtual real getPropertyReal(const String&) const;

		protected:
			virtual void _setData( void* pData );
			virtual void* _getData() const;

		protected:
			dGeomID				mOdeGeomID;
			dSpace*				mOdeSpace;
			dGeom*				mOdeGeom;
			OdeMaterial*		mMaterial;
			OdeActor*			mOwner;
			typedef std::map<String,boost::any> PropMap;
			PropMap				mProps;
		};
		
		class YAKE_PHYSICSODE_API OdeTransformGeom : public OdeGeom
		{
		public:
			OdeTransformGeom( dSpace* pSpace, OdeActor* pOwner );
			
			virtual ShapeType getType() const;
			
			virtual void setPosition( Vector3 const& rPosition);
			virtual Vector3 getPosition() const;
			virtual void setOrientation( Quaternion const& rOrientation );
			virtual Quaternion getOrientation() const;

			void attachGeom( OdeGeom* pGeom );
			OdeGeom* getAttachedGeom() const;
		
			virtual Vector3 getDerivedPosition() const;
			virtual Quaternion getDerivedOrientation() const;
		private:
			OdeGeom*	mAttachedGeom;
		};
		
		class YAKE_PHYSICSODE_API OdePlane : public OdeGeom
		{
		public:
			OdePlane( dSpace* pSpace, OdeActor* pOwner, real a, real b, real c, real d );
			
			virtual ShapeType getType() const;
		};
		
		class YAKE_PHYSICSODE_API OdeSphere :  public OdeGeom
		{
		public:
			OdeSphere::OdeSphere( dSpace* pSpace, OdeActor* pOwner, real radius );
			
			virtual ShapeType getType() const;
		};
		
		class YAKE_PHYSICSODE_API OdeBox : public OdeGeom
		{
		public:
			OdeBox( dSpace* pSpace, OdeActor* pOwner, real sizex, real sizey, real sizez);
			
			virtual ShapeType getType() const;
		};
		
		class YAKE_PHYSICSODE_API OdeCCylinder : public OdeGeom
		{
		public:
			OdeCCylinder( dSpace* pSpace, OdeActor* pOwner, real radius, real length );
			
			virtual ShapeType getType() const;
		};
		
		class YAKE_PHYSICSODE_API OdeTriMesh : public OdeGeom
		{
			public:

				struct MeshData
				{
					MeshData() : id( NULL ), vertices( NULL ), indices( NULL ), normals( NULL )
					{}

					dTriMeshDataID id;

				/// We need to store these here 'cause ODE doesn't copy data on mesh data construction
					dReal*		vertices;
					uint32*		indices;
					dReal*		normals;
				};

				OdeTriMesh( dSpace* pSpace, OdeActor* pOwner, dTriMeshDataID meshDataId );
				virtual ~OdeTriMesh();

				virtual ShapeType getType() const;

				static void buildMeshData(	MeshData& dataToFill,
													TriangleMeshDesc::VertexVector const& rVertices,
													TriangleMeshDesc::IndexVector const& rIndices,
													TriangleMeshDesc::NormalVector const& rNormals );
		};
		
} // physics
} // yake

#endif
