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
#include <yake/plugins/physicsODE/OdeBody.h>

namespace yake {
	namespace physics {

		//---------------------------------------------------
		OdeBody::OdeBody( OdeWorld* world ) : mWorld( world), mOdeBody( 0 ), mValid( false )
		{
			YAKE_ASSERT( mWorld ).debug("No body without a world, pal!");

			mOdeBody = new dBody( mWorld->_getOdeID() );
			YAKE_ASSERT( mOdeBody ).error("Out of memory ?");
			if (!mOdeBody)
				return;
			mValid = true;
			mOdeBody->enable();
			mOdeBody->setTorque( 0, 0, 0 );
			mOdeBody->setForce( 0, 0, 0 );
			mOdeBody->setLinearVel( 0, 0, 0 );
			mOdeBody->setAngularVel( 0, 0, 0 );
			setMassSphere( 1, 1 ); // default
			setMass( 1 );
		}

		//---------------------------------------------------
		void OdeBody::translateMass( const Vector3 & d )
		{
			dMassTranslate(&mMass, d.x, d.y, d.z);
		}

		//---------------------------------------------------
		OdeBody::~OdeBody()
		{
			mValid = false;
			YAKE_SAFE_DELETE( mOdeBody );
		}

		//-----------------------------------------------------
		void OdeBody::setPosition(const Vector3 & position)
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			mOdeBody->setPosition( position.x, position.y, position.z );
		}

		//-----------------------------------------------------
		Vector3 OdeBody::getPosition() const
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			const dReal* pos = mOdeBody->getPosition();
			return Vector3(pos[0],pos[1],pos[2]);
		}

		//-----------------------------------------------------
		void OdeBody::setOrientation(const Quaternion & orientation)
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			dQuaternion q = { orientation.w, orientation.x, orientation.y, orientation.z };
			mOdeBody->setQuaternion( q );
		}

		//-----------------------------------------------------
		Quaternion OdeBody::getOrientation() const
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			const dReal* q = mOdeBody->getQuaternion();
			return Quaternion(q[0],q[1],q[2],q[3]);
		}

		//-----------------------------------------------------
		void OdeBody::setLinearVelocity(const Vector3 & velocity)
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			mOdeBody->setLinearVel( velocity.x, velocity.y, velocity.z );
		}

		//-----------------------------------------------------
		Vector3 OdeBody::getLinearVelocity() const
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			const dReal* v = mOdeBody->getLinearVel();
			return Vector3( v[0], v[1], v[2] );
		}

		//-----------------------------------------------------
		void OdeBody::setAngularVelocity(const Vector3 & velocity)
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			mOdeBody->setAngularVel( velocity.x, velocity.y, velocity.z );
		}

		//-----------------------------------------------------
		Vector3 OdeBody::getAngularVelocity() const
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			const dReal* v = mOdeBody->getAngularVel();
			return Vector3( v[0], v[1], v[2] );
		}

		//-----------------------------------------------------
		IBody::MassType OdeBody::getType() const
		{
			return mMassType;
		}

		//-----------------------------------------------------
		void OdeBody::setMassBox(real lx, real ly, real lz, real density)
		{
			mMassType = MT_BOX;
			dMassSetBox( &mMass, density, lx, ly, lz );
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->setMass( &mMass );
		}

		//-----------------------------------------------------
		void OdeBody::setMassSphere(real r, real density)
		{
			mMassType = MT_SPHERE;
			dMassSetSphere( &mMass, density, r );
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->setMass( &mMass );
		}

		//-----------------------------------------------------
		void OdeBody::setMass(real mass)
		{
			dMassAdjust( &mMass, mass );
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->setMass( &mMass );
		}

		//-----------------------------------------------------
		real OdeBody::getMass() const
		{
			return mMass.mass;
		}

		//-----------------------------------------------------
		void OdeBody::addForce( const Vector3 & force )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->addForce( force.x, force.y, force.z );
		}

		//-----------------------------------------------------
		void OdeBody::addTorque( const Vector3 & torque )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->addTorque( torque.x, torque.y, torque.z );
		}

		//-----------------------------------------------------
		void OdeBody::addRelForce( const Vector3 & force )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->addRelForce( force.x, force.y, force.z );
		}

		//-----------------------------------------------------
		void OdeBody::addRelTorque( const Vector3 & torque )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->addRelTorque( torque.x, torque.y, torque.z );
		}

		//-----------------------------------------------------
		void OdeBody::addForceAtPosition( const Vector3 & force, const Vector3 & position )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->addForceAtPos( force.x, force.y, force.z, position.x, position.y, position.z );
		}

		//-----------------------------------------------------
		void OdeBody::addRelForceAtRelPosition( const Vector3 & force, const Vector3 & position )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			mOdeBody->addRelForceAtRelPos( force.x, force.y, force.z, position.x, position.y, position.z );
		}

		//-----------------------------------------------------
		Vector3 OdeBody::getTorque() const
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			const dReal * torque = mOdeBody->getTorque();
			return Vector3(torque[0], torque[1], torque[2]);
		}

		//-----------------------------------------------------
		void OdeBody::setEnabled( bool enabled )
		{
			YAKE_ASSERT( mOdeBody ).error("Need a body!");
			if (enabled)
				mOdeBody->enable();
			else
				mOdeBody->disable();
		}

		//-----------------------------------------------------
		OdeWorld* OdeBody::_getWorld() const
		{
			YAKE_ASSERT( mWorld ).error("Need a world!");
			return mWorld;
		}

		//-----------------------------------------------------
		dBody* OdeBody::_getOdeBody() const
		{
			YAKE_ASSERT( mOdeBody ).debug("Need a body!");
			return mOdeBody;
		}

	}
}