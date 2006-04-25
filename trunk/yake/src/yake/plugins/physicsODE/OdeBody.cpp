/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeActor.h>

namespace yake {
namespace physics {

		//---------------------------------------------------
		OdeBody::OdeBody( OdeWorld* pOdeWorld, OdeActor& rOwner ) : 
								mOdeWorld( pOdeWorld ),
								mOdeBody( 0 ),
//								mValid( false ),
								mOwner( rOwner )
		{
			YAKE_ASSERT( mOdeWorld ).debug( "No body without a world, pal!" );

			mOdeBody = new dBody( mOdeWorld->_getOdeID() );
			YAKE_ASSERT( mOdeBody ).error( "Failed to create ODE body!" );
			
			mOdeBody->setAutoDisableFlag( 0 );
//			mValid = true;
			mOdeBody->enable();
			mOdeBody->setTorque( 0, 0, 0 );
			mOdeBody->setForce( 0, 0, 0 );
			mOdeBody->setLinearVel( 0, 0, 0 );
			mOdeBody->setAngularVel( 0, 0, 0 );

			dMassSetSphere( &mMass, 1, 1 );
			mOdeBody->setMass( &mMass );

			setPosition( math::Vector3(0,0,0) );
			setOrientation( math::Quaternion::kIdentity );

			mOdeWorld->_addBody( this );
		}

		//---------------------------------------------------
		OdeBody::~OdeBody()
		{
			mOdeWorld->_removeBody( this );
			YAKE_SAFE_DELETE( mOdeBody );
		}
		
		//---------------------------------------------------
		IActor& OdeBody::getActor() const
		{
			return mOwner;
		}

		//---------------------------------------------------
 		void OdeBody::setMass( real mass )
		{
 			dMassAdjust( &mMass, mass );
			mOdeBody->setMass( &mMass );
		}
		
		//---------------------------------------------------
		real OdeBody::getMass() const
		{
 			return real(mMass.mass);
		}
		
		//---------------------------------------------------
		/**
		 *     Parse mass description struct and fill ODE mass struct.
		 * @param desc Mass description.
		 * @param mass Mass struct to fill.
		 */
		void parseMassDesc( const IBody::MassDesc& desc, dMass* mass )
		{
			if ( const IBody::SphereMassDesc* sphereMassDesc =
				dynamic_cast<const IBody::SphereMassDesc*>( &desc ) )
			{
				if ( sphereMassDesc->qType == IBody::QT_DENSITY )
					dMassSetSphere( mass, sphereMassDesc->quantity, sphereMassDesc->radius );
				else
					dMassSetSphereTotal( mass, sphereMassDesc->quantity, sphereMassDesc->radius );
			}
			else if ( const IBody::BoxMassDesc* boxMassDesc = dynamic_cast<const IBody::BoxMassDesc*>( &desc ) )
			{
				if ( boxMassDesc->qType == IBody::QT_DENSITY )
					dMassSetBox( mass, boxMassDesc->quantity, 
						boxMassDesc->sizeX, boxMassDesc->sizeY, boxMassDesc->sizeZ );
				else
					dMassSetBoxTotal( mass, boxMassDesc->quantity, 
						boxMassDesc->sizeX, boxMassDesc->sizeY, boxMassDesc->sizeZ );
			}
			else if ( const IBody::CapsuleMassDesc* capsuleMassDesc = 
				dynamic_cast<const IBody::CapsuleMassDesc*>( &desc ) )
			{
				if ( capsuleMassDesc->qType == IBody::QT_DENSITY )
					dMassSetCappedCylinder( mass,
						capsuleMassDesc->quantity,
						2, 							// along Y axis
						capsuleMassDesc->radius,
						capsuleMassDesc->length );
				else
				    dMassSetCappedCylinderTotal( mass,
					    capsuleMassDesc->quantity,
					    2, 							// along Y axis
					    capsuleMassDesc->radius,
					    capsuleMassDesc->length );
			}
			else if ( const IBody::CylinderMassDesc* cylMassDesc = 
				dynamic_cast<const IBody::CylinderMassDesc*>( &desc ) )
			{
				if ( cylMassDesc->qType == IBody::QT_DENSITY )
					dMassSetCylinder( mass,
						cylMassDesc->quantity,
						2,							// along Y axis
						cylMassDesc->radius,
						cylMassDesc->length );
				else
					dMassSetCylinderTotal( mass,
						cylMassDesc->quantity,
						2,							// along Y axis
						cylMassDesc->radius,
						cylMassDesc->length );
			}
			else if ( const IBody::MassDesc* massDesc = 
				dynamic_cast<const IBody::MassDesc*>( &desc ) )
			{
			    // creating unit sphere
			    dMassSetSphereTotal( mass, massDesc->quantity, 1 );
			}
			else
			{
				YAKE_ASSERT( 0 ).error( "Unsupported/unknown mass description type!" );
			}
			
			dMassTranslate( mass, desc.offset.x, desc.offset.y, desc.offset.z );
		}
		
		//-----------------------------------------------------
		void OdeBody::_applyMassDescFromShapeDesc( IShape::Desc const& rShapeDesc, real massOrDensity, IBody::quantityType qType )
		{
			const IShape::Desc* pShapeDesc = &( rShapeDesc );
			
			if ( const IShape::SphereDesc* pSphereDesc = dynamic_cast<const IShape::SphereDesc*>( pShapeDesc ) )
			{
				this->addMass( IBody::SphereMassDesc( pSphereDesc->radius, massOrDensity, pSphereDesc->position, qType ) );
			}
			else if ( const IShape::BoxDesc* pBoxDesc = dynamic_cast<const IShape::BoxDesc*>( pShapeDesc ) )
			{
				this->addMass( IBody::BoxMassDesc( pBoxDesc->dimensions.x, pBoxDesc->dimensions.y, pBoxDesc->dimensions.z, massOrDensity,
					pBoxDesc->position, qType ) );
			}
			else if ( const IShape::CapsuleDesc* pCapsuleDesc = dynamic_cast<const IShape::CapsuleDesc*>( pShapeDesc ) )
			{
				this->addMass( IBody::CapsuleMassDesc( pCapsuleDesc->radius, pCapsuleDesc->height, massOrDensity, pCapsuleDesc->position, qType ) );
			}
			else
			{
				YAKE_ASSERT( 1 == 0 ).warning( "Unsupported shape type!" );
			}
		}

		//---------------------------------------------------
		void OdeBody::setMass( const MassDesc& rDesc )
		{
			parseMassDesc( rDesc, &mMass );
			mOdeBody->setMass( &mMass );
		}
		
		//---------------------------------------------------
		void OdeBody::addMass( const MassDesc& rDesc )
		{
			dMass massToAdd;
			
			parseMassDesc( rDesc, &massToAdd );
			
			dMassAdd( &mMass, &massToAdd );
			
			mOdeBody->setMass( &mMass );
		}

		
		//---------------------------------------------------
		void OdeBody::addForce( const Force& force )
		{
			//@todo apply force over several time steps according to duration.
			const math::Vector3 totalForce = force.force * force.duration;
			if (force.frameType == RF_GLOBAL)
				mOdeBody->addForce( totalForce.x, totalForce.y, totalForce.z );
			else
				mOdeBody->addRelForce( totalForce.x, totalForce.y, totalForce.z );
		}

		//---------------------------------------------------
		void OdeBody::addForce( math::Vector3 const& rForce )
		{
			mOdeBody->addForce( rForce.x, rForce.y, rForce.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addForceAtPos( math::Vector3 const& rForce, math::Vector3 const& rPos )
		{
			mOdeBody->addForceAtPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addForceAtLocalPos( math::Vector3 const& rForce, math::Vector3 const& rPos )
		{
			mOdeBody->addForceAtRelPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}

		//---------------------------------------------------
		void OdeBody::addLocalForce( math::Vector3 const& rForce )
		{
			mOdeBody->addRelForce( rForce.x, rForce.y, rForce.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addLocalForceAtLocalPos( math::Vector3 const& rForce, math::Vector3 const& rPos )
		{
			mOdeBody->addRelForceAtRelPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addLocalForceAtPos( math::Vector3 const& rForce, math::Vector3 const& rPos )
		{
			mOdeBody->addRelForceAtPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addTorque( math::Vector3 const& rTorque )
		{
			mOdeBody->addTorque( rTorque.x, rTorque.y, rTorque.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addLocalTorque( math::Vector3 const& rTorque )
		{
			mOdeBody->addRelTorque( rTorque.x, rTorque.y, rTorque.z );
		}
		
		//---------------------------------------------------
		void OdeBody::setLinearVelocity( math::Vector3 const& rVelocity )
		{
			mOdeBody->setLinearVel( rVelocity.x, rVelocity.y, rVelocity.z );
		}
		
		//---------------------------------------------------
		math::Vector3 OdeBody::getLinearVelocity() const
		{
			const dReal* v = mOdeBody->getLinearVel();
			return math::Vector3( static_cast<real>(v[0]), static_cast<real>(v[1]), static_cast<real>(v[2]) );
		}
		
		//---------------------------------------------------
		void OdeBody::setAngularVelocity( math::Vector3 const& rVelocity)
		{
			mOdeBody->setAngularVel( rVelocity.x, rVelocity.y, rVelocity.z );
		}
		
		//---------------------------------------------------
		math::Vector3 OdeBody::getAngularVelocity() const
		{
			const dReal* v = mOdeBody->getAngularVel();
			return math::Vector3( static_cast<real>(v[0]), static_cast<real>(v[1]), static_cast<real>(v[2]) );
		}

		//---------------------------------------------------
		void OdeBody::setPosition( const math::Vector3& rPosition )
		{
			mOdeBody->setPosition( rPosition.x, rPosition.y, rPosition.z );
		}
		
		//---------------------------------------------------
		math::Vector3 OdeBody::getPosition() const
		{
			const dReal* pos = mOdeBody->getPosition();

			return math::Vector3( static_cast<real>(pos[0]), 
				static_cast<real>(pos[1]), 
				static_cast<real>(pos[2]) );
		}
		
		//---------------------------------------------------
		void OdeBody::setOrientation( const math::Quaternion& rOrientation )
		{
			dQuaternion q = { rOrientation.w, rOrientation.x, rOrientation.y, rOrientation.z };
			mOdeBody->setQuaternion( q );
		}
		
		//---------------------------------------------------
		math::Quaternion OdeBody::getOrientation() const
		{
			const dReal* q = mOdeBody->getQuaternion();

			return math::Quaternion( static_cast<real>(q[0]), 
				static_cast<real>(q[1]), 
				static_cast<real>(q[2]), 
				static_cast<real>(q[3]) );
		}
		
		//-----------------------------------------------------
		dBody* OdeBody::_getOdeBody() const
		{
			return mOdeBody;
		}

} // physics
} // yake

