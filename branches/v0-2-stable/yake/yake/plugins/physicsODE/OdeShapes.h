/*
   ------------------------------------------------------------------------------------
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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_ODESHAPES_H
#define YAKE_ODESHAPES_H

#include <yake/plugins/physicsODE/OdeMaterial.h>

namespace yake {
namespace physics {

		class OdeActor;
		class OdeTransformGeom;
		class OdeGeom : public IShape
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

			virtual void setPosition( Vector3 const& rPosition);
			virtual Vector3 getPosition() const;
			virtual void setOrientation( Quaternion const& rOrientation );
			virtual Quaternion getOrientation() const;

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
		
		class OdeTransformGeom : public OdeGeom
		{
		public:
			OdeTransformGeom( dSpace* pSpace, OdeActor* pOwner );
			
			virtual ShapeType getType() const;
			
			void attachGeom( OdeGeom* pGeom );
			OdeGeom* getAttachedGeom() const;
		
		private:
			OdeGeom*	mAttachedGeom;
		};
		
		class OdePlane : public OdeGeom
		{
		public:
			OdePlane( dSpace* pSpace, OdeActor* pOwner, real a, real b, real c, real d );
			
			virtual ShapeType getType() const;
		};
		
		class OdeSphere :  public OdeGeom
		{
		public:
			OdeSphere::OdeSphere( dSpace* pSpace, OdeActor* pOwner, real radius );
			
			virtual ShapeType getType() const;
		};
		
		class OdeBox : public OdeGeom
		{
		public:
			OdeBox( dSpace* pSpace, OdeActor* pOwner, real sizex, real sizey, real sizez);
			
			virtual ShapeType getType() const;
		};
		
		class OdeCCylinder : public OdeGeom
		{
		public:
			OdeCCylinder( dSpace* pSpace, OdeActor* pOwner, real radius, real length );
			
			virtual ShapeType getType() const;
		};
		
		class OdeTriMesh : public OdeGeom
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
