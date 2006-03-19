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
#ifndef YAKE_ODEBODY_H
#define YAKE_ODEBODY_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>

namespace yake {
namespace physics {

		class OdeActor;
		
		class YAKE_PHYSICSODE_API OdeBody : public IBody
		{
		public:
			OdeBody( OdeWorld* pWorld, OdeActor& rOwner );
			virtual ~OdeBody();

			YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, bool, OnSleeping )

			virtual IActor& getActor() const;

			virtual void setMass( const real mass );
			virtual real getMass() const;
			virtual void setMass( const MassDesc& rDesc );
			virtual void addMass( const MassDesc& rDesc );

			virtual void setLinearVelocity( math::Vector3 const& rVelocity );
			virtual math::Vector3 getLinearVelocity() const;
			virtual void setAngularVelocity( math::Vector3 const& rVelocity);
			virtual math::Vector3 getAngularVelocity() const;
			
			virtual void addForce( const Force& force );
			virtual void addForce( math::Vector3 const& rForce );
			virtual void addForceAtPos( math::Vector3 const& rForce, math::Vector3 const& rPos );
			virtual void addForceAtLocalPos( const math::Vector3& rForce, const math::Vector3& rPos );
			virtual void addLocalForce( math::Vector3 const& rForce );
			virtual void addLocalForceAtLocalPos( math::Vector3 const& rForce, math::Vector3 const& rPos );
			virtual void addLocalForceAtPos( math::Vector3 const& rForce, math::Vector3 const& rPos );
			virtual void addTorque( math::Vector3 const& rTorque );
			virtual void addLocalTorque( math::Vector3 const& rTorque );
			
			// helpers for OdeActor
			virtual void setPosition( const math::Vector3 & position );
			virtual void setOrientation( const math::Quaternion & orientation );
			virtual math::Vector3 getPosition() const;
			virtual math::Quaternion getOrientation() const;
			virtual math::Vector3 getDerivedPosition() const { return this->getPosition(); }
			virtual math::Quaternion getDerivedOrientation() const { return this->getOrientation(); }

			
// 			virtual void setMassBox(real lx, real ly, real lz, real density = 1.);
// 			virtual void setMassSphere(real r, real density = 1.);
// 			virtual void setMass(real mass);
// 			/// void dMassAdd (dMass *a, const dMass *b);
// 			virtual real getMass() const;
// //			virtual MassType getType() const;
// 			virtual void translateMass( const math::Vector3 & d );
// 
// 			virtual math::Vector3 getTorque() const;
// 
// 			virtual void setEnabled( bool enabled );

			// helper functions
			dBody* _getOdeBody() const;
			//OdeWorld* _getWorld() const;
			//bool _isValid() const { return mValid; }*/

			void _applyMassDescFromShapeDesc( const IShape::Desc& rShapeDesc, real massOrDensity, IBody::quantityType qType );
		protected:
			OdeWorld* 				mOdeWorld;
			OdeActor&				mOwner;
			dBody* 					mOdeBody;
			dMass					mMass;
//			MassType		mMassType;
//			bool			mValid;
		};

} // physics
} // yake

#endif

