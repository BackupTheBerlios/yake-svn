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

namespace yake {
	namespace physics {

		//-----------------------------------------------------
		OdeComplexObject::OdeComplexObject( OdeWorld * world ) : mWorld( world ), mBody(0), mSoftness( 0.01 ), mFriction( 5.0 ), mFriction2( 2.0 )
		{
			YAKE_ASSERT( mWorld ).debug("Need a world, pal!");
		}

		//-----------------------------------------------------
		OdeComplexObject::~OdeComplexObject()
		{
			mGeoms.clear(); // trigger deletion of collision geometry.
			YAKE_SAFE_DELETE( mBody );
		}

		//-----------------------------------------------------
		IBody* OdeComplexObject::getBody() const
		{
			return mBody;
		}

		//-----------------------------------------------------
		void OdeComplexObject::setBody(IBody* body)
		{
			mBody = static_cast< OdeBody* >(body);
			for (GeomList::const_iterator it = mGeoms.begin(); it != mGeoms.end(); ++it)
			{
				OdeCollisionGeomBase* pGeomBase = static_cast<OdeCollisionGeomBase*>(*it);
				YAKE_ASSERT( pGeomBase );
				if (mBody)
					dGeomSetBody( pGeomBase->_getOdeGeomID(), mBody->_getOdeBody()->id() );
				else
					dGeomSetBody( pGeomBase->_getOdeGeomID(), 0 );
			}
		}

		//-----------------------------------------------------
		void OdeComplexObject::_updateCollisionGeometry()
		{
			if (mBody)
			{
				for (GeomList::const_iterator it = mGeoms.begin(); it != mGeoms.end(); ++it)
				{
					(*it)->setPosition( mBody->getPosition() );
					(*it)->setOrientation( mBody->getOrientation() );
				}
			}
		}

		//-----------------------------------------------------
		void OdeComplexObject::setPosition( const Vector3 & position)
		{
			if (mBody)
			{
				mBody->setPosition( position );
			}
			for (GeomList::const_iterator it = mGeoms.begin(); it != mGeoms.end(); ++it)
				(*it)->setPosition( position );
		}

		//-----------------------------------------------------
		Vector3 OdeComplexObject::getPosition() const
		{
			YAKE_ASSERT( mBody ).warning("Need a valid body for this operation!");
			if (mBody)
				return mBody->getPosition();
			return Vector3::kZero;
		}

		//-----------------------------------------------------
		void OdeComplexObject::setOrientation( const Quaternion & orientation )
		{
			if (mBody)
			{
				mBody->setOrientation( orientation );
			}
			for (GeomList::const_iterator it = mGeoms.begin(); it != mGeoms.end(); ++it)
				(*it)->setOrientation( orientation );
		}

		//-----------------------------------------------------
		Quaternion OdeComplexObject::getOrientation() const
		{
			YAKE_ASSERT( mBody ).warning("Need a valid body for this operation!");
			if (mBody)
				return mBody->getOrientation();
			return Quaternion::kIdentity;
		}

		//-----------------------------------------------------
		void OdeComplexObject::addCollisionGeometry(ICollisionGeometry* geom)
		{
			YAKE_ASSERT( geom ).debug("Invalid pointer.");
			if (!geom)
				return;

			OdeCollisionGeomBase* pGeomBase = static_cast<OdeCollisionGeomBase*>( geom );

			int geomClass = dGeomGetClass(pGeomBase->_getOdeGeomID());
			if (geomClass != dPlaneClass && geomClass != dGeomTransformClass) 
			{
				/*
				// Now wrap the geometry object in a transformation object. This allows
				// us to offset the wrapped object relative to its original point of reference.

				dSpaceRemove( mWorld->_getOdeSpace()->id(), pGeomBase->_getOdeGeomID() );

				Internal::OdeCollisionGeomTransform* pGeomTransform = new Internal::OdeCollisionGeomTransform( mWorld->_getOdeSpace(), pGeomBase );
				pGeomTransform->_setData( this );

				if ( mBody )
				{
					pGeomTransform->setPosition( mBody->getPosition() );
					pGeomTransform->setOrientation( mBody->getOrientation() );

					YAKE_ASSERT( mBody->_getOdeBody() ).debug("Help! No ODE body where on is expected!");
					dGeomSetBody( pGeomBase->_getOdeGeomID(), mBody->_getOdeBody()->id() );
				}
				mGeoms.push_back( RefPtr< ICollisionGeometry >( pGeomTransform ) );
				*/

				if ( mBody )
					dGeomSetBody( pGeomBase->_getOdeGeomID(), mBody->_getOdeBody()->id() );
				else
					dGeomSetBody( pGeomBase->_getOdeGeomID(), 0 );
			}
			dGeomSetData( pGeomBase->_getOdeGeomID(), this );
			mGeoms.push_back( geom );

		}

		//-----------------------------------------------------
		void OdeComplexObject::setFriction(real f)
		{
			mFriction = f;
		}

		//-----------------------------------------------------
		void OdeComplexObject::setFriction2(real f)
		{
			mFriction2 = f;
		}

		//-----------------------------------------------------
		void OdeComplexObject::setSoftness(real s)
		{
			mSoftness = s;
		}

		//-----------------------------------------------------
		real OdeComplexObject::getFriction() const
		{
			return mFriction;
		}

		//-----------------------------------------------------
		real OdeComplexObject::getFriction2() const
		{
			return mFriction2;
		}

		//-----------------------------------------------------
		real OdeComplexObject::getSoftness() const
		{
			return mSoftness;
		}

		//-----------------------------------------------------
		void OdeComplexObject::subscribeToEnterCollision( const EnterCollisionSignal::slot_type & slot )
		{
			mEnterCollisionSignal.connect( slot );
		}

		//-----------------------------------------------------
		void OdeComplexObject::subscribeToLeaveCollision( const LeaveCollisionSignal::slot_type & slot )
		{
			mLeaveCollisionSignal.connect( slot );
		}

		//-----------------------------------------------------
		void OdeComplexObject::_collide(OdeComplexObject* pOther, dGeomID geomA, dGeomID geomB, dJointGroup * contactJointGroup)
		{
			YAKE_ASSERT( pOther ).debug("Need the other object participating in the collision!");
			YAKE_ASSERT( contactJointGroup ).error("Need a joint group for contact joints!");

			if ((dGeomGetClass(geomA) == dRayClass) || (dGeomGetClass(geomB) == dRayClass))
				return;

			//
			float softness = this->getSoftness() + pOther->getSoftness();

			//
			const int N = 40;
			dContact contact[N];

			int n = dCollide (geomA,geomB,N,&contact[0].geom,sizeof(dContact));

			if (n > 0)
			{
				for (int i=0; i<n; i++) 
				{
					contact[i].surface.mode = //dContactSlip1
											//|dContactSlip2
											dContactSoftERP
											//|dContactSoftCFM
											//|dContactBounce
											|dContactApprox1
											//|dContactMu2
											;

					if (softness > 0)
					{
						contact[i].surface.mode |= dContactSoftCFM;
						contact[i].surface.soft_cfm = softness;
					}
					contact[i].surface.mu = std::min(this->getFriction(), pOther->getFriction());
					float friction2 = std::min(this->getFriction2(), pOther->getFriction2());
					if (friction2 > 0.f)
					{
						contact[i].surface.mode |= dContactMu2;
						contact[i].surface.mu2 = friction2; // 0 or 10
					}
					//contact[i].surface.slip1 = 0.1;
					//contact[i].surface.slip2 = 0.1;
					contact[i].surface.soft_erp = 0.8; // 0.8
					//contact[i].surface.bounce = 0.05; //0.05
					//contact[i].surface.bounce_vel = 0.025; //0.025
					
					//for (int j=1; j<=mNumJointsPerContact; ++j)
					{
						dJointID c = dJointCreateContact (
									mWorld->_getOdeID(),
									contactJointGroup->id(),
									contact+i);
						dJointAttach (c,dGeomGetBody(geomA),dGeomGetBody(geomB));
					}
				}
			}
		}

	}
}