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
#ifndef YAKE_PHYSICS_BODYNX_H
#define YAKE_PHYSICS_BODYNX_H

namespace yake {
namespace physics {

	class BodyNx : public IBody
	{
	public:
		BodyNx( NxActor* pActor, ActorNx* pActorNx );
		virtual ~BodyNx();

		virtual IDynamicActor& getActor() const;

		virtual void setMass(const real mass);
		virtual real getMass() const;
		virtual void setMass( const MassDesc& rDesc )
		{ YAKE_ASSERT( 0 && "BodyNx::setMass() NOT IMPLEMENTED" ).warning("NOT IMPLEMENTED!"); }
		virtual void addMass( const MassDesc& rDesc )
		{ YAKE_ASSERT( 0 && "BodyNx::addMass() NOT IMPLEMENTED" ).warning("NOT IMPLEMENTED!"); }

		virtual void setLinearVelocity(const Vector3 & rkVelocity);
		virtual Vector3 getLinearVelocity() const;
		virtual void setAngularVelocity(const Vector3 & rkVelocity);
		virtual Vector3 getAngularVelocity() const;

		virtual void addForce( const Vector3 & rkForce );
		virtual void addForceAtPos( const Vector3 & rkForce, const Vector3 & rkPos );
		virtual void addForceAtLocalPos( const Vector3& rForce, const Vector3& rPos );
		virtual void addLocalForce( const Vector3 & rkForce );
		virtual void addLocalForceAtLocalPos( const Vector3 & rkForce, const Vector3 & rkPos );
		virtual void addLocalForceAtPos( const Vector3 & rkForce, const Vector3 & rkPos );
		virtual void addTorque( const Vector3 & rkTorque );
		virtual void addLocalTorque( const Vector3 & rkTorque );

		// helpers
	private:
		ActorNx*	mActor;
		NxActor*	mNxActor;
		NxBodyDesc	mNxDesc;
	};

}
}

#endif