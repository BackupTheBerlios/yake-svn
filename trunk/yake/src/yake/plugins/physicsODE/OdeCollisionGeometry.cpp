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

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		Vector3 OdeCollisionGeomBase::planeGetNormal() const
		{ return Vector3::kZero; }
		//-----------------------------------------------------
		real OdeCollisionGeomBase::planeGetDistance() const
		{ return 0; }
		//-----------------------------------------------------
		real OdeCollisionGeomBase::sphereGetRadius() const
		{ return 0; }
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
			if (mOdeGeomID)
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
		OdeCollisionGeomBox::OdeCollisionGeomBox(dSpace* space, float lx, float ly, float lz)
		{
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
		Internal::OdeCollisionGeomTransform::OdeCollisionGeomTransform(dSpace* space, OdeCollisionGeomBase* geom) : mWrappedGeom(geom)
		{
			YAKE_ASSERT( space ).debug("Need a valid space.");
			YAKE_ASSERT( mWrappedGeom ).debug("Need a valid collision geometry object in order to encapsulate it.");

			mOdeGeom = new dGeomTransform( *space );
			mOdeGeomID = mOdeGeom->id();
			static_cast<dGeomTransform*>(mOdeGeom)->setGeom( mWrappedGeom->_getOdeGeomID() );
		}

	}
}