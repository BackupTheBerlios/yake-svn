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
#ifndef YAKE_ODECOLLISIONGEOM_H
#define YAKE_ODECOLLISIONGEOM_H

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		class OdeCollisionGeomBase : public ICollisionGeometry
		{
		protected:
			// do not allow instantiation of this class.
			OdeCollisionGeomBase();
			OdeCollisionGeomBase( const OdeCollisionGeomBase & );
		public:
			virtual ~OdeCollisionGeomBase();

			// override
			virtual ICollisionGeometry::CollisonGeomType getType() const;

			virtual void setPosition(const Vector3 & position);
			virtual Vector3 getPosition() const;
			virtual void setOrientation(const Quaternion & orientation);
			virtual Quaternion getOrientation() const;

			virtual Vector3 planeGetNormal() const;
			virtual real planeGetDistance() const;
			virtual real sphereGetRadius() const;
			virtual Vector3 boxGetDimensions() const;
			virtual Vector3 rayGetOrigin() const;
			virtual Quaternion rayGetOrientation() const;
			virtual base::String meshGetName() const;

			// Helpers

			virtual dGeomID _getOdeGeomID() const
			{ return mOdeGeomID; }

			virtual void _setData(void * data);
			virtual void* _getData() const;

		protected:
			CollisonGeomType	mType;
			dGeomID				mOdeGeomID;
		};

		//-----------------------------------------------------
		class OdeCollisionGeom : public OdeCollisionGeomBase
		{
		protected:
			OdeCollisionGeom() : mOdeGeom( 0 )
			{ }
		public:
			virtual dGeom* _getOdeGeomPtr() { return mOdeGeom; }

		protected:
			dGeom*				mOdeGeom;
		};

		namespace Internal {
			//-----------------------------------------------------
			class OdeCollisionGeomTransform : public OdeCollisionGeom
			{
			private:
				OdeCollisionGeomTransform();
			public:
				// Ownership of the geom object passed in is transferred to this object!!
				OdeCollisionGeomTransform(dSpace* space, OdeCollisionGeomBase* geom);
			protected:
				OdeCollisionGeomBase	* mWrappedGeom;
			};
		}

		//-----------------------------------------------------
		class OdeCollisionGeomPlane : public OdeCollisionGeom
		{
		private:
			OdeCollisionGeomPlane();
		public:
			OdeCollisionGeomPlane(dSpace* space, real a, real b, real c, real d);
			virtual Vector3 planeGetNormal() const;
			virtual real planeGetDistance() const;
		};

		//-----------------------------------------------------
		class OdeCollisionGeomSphere : public OdeCollisionGeom
		{
		private:
			OdeCollisionGeomSphere();
		public:
			OdeCollisionGeomSphere(dSpace* space, real radius);
			virtual real sphereGetRadius() const;
		};

		//-----------------------------------------------------
		class OdeCollisionGeomBox : public OdeCollisionGeom
		{
		private:
			OdeCollisionGeomBox();
		public:
			OdeCollisionGeomBox(dSpace* space, real lx, real ly, real lz);
			virtual Vector3 boxGetDimensions() const;
		};

		//-----------------------------------------------------
		class OdeCollisionGeomTriMesh : public OdeCollisionGeomBase
		{
		private:
			OdeCollisionGeomTriMesh();
		public:
			OdeCollisionGeomTriMesh(dSpace* space, const base::String & meshfile/*, OdeCollisionTriMesh* mesh*/);
			virtual ~OdeCollisionGeomTriMesh();

		private:
			void _loadMesh(const base::String & meshfile, bool bAppend = true );
			void _build(dSpace* space);

		protected:
			dTriMeshDataID	mDataId;
			dSpaceID		mSpaceId;

			#pragma pack(1)
			struct CMVertex
			{
				CMVertex(dReal x_=0,dReal y_=0,dReal z_=0) : x(x_), y(y_), z(z_)
				{}
				dReal x,y,z;
				CMVertex& operator = (const CMVertex & rhs)
				{
					x = rhs.x;
					y = rhs.y;
					z = rhs.z;
					return *this;
				}
			};
			#pragma
			typedef unsigned long CMIndex;
			dReal*		mpVertices;
			uint32*		mpIndices;

			typedef base::templates::Vector<CMVertex> VertexList;
			typedef base::templates::Vector<CMIndex> IndexList;
			VertexList	mVertices;
			IndexList	mIndices;
		};

	}
}

#endif
