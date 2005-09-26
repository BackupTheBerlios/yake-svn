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
			setMass( 1 );
			setPosition( Vector3(0,0,0) );
			setOrientation( Quaternion::kIdentity );
		}

		//---------------------------------------------------
		OdeBody::~OdeBody()
		{
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
			if ( const IBody::SphereMassDesc* sphereMassDesc = dynamic_cast<const IBody::SphereMassDesc*>( &desc ) )
			{
				dMassSetSphere( mass, sphereMassDesc->density, sphereMassDesc->radius );
			}
			else if ( const IBody::BoxMassDesc* boxMassDesc = dynamic_cast<const IBody::BoxMassDesc*>( &desc ) )
			{
				dMassSetBox( mass, boxMassDesc->density, boxMassDesc->sizeX, boxMassDesc->sizeY, boxMassDesc->sizeZ );
			}
			else if ( const IBody::CapsuleMassDesc* capsuleMassDesc = dynamic_cast<const IBody::CapsuleMassDesc*>( &desc ) )
			{
				dMassSetCappedCylinder( mass,
												capsuleMassDesc->density,
												2, 							// along Y axis
												capsuleMassDesc->radius,
												capsuleMassDesc->length );
			}
			else if ( const IBody::CylinderMassDesc* cylMassDesc = dynamic_cast<const IBody::CylinderMassDesc*>( &desc ) )
			{
				dMassSetCylinder( mass,
										cylMassDesc->density,
										2,							// along Y axis
										cylMassDesc->radius,
										cylMassDesc->length );
			}
			else
			{
				YAKE_ASSERT( 0 ).error( "Unsupported/unknown mass description type!" );
			}
			
			dMassTranslate( mass, desc.offset.x, desc.offset.y, desc.offset.z );
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
			const Vector3 totalForce = force.force * force.duration;
			if (force.frameType == RF_GLOBAL)
				mOdeBody->addForce( totalForce.x, totalForce.y, totalForce.z );
			else
				mOdeBody->addRelForce( totalForce.x, totalForce.y, totalForce.z );
		}

		//---------------------------------------------------
		void OdeBody::addForce( Vector3 const& rForce )
		{
			mOdeBody->addForce( rForce.x, rForce.y, rForce.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addForceAtPos( Vector3 const& rForce, Vector3 const& rPos )
		{
			mOdeBody->addForceAtPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addForceAtLocalPos( Vector3 const& rForce, Vector3 const& rPos )
		{
			mOdeBody->addForceAtRelPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}

		//---------------------------------------------------
		void OdeBody::addLocalForce( Vector3 const& rForce )
		{
			mOdeBody->addRelForce( rForce.x, rForce.y, rForce.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addLocalForceAtLocalPos( Vector3 const& rForce, Vector3 const& rPos )
		{
			mOdeBody->addRelForceAtRelPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addLocalForceAtPos( Vector3 const& rForce, Vector3 const& rPos )
		{
			mOdeBody->addRelForceAtPos( rForce.x, rForce.y, rForce.z, rPos.x, rPos.y, rPos.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addTorque( Vector3 const& rTorque )
		{
			mOdeBody->addTorque( rTorque.x, rTorque.y, rTorque.z );
		}
		
		//---------------------------------------------------
		void OdeBody::addLocalTorque( Vector3 const& rTorque )
		{
			mOdeBody->addRelTorque( rTorque.x, rTorque.y, rTorque.z );
		}
		
		//---------------------------------------------------
		void OdeBody::setLinearVelocity( Vector3 const& rVelocity )
		{
			mOdeBody->setLinearVel( rVelocity.x, rVelocity.y, rVelocity.z );
		}
		
		//---------------------------------------------------
		Vector3 OdeBody::getLinearVelocity() const
		{
			const dReal* v = mOdeBody->getLinearVel();
			return Vector3( real(v[0]), real(v[1]), real(v[2]) );
		}
		
		//---------------------------------------------------
		void OdeBody::setAngularVelocity( Vector3 const& rVelocity)
		{
			mOdeBody->setAngularVel( rVelocity.x, rVelocity.y, rVelocity.z );
		}
		
		//---------------------------------------------------
		Vector3 OdeBody::getAngularVelocity() const
		{
			const dReal* v = mOdeBody->getAngularVel();
			return Vector3( real(v[0]), real(v[1]), real(v[2]) );
		}

		//---------------------------------------------------
		void OdeBody::setPosition(  Vector3 const& rPosition )
		{
			mOdeBody->setPosition( rPosition.x, rPosition.y, rPosition.z );
		}
		
		//---------------------------------------------------
		Vector3 OdeBody::getPosition() const
		{
			const dReal* pos = mOdeBody->getPosition();
			return Vector3( real(pos[0]), real(pos[1]), real(pos[2]) );
		}
		
		//---------------------------------------------------
		void OdeBody::setOrientation( Quaternion const& rOrientation )
		{
			dQuaternion q = { rOrientation.w, rOrientation.x, rOrientation.y, rOrientation.z };
			mOdeBody->setQuaternion( q );
		}
		
		//---------------------------------------------------
		Quaternion OdeBody::getOrientation() const
		{
			const dReal* q = mOdeBody->getQuaternion();
			return Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) );
		}
		
		
// 		//---------------------------------------------------
// 		void OdeBody::translateMass( const Vector3 & d )
// 		{
// 			dMassTranslate(&mMass, d.x, d.y, d.z);
// 		}
// 
// 		//-----------------------------------------------------
// 		IBody::MassType OdeBody::getType() const
// 		{
// 			return mMassType;
// 		}
// 
// 		//-----------------------------------------------------
// 		void OdeBody::setMassBox(real lx, real ly, real lz, real density)
// 		{
// 			mMassType = MT_BOX;
// 			dMassSetBox( &mMass, density, lx, ly, lz );
// 			YAKE_ASSERT( mOdeBody ).error("Need a body!");
// 			mOdeBody->setMass( &mMass );
// 		}
// 
// 		//-----------------------------------------------------
// 		void OdeBody::setMassSphere(real r, real density)
// 		{
// 			mMassType = MT_SPHERE;
// 			dMassSetSphere( &mMass, density, r );
// 			YAKE_ASSERT( mOdeBody ).error("Need a body!");
// 			mOdeBody->setMass( &mMass );
// 		}
// 
// 		//-----------------------------------------------------
// 		Vector3 OdeBody::getTorque() const
// 		{
// 			YAKE_ASSERT( mOdeBody ).error("Need a body!");
// 			const dReal * torque = mOdeBody->getTorque();
// 			return Vector3(torque[0], torque[1], torque[2]);
// 		}
// 
// 		//-----------------------------------------------------
// 		void OdeBody::setEnabled( bool enabled )
// 		{
// 			YAKE_ASSERT( mOdeBody ).error("Need a body!");
// 			if (enabled)
// 				mOdeBody->enable();
// 			else
// 				mOdeBody->disable();
// 		}
// 
// 		//-----------------------------------------------------
// 		OdeWorld* OdeBody::_getWorld() const
// 		{
// 			YAKE_ASSERT( mWorld ).error("Need a world!");
// 			return mWorld;
// 		}
// 
		//-----------------------------------------------------
		dBody* OdeBody::_getOdeBody() const
		{
			return mOdeBody;
		}

} // physics
} // yake
