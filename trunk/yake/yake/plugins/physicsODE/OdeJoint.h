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
#ifndef YAKE_ODEJOINT_H
#define YAKE_ODEJOINT_H

namespace yake {
namespace physics {

		class OdeWorld;
		
		class OdeJoint : public IJoint
		{
		protected:
			OdeJoint();
			OdeJoint( OdeJoint const& );
			OdeJoint( OdeWorld* pWorld, JointType type );
		
		public:
			virtual ~OdeJoint();

			virtual void attach( IBody& rBody1, IBody& rBody2 );

			//virtual JointType getType() const;
			
			//virtual size_t getNumAxis() const;
			//virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			//virtual size_t getNumAnchors() const;
			//virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			//virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
			
			virtual void setBreakable( bool enabled );
			virtual void setBreakableForce( real force );
			virtual void setBreakableTorque( real torque );
			virtual void setConnectedBodiesCollide( bool enabled );
		
		protected:
			void _applySpring();
			real _getCFMFromSpring() const;
			real _getERPFromSpring() const;
			
		protected:
			OdeWorld* 		mWorld;
			dJoint*			mOdeJoint;
			
			real				mSpringConstant;
			real				mDampingConstant;
			JointType		mType;
		};

		class OdeHingeJoint : public OdeJoint
		{
		public:
			OdeHingeJoint( OdeWorld* pWorld );
			
			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};

		class OdeHinge2Joint : public OdeJoint
		{
		public:
			OdeHinge2Joint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};

		class OdeBallJoint : public OdeJoint
		{
		public:
			OdeBallJoint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};

		class OdeFixedJoint : public OdeJoint
		{
		public:
			OdeFixedJoint( OdeWorld* pWorld );

			virtual void attach( IBody& rBody1, IBody& rBody2 );
			
			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};

		class OdeSliderJoint : public OdeJoint
		{
		public:
			OdeSliderJoint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};
		
		class OdeUniversalJoint : public OdeJoint
		{
		public:
			OdeUniversalJoint( OdeWorld* pWorld );

			virtual JointType getType() const;
			
			virtual size_t getNumAxis() const;
			virtual void setAxis( size_t axisIndex, Vector3 const& rAxis );
			
			virtual size_t getNumAnchors() const;
			virtual void setAnchor( size_t anchorIndex, Vector3 const& rAnchor );
			
			virtual void setMotor( size_t axisIndex, real velocityTarget, real maximumForce );
			
			virtual void setLimits( size_t axisIndex, real low, real high );
		};
} // physics
} // yake

#endif
