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
#ifndef YAKE_ODESHAPES_H
#define YAKE_ODESHAPES_H

#include <yake/plugins/physicsODE/OdeMaterial.h>

namespace yake {
namespace physics {

		class OdeGeom
		{
		public:
			OdeGeom();
			virtual ~OdeGeom();
			
			virtual dGeomID _getOdeGeomID() const;
			virtual dSpace* _getOdeSpace() const;
			virtual void _setData( void* pData );
			virtual void* _getData() const;
			virtual dGeom* _getOdeGeomPtr();

			const OdeMaterial&	getMaterial() const;
			void setMaterial( OdeMaterial const& rMaterial );
			
		protected:
			dGeomID			mOdeGeomID;
			dSpace*				mOdeSpace;
			dGeom*				mOdeGeom;
			OdeMaterial			mMaterial;
		};
		
		class OdeStaticGeom : public OdeGeom, public IShape
		{
		};
		
		class OdeMovableGeom : public OdeGeom, public IMovableShape
		{
		public:
			virtual void setPosition( Vector3 const& rPosition);
			virtual Vector3 getPosition() const;
			virtual void setOrientation( Quaternion const& rOrientation );
			virtual Quaternion getOrientation() const;
		};
		
		class OdeTransformGeom : public OdeMovableGeom
		{
		public:
			OdeTransformGeom( dSpace* pSpace );
			
			virtual void _setData( void* pData );
			
			virtual ShapeType getType() const;
			
			void attachGeom( OdeGeom* pGeom );
			OdeGeom* getAttachedGeom() const;
		
		private:
			OdeGeom*	mAttachedGeom;
		};
		
		class OdePlane : public OdeStaticGeom
		{
		public:
			OdePlane( dSpace* pSpace, real a, real b, real c, real d );
			
			virtual ShapeType getType() const;
		};
		
		class OdeSphere :  public OdeMovableGeom
		{
		public:
			OdeSphere::OdeSphere( dSpace* pSpace, real radius );
			
			virtual ShapeType getType() const;
		};
		
		class OdeBox : public OdeMovableGeom
		{
		public:
			OdeBox( dSpace* pSpace, real sizex, real sizey, real sizez);
			
			virtual ShapeType getType() const;
		};
		
		class OdeCCylinder : public OdeMovableGeom
		{
		public:
			OdeCCylinder( dSpace* pSpace, real radius, real length );
			
			virtual ShapeType getType() const;
		};
		
		class OdeTriMesh : public OdeMovableGeom
		{
		public:
			OdeTriMesh::OdeTriMesh( dSpace* pSpace, dTriMeshDataID meshDataId ); 
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
