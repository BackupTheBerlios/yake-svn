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
#ifndef YAKE_ODEWORLD_H
#define YAKE_ODEWORLD_H

namespace yake {

	using namespace base::math;

	namespace physics {

		class OdeBody;
		class OdeWorld : public IWorld
		{
		public:
			OdeWorld();
			virtual ~OdeWorld();

			virtual void update( const real timeElapsed );
			virtual void setGlobalGravity( const Vector3 & acceleration );

			virtual IBody* createBody();
			virtual IJoint* createJoint(IJoint::JointType type, IJointGroup* group = 0);

			// dynamics objects
			virtual IComplexObject* createPlane(real a, real b, real c, real d);
			virtual IComplexObject* createSphere(real radius);
			virtual IComplexObject* createBox(real lx, real ly, real lz);
			virtual IComplexObject* createMesh(const base::String & mesh);
			virtual IComplexObject* createRay(const Vector3 & origin, const Quaternion & orientation);
			virtual IComplexObject* createEmptyPhysicsObject();

			// collision geometry
			virtual ICollisionGeometry* createCollisionGeomSphere(real radius);
			virtual ICollisionGeometry* createCollisionGeomBox(real lx, real ly, real lz);
			virtual ICollisionGeometry* createCollisionGeomMesh( const base::String & collisionMeshResourceName );

			// helpers
			dWorldID _getOdeID() const;
			dJointGroup* _getOdeContactJointGroup() const 
			{ return mOdeContactGroup; }
			dSpace* _getOdeSpace() const
			{ return mOdeSpace; }

			static void _OdeNearCallback (void *data, dGeomID o1, dGeomID o2);

			void _addBody( OdeBody* body );
			void _removeBody( OdeBody* body );

			real _getStepSize() const
			{ return mStepSize; }
		protected:
			real				mStepSize;
			dWorld			* mOdeWorld;
			dJointGroup		* mOdeContactGroup;
			dSpace			* mOdeSpace;

			typedef std::vector< OdeBody* > BodyList;
			BodyList			mBodies;
		};

	}
}

#endif