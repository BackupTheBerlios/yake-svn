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

		class OdeBody : public IBody
		{
		public:
			OdeBody( OdeWorld* world );
			virtual ~OdeBody();

			virtual void setPosition( const Vector3 & position );
			virtual Vector3 getPosition() const;
			virtual void setOrientation( const Quaternion & orientation );
			virtual Quaternion getOrientation() const;

			virtual void setLinearVelocity(const Vector3 & velocity);
			virtual Vector3 getLinearVelocity() const;
			virtual void setAngularVelocity(const Vector3 & velocity);
			virtual Vector3 getAngularVelocity() const;
			
			virtual void setMassBox(real lx, real ly, real lz, real density = 1.);
			virtual void setMassSphere(real r, real density = 1.);
			virtual void setMass(real mass);
			virtual real getMass() const;
			virtual IBody::MassType getType() const;

			virtual void addForce( const Vector3 & force );
			virtual void addRelForce( const Vector3 & force );
			virtual void addTorque( const Vector3 & torque );
			virtual void addRelTorque( const Vector3 & torque );
			virtual void addForceAtPosition( const Vector3 & force, const Vector3 & position = Vector3(0,0,0) );
			virtual void addRelForceAtRelPosition( const Vector3 & force, const Vector3 & relPosition = Vector3(0,0,0) );

			virtual Vector3 getTorque() const;

			virtual void setEnabled( bool enabled );

			// helper functions
			dBody* _getOdeBody() const;
			OdeWorld* _getWorld() const;
			bool _isValid() const
			{ return mValid; }
		protected:
			OdeWorld		* mWorld;
			dBody			* mOdeBody;
			dMass			mMass;
			MassType		mMassType;
			bool			mValid;
		};

	}
}

#endif