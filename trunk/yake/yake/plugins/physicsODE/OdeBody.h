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
#ifndef YAKE_ODEBODY_H
#define YAKE_ODEBODY_H

namespace yake {
namespace physics {

		class OdeDynamicActor;
		
		class OdeBody : public IBody
		{
		public:
			OdeBody( OdeWorld* pWorld, OdeDynamicActor& rOwner );
			virtual ~OdeBody();

			virtual IDynamicActor& getActor() const;

			virtual void setMass( const real mass );
			virtual real getMass() const;
			virtual void setMass( const MassDesc& rDesc )
			{ YAKE_ASSERT(0); }
			virtual void addMass( const MassDesc& rDesc )
			{ YAKE_ASSERT(0); }

			virtual void setLinearVelocity( Vector3 const& rVelocity );
			virtual Vector3 getLinearVelocity() const;
			virtual void setAngularVelocity( Vector3 const& rVelocity);
			virtual Vector3 getAngularVelocity() const;
			
			virtual void addForce( Vector3 const& rForce );
			virtual void addForceAtPos( Vector3 const& rForce, Vector3 const& rPos );
			virtual void addLocalForce( Vector3 const& rForce );
			virtual void addLocalForceAtLocalPos( Vector3 const& rForce, Vector3 const& rPos );
			virtual void addLocalForceAtPos( Vector3 const& rForce, Vector3 const& rPos );
			virtual void addTorque( Vector3 const& rTorque );
			virtual void addLocalTorque( Vector3 const& rTorque );
			
			// helpers for OdeActor
			virtual void setPosition( const Vector3 & position );
			virtual Vector3 getPosition() const;
			virtual void setOrientation( const Quaternion & orientation );
			virtual Quaternion getOrientation() const;

			
// 			virtual void setMassBox(real lx, real ly, real lz, real density = 1.);
// 			virtual void setMassSphere(real r, real density = 1.);
// 			virtual void setMass(real mass);
// 			/// void dMassAdd (dMass *a, const dMass *b);
// 			virtual real getMass() const;
// //			virtual MassType getType() const;
// 			virtual void translateMass( const Vector3 & d );
// 
// 			virtual Vector3 getTorque() const;
// 
// 			virtual void setEnabled( bool enabled );

			// helper functions
			dBody* _getOdeBody() const;
			//OdeWorld* _getWorld() const;
			//bool _isValid() const { return mValid; }*/
		protected:
			OdeWorld* 				mOdeWorld;
			OdeDynamicActor&		mOwner;
			dBody* 					mOdeBody;
			dMass					mMass;
//			MassType		mMassType;
//			bool			mValid;
		};

} // physics
} // yake

#endif
