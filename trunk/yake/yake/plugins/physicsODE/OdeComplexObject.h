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
#ifndef YAKE_ODECOMPLEXOBJECT_H
#define YAKE_ODECOMPLEXOBJECT_H


namespace yake {
	namespace physics {

		class OdeBody;
		class OdeWorld;
		class OdeComplexObject : public IComplexObject
		{
		public:
			OdeComplexObject( OdeWorld * world );
			virtual ~OdeComplexObject();

			virtual void setPosition( const Vector3 & position );
			virtual Vector3 getPosition() const;
			virtual void setOrientation( const Quaternion & orientation );
			virtual Quaternion getOrientation() const;

			virtual IBody* getBody() const;
			virtual void setBody( IBody* body );

			virtual void addCollisionGeometry( ICollisionGeometry* geom);
			virtual base::templates::Vector<ICollisionGeometry*> getCollisionGeometryVector() const;

			virtual void setFriction(real f);
			virtual void setFriction2(real f);
			virtual void setSoftness(real s);
			virtual real getFriction() const;
			virtual real getFriction2() const;
			virtual real getSoftness() const;

			virtual void setLateralSlipEnabled( bool enabled );
			//virtual void setSlipNormal( const Vector3 & slipNormal );
			virtual void setSlipNormalSource( ISlipNormalSource* pSource );
			virtual void setSlipCoefficients( const real linear, const real angular );

			// signals
			virtual void subscribeToEnterCollision( const EnterCollisionSignal::slot_type & slot );
			virtual void subscribeToLeaveCollision( const LeaveCollisionSignal::slot_type & slot );

			// helpers...
			void _collide(OdeComplexObject* pOther, dGeomID geomA, dGeomID geomB, dJointGroup * contactJointGroup);
			void _updateCollisionGeometry();
			void postStep( real timeElapsed );

		protected:
			EnterCollisionSignal		mEnterCollisionSignal;
			LeaveCollisionSignal		mLeaveCollisionSignal;

			struct CollisionInfo
			{
				real	time;
			};
			typedef base::templates::AssocVector< OdeComplexObject*, CollisionInfo > CollisionList;
			CollisionList				mCollisions;

			yake::base::templates::SignalConnection	mPostStepConn;

			OdeBody						* mBody;

			OdeWorld					* mWorld;

			typedef base::templates::Vector< ICollisionGeometry* > GeomList;
			GeomList					mGeoms;

			real						mSoftness;
			real						mFriction;
			real						mFriction2;

			bool						mLateralSlip;
			ISlipNormalSource*			mSlipNormalSource;
			Vector3						mSlipNormal;
			real						mSlipLinearCoeff;
			real						mSlipAngularCoeff;
		};

	}
}

#endif
