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
#ifndef YAKE_ODEJOINT_H
#define YAKE_ODEJOINT_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>

namespace yake {
namespace physics {

		class OdeWorld;
		
		class YAKE_PHYSICSODE_API OdeJoint : public IJoint
		{
		protected:
			//OdeJoint();
			//OdeJoint( OdeJoint const& );
			OdeJoint( OdeWorld* pWorld, JointType type );
		
		public:
			virtual ~OdeJoint();

			virtual IWorld* getCreator() const
			{ return mWorld; }

			/** Attach joint to bodies. If one of the bodies is 0, then attach
			 * to static environment.
			 */
			virtual void attach( IBody* pBody1, IBody* pBody2 );

			//virtual JointType getType() const;
			
			//virtual size_t getNumAxis() const;
			//virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			//virtual size_t getNumAnchors() const;
			//virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			//virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			//virtual void setLimits( size_t axisIndex, real low, real high );
			
			virtual void setBreakable( bool enabled );
			virtual void setBreakableForce( real force );
			virtual void setBreakableTorque( real torque );
			virtual void setConnectedBodiesCollide( bool enabled );
		
			virtual void setSpring(real);
			virtual real getSpring() const;
			virtual void setDamping(real);
			virtual real getDamping() const;

			YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, OnBreaking )
			YAKE_MEMBERSIGNAL_FIRE_FN0( public, OnBreaking )

			dJoint* _getOdeJoint() const
			{ return mOdeJoint; }
		protected:
			void _applySpring();
			real _getCFMFromSpring() const;
			real _getERPFromSpring() const;
			
		protected:
			OdeWorld* 		mWorld;
			dJoint*			mOdeJoint;
			
			real			mSpringConstant;
			real			mDampingConstant;
			JointType		mType;
		};

		class YAKE_PHYSICSODE_API OdeHingeJoint : public OdeJoint
		{
		public:
			OdeHingeJoint( OdeWorld* pWorld );
			
			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			virtual void setMotorEnabled( size_t axisIndex, bool enabled );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		private:
			real	mVelTarget0;
			real	mMaxForce0;
		};

		class YAKE_PHYSICSODE_API OdeHinge2Joint : public OdeJoint
		{
		public:
			OdeHinge2Joint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			virtual void setMotorEnabled( size_t axisIndex, bool enabled );
			
			virtual void setLimits( size_t axisIndex, real low, real high );

		private:
			real	mVelTarget[2];
			real	mMaxForce[2];
		};

		class YAKE_PHYSICSODE_API OdeBallJoint : public OdeJoint
		{
		public:
			OdeBallJoint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			virtual void setMotorEnabled( size_t axisIndex, bool enabled );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};

		class YAKE_PHYSICSODE_API OdeFixedJoint : public OdeJoint
		{
		public:
			OdeFixedJoint( OdeWorld* pWorld );

			virtual void attach( IBody* pBody1, IBody* pBody2 );
			
			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			virtual void setMotorEnabled( size_t axisIndex, bool enabled );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};

		class YAKE_PHYSICSODE_API OdeSliderJoint : public OdeJoint
		{
		public:
			OdeSliderJoint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			virtual void setMotorEnabled( size_t axisIndex, bool enabled );
			
			virtual void setLimits( size_t axisIndex, real low, real high );

		private:
			real	mVelTarget0;
			real	mMaxForce0;
		};
		
		class YAKE_PHYSICSODE_API OdeUniversalJoint : public OdeJoint
		{
		public:
			OdeUniversalJoint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, math::Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, math::Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			virtual void setMotorEnabled( size_t axisIndex, bool enabled );
			
			virtual void setLimits( size_t axisIndex, real low, real high );

		private:
			real	mVelTarget[2];
			real	mMaxForce[2];
		};
} // physics
} // yake

#endif

