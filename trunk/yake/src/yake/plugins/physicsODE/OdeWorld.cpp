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
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeJoint.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeComplexObject.h>
#include <yake/plugins/physicsODE/OdeCollisionGeometry.h>

#define ADJUST_FPU_PRECISION

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		OdeWorld::OdeWorld()
		{
			mStepSize = 1. / 60.;	// default: 50Hz
			mOdeWorld = new dWorld();
			mOdeSpace = new dSimpleSpace( 0 );
			//mOdeSpace = new dHashSpace( 0 );
			mOdeContactGroup = new dJointGroup( 0 );

			mOdeWorld->setGravity( 0., 0., 0. );
			mOdeWorld->setCFM( 0.025 );
			mOdeWorld->setERP( 0.99 );
			mOdeWorld->setAutoDisableFlag( 1 );
			mOdeWorld->setAutoDisableAngularThreshold( 0.005 ); // ODE default: 0.01
			mOdeWorld->setAutoDisableLinearThreshold( 0.005 ); // ODE default: 0.01
			mOdeWorld->setAutoDisableSteps( 10 ); // ODE default: 10
			mOdeWorld->setAutoDisableTime( 0 ); // ODE default: 0. (= ignore time)

			dWorldSetQuickStepNumIterations( mOdeWorld->id(), 20 );

			dWorldSetContactMaxCorrectingVel( mOdeWorld->id(), 2. );
			dWorldSetContactSurfaceLayer( mOdeWorld->id(), 0.0075 );
		}

		//-----------------------------------------------------
		OdeWorld::~OdeWorld()
		{
			YAKE_SAFE_DELETE( mOdeContactGroup );
			YAKE_SAFE_DELETE( mOdeSpace );
			YAKE_SAFE_DELETE( mOdeWorld);
		}

		//-----------------------------------------------------
		void OdeWorld::setGlobalGravity( const Vector3 & acceleration )
		{
			if (mOdeWorld)
				mOdeWorld->setGravity( acceleration.x, acceleration.y, acceleration.z );
		}

		//-----------------------------------------------------
		void OdeWorld::update( const real timeElapsed )
		{
			static real overflow = 0.;
			real t = overflow + timeElapsed;
			while ( t >= 0. )
			{
				t -= mStepSize;
		#ifdef ADJUST_FPU_PRECISION
				_controlfp(_PC_64, _MCW_PC);
		#endif
				// Now collide the objects in the world
				dSpaceCollide(mOdeSpace->id(), this, &_OdeNearCallback);

				mOdeWorld->step( mStepSize );
				//mOdeWorld->stepFast1( mStepSize, 4 );
				dWorldQuickStep( mOdeWorld->id(), mStepSize );
				//dWorldStepFast1( mOdeWorld->id(), mStepSize, 4 );

		#ifdef ADJUST_FPU_PRECISION
				_controlfp(_PC_24, _MCW_PC);
		#endif

				// Clear contacts
				mOdeContactGroup->empty();

				//mPostStepSignal( mStepSize );
			}
			mPostStepSignal( timeElapsed );

			overflow = t;
		}

		//-----------------------------------------------------
		IBody* OdeWorld::createBody()
		{
			OdeBody* pBody = new OdeBody( this );
			YAKE_ASSERT( pBody ).debug("Out of memory ?");
			if (!pBody)
				return 0;

			YAKE_ASSERT( pBody ).debug("Body couldn't be initialised!");
			if (!pBody->_isValid())
			{
				delete pBody;
				return 0;
			}
			mBodies.push_back( pBody );
			return pBody;
		}

		//-----------------------------------------------------
		ICollisionGeometry* OdeWorld::createCollisionGeomSphere(real radius)
		{
			ICollisionGeometry* pGeom = new OdeCollisionGeomSphere( mOdeSpace, radius );
			YAKE_ASSERT( pGeom ).debug("Out of memory ?");
			return pGeom;
		}

		//-----------------------------------------------------
		ICollisionGeometry* OdeWorld::createCollisionGeomBox(real lx, real ly, real lz)
		{
			ICollisionGeometry* pGeom = new OdeCollisionGeomBox( mOdeSpace, lx, ly, lz );
			YAKE_ASSERT( pGeom ).debug("Out of memory ?");
			return pGeom;
		}

		//-----------------------------------------------------
		ICollisionGeometry* OdeWorld::createCollisionGeomMesh( const base::String & collisionMeshResourceName )
		{
			ICollisionGeometry* pGeom = new OdeCollisionGeomTriMesh( mOdeSpace, collisionMeshResourceName );
			YAKE_ASSERT( pGeom ).debug("Out of memory ?");
			return pGeom;
		}

		//-----------------------------------------------------
		dWorldID OdeWorld::_getOdeID() const
		{
			YAKE_ASSERT( mOdeWorld ).debug("Need a world!");
			return mOdeWorld->id();
		}

		//-----------------------------------------------------
		IJoint* OdeWorld::createJoint(IJoint::JointType type, IJointGroup* group)
		{
			IJoint* pJoint = 0;
			if (type == IJoint::JT_BALL)
				pJoint = new OdeBallJoint( this, 0 );
			else if (type == IJoint::JT_HINGE)
				pJoint = new OdeHingeJoint( this, 0 );
			else if (type == IJoint::JT_HINGE2)
				pJoint = new OdeHinge2Joint( this, 0 );
			else if (type == IJoint::JT_FIXED)
				pJoint = new OdeFixedJoint( this, 0 );
			else
			{
				YAKE_ASSERT( 1==0 ).error("Unhandled joint type!");
				return 0;
			}
			YAKE_ASSERT( pJoint ).debug("Out of memory ?");
			return pJoint;
		}

		//-----------------------------------------------------
		IComplexObject* OdeWorld::createEmptyPhysicsObject()
		{
			// create the collidable object, but no body and no collision geometry!
			OdeComplexObject* pComplex = new OdeComplexObject( this );
			YAKE_ASSERT( pComplex ).debug("Out of memory ?");
			return pComplex;
		}

		//-----------------------------------------------------
		IComplexObject* OdeWorld::createPlane(const Vector3 & n, real d)
		{
			OdeComplexObject* pComplex = new OdeComplexObject( this );
			YAKE_ASSERT( pComplex ).debug("Out of memory ?");

			OdeCollisionGeomPlane* pPlane = new OdeCollisionGeomPlane( mOdeSpace, n.x, n.y, n.z, d );
			YAKE_ASSERT( pPlane ).debug("Out of memory ?");

			pComplex->addCollisionGeometry( pPlane );

			return pComplex;
		}

		//-----------------------------------------------------
		IComplexObject* OdeWorld::createSphere(real radius)
		{
			OdeComplexObject* pComplex = new OdeComplexObject( this );
			YAKE_ASSERT( pComplex ).debug("Out of memory ?");

			OdeCollisionGeomSphere* pShere = new OdeCollisionGeomSphere( mOdeSpace, radius );
			YAKE_ASSERT( pShere ).debug("Out of memory ?");

			OdeBody* pBody = new OdeBody( this );
			YAKE_ASSERT( pBody ).debug("Out of memory ?");

			pComplex->setBody( pBody );
			pComplex->addCollisionGeometry( pShere );

			return pComplex;
		}

		//-----------------------------------------------------
		IComplexObject* OdeWorld::createBox(real lx, real ly, real lz)
		{
			OdeComplexObject* pComplex = new OdeComplexObject( this );
			YAKE_ASSERT( pComplex ).debug("Out of memory ?");

			OdeCollisionGeomBox* pBox = new OdeCollisionGeomBox( mOdeSpace, lx, ly, lz );
			YAKE_ASSERT( pBox ).debug("Out of memory ?");

			OdeBody* pBody = new OdeBody( this );
			YAKE_ASSERT( pBody ).debug("Out of memory ?");

			pComplex->setBody( pBody );
			pComplex->addCollisionGeometry( pBox );

			return pComplex;
		}

		//-----------------------------------------------------
		IComplexObject* OdeWorld::createMesh(const base::String & mesh)
		{
			OdeComplexObject* pComplex = new OdeComplexObject( this );
			YAKE_ASSERT( pComplex ).debug("Out of memory ?");

			OdeCollisionGeomTriMesh* pMesh = new OdeCollisionGeomTriMesh( mOdeSpace, mesh );
			YAKE_ASSERT( pMesh ).debug("Out of memory ?");

			pComplex->addCollisionGeometry( pMesh );

			return pComplex;
		}

		//-----------------------------------------------------
		IComplexObject* OdeWorld::createRay(const Vector3 & origin, const Quaternion & orientation)
		{
			YAKE_ASSERT( 1==0 ).debug("UNSUPPORT COMPLEX OBECT");
			return 0;
		}

		//-----------------------------------------------------
		void OdeWorld::_OdeNearCallback (void *data, dGeomID o1, dGeomID o2)
		{
			// make sure we traverse through all spaces and sub-spaces
			if (dGeomIsSpace (o1) || dGeomIsSpace (o2))
			{
				// colliding a space with something
				dSpaceCollide2 (o1,o2,data,&_OdeNearCallback);

				// collide all geoms internal to the space(s)
				if (dGeomIsSpace (o1)) dSpaceCollide (dSpaceID(o1),data,&_OdeNearCallback);
				if (dGeomIsSpace (o2)) dSpaceCollide (dSpaceID(o2),data,&_OdeNearCallback);
			}

			// colliding two non-space geoms, so generate contact joints
			////
			dJointGroup* odeContactGroup = (static_cast<OdeWorld*>(data))->_getOdeContactJointGroup();
			dWorldID odeWorldID = (static_cast<OdeWorld*>(data))->_getOdeID();

			// exit without doing anything if the two bodies are connected by a joint
			dBodyID b1 = dGeomGetBody(o1);
			dBodyID b2 = dGeomGetBody(o2);
			if (b1 && b2 && dAreConnected (b1,b2))
				return;

			int t1 = dGeomGetClass(o1);
			int t2 = dGeomGetClass(o2);

			OdeComplexObject* pA = static_cast<OdeComplexObject*>(dGeomGetData(o1));
			OdeComplexObject* pB = static_cast<OdeComplexObject*>(dGeomGetData(o2));
			
			if (pA && pB)
			{
				// collide
				pA->_collide( pB, o1, o2, odeContactGroup );
				return;
			}

			//FALLBACK
			/*

			const int N = 40;
			dContact contact[N];

			int n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));

			if (n > 0)
			{
				for (int i=0; i<n; i++) 
				{
					contact[i].surface.mode = dContactSlip1
											|dContactSlip2
											//|dContactSoftERP
											//|dContactSoftCFM
											//|dContactBounce
											//|dContactApprox1
											;
					contact[i].surface.mu = 20; // 60
					contact[i].surface.mu2 = 0; // 0 or 10
					contact[i].surface.slip1 = 0.1;
					contact[i].surface.slip2 = 0.1;
					//contact[i].surface.soft_erp = 0.8; // 0.8
					//contact[i].surface.soft_cfm = 0.01; // 0.01
					//contact[i].surface.bounce = 0.05; //0.3
					//contact[i].surface.bounce_vel = 0.025; //0.1
					dJointID c = dJointCreateContact (odeWorldID,odeContactGroup->id(),contact+i);
					dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
				}
			}
			*/
		}

	}
}