/**------------------------------------------------------------------------------------
This file is part of YAKE
Copyright � 2004 The YAKE Team
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
#ifndef YAKE_ODESHAPES_H
#define YAKE_ODESHAPES_H

#include <yake/plugins/physicsODE/OdeMaterial.h>

namespace yake {
namespace physics {

		class OdeActor;
		class OdeTransformGeom;
		class OdeGeom
		{
			friend class OdeTransformGeom;
			OdeGeom();
			OdeGeom(const OdeGeom&);
		public:
			OdeGeom(OdeActor* pOwner);
			virtual ~OdeGeom();
			
			virtual dGeomID _getOdeGeomID() const;
			virtual dSpace* _getOdeSpace() const;
			virtual dGeom* _getOdeGeomPtr();

			const OdeMaterial&	getMaterial() const;
			void setMaterial( OdeMaterial const& rMaterial );

			OdeActor* getOwner() const
			{ return mOwner; }

		protected:
			virtual void _setData( void* pData );
			virtual void* _getData() const;

		protected:
			dGeomID				mOdeGeomID;
			dSpace*				mOdeSpace;
			dGeom*				mOdeGeom;
			OdeMaterial			mMaterial;
			OdeActor*			mOwner;
		};
		
		class OdeStaticGeom : public OdeGeom, public IShape
		{
		protected:
			OdeStaticGeom( OdeActor* pOwner ) : OdeGeom( pOwner )
			{}
		public:
		};
		
		class OdeMovableGeom : public OdeGeom, public IMovableShape
		{
		protected:
			OdeMovableGeom( OdeActor* pOwner ) : OdeGeom( pOwner )
			{}
		public:
			virtual void setPosition( Vector3 const& rPosition);
			virtual Vector3 getPosition() const;
			virtual void setOrientation( Quaternion const& rOrientation );
			virtual Quaternion getOrientation() const;
		};
		
		class OdeTransformGeom : public OdeMovableGeom
		{
		public:
			OdeTransformGeom( dSpace* pSpace, OdeActor* pOwner );
			
			virtual ShapeType getType() const;
			
			void attachGeom( OdeGeom* pGeom );
			OdeGeom* getAttachedGeom() const;
		
		private:
			OdeGeom*	mAttachedGeom;
		};
		
		class OdePlane : public OdeStaticGeom
		{
		public:
			OdePlane( dSpace* pSpace, OdeActor* pOwner, real a, real b, real c, real d );
			
			virtual ShapeType getType() const;
		};
		
		class OdeSphere :  public OdeMovableGeom
		{
		public:
			OdeSphere::OdeSphere( dSpace* pSpace, OdeActor* pOwner, real radius );
			
			virtual ShapeType getType() const;
		};
		
		class OdeBox : public OdeMovableGeom
		{
		public:
			OdeBox( dSpace* pSpace, OdeActor* pOwner, real sizex, real sizey, real sizez);
			
			virtual ShapeType getType() const;
		};
		
		class OdeCCylinder : public OdeMovableGeom
		{
		public:
			OdeCCylinder( dSpace* pSpace, OdeActor* pOwner, real radius, real length );
			
			virtual ShapeType getType() const;
		};
		
		class OdeTriMesh : public OdeMovableGeom
		{
		public:
			OdeTriMesh::OdeTriMesh( dSpace* pSpace, OdeActor* pOwner, dTriMeshDataID meshDataId ); 
			virtual ~OdeTriMesh();

			virtual ShapeType getType() const;
			
			static dTriMeshDataID buildMeshData( TriangleMeshDesc::VertexVector const& rVertices,
						 		  	  			TriangleMeshDesc::IndexVector const& rIndices,
						 		  	 			TriangleMeshDesc::NormalVector const& rNormals );

 		
		protected:
 		  	TriangleMeshDesc::VertexVector		mVertices;
		 	TriangleMeshDesc::IndexVector		mIndices;
			TriangleMeshDesc::NormalVector		mNormals;
		};
		
} // physics
} // yake

#endif
