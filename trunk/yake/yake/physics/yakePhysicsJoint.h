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
#ifndef YAKE_PHYSICS_JOINT_H
#define YAKE_PHYSICS_JOINT_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	/**
		YAKE         ODE          NX
		Ball         Ball         Spherical
		Hinge        Hinge        Revolute
		Fixed        Fixed        [simulated] (using Prismatic ?]
		Hinge2       Hinge2       ? (has to be simulated?)
		Slider       Slider       Cylindrical(?)(or Slider?) (Prismatic="single translational DOF", Cylindrical="sliding joint, permits one translational and one rotational DOF")
		Universal    Universal    ? (has to be simulated?)

	*/
	enum JointType
	{
		JT_BALL,
		JT_HINGE,
		JT_HINGE2,
		JT_FIXED,
		JT_SLIDER,
		JT_UNIVERSAL,
		JT_ANGULAR_MOTOR,
		JT_OTHER
	};

	struct Limit
	{
		size_t axis;
		real low;
		real high;
		Limit(const size_t a = 0, const real l = real(-1), const real h = real(1)) :
			axis(a),
			low(l),
			high(h)
		{}
	};
	typedef Deque<Limit> LimitList;

	class IActor;
	class IAngularMotor;
	class YAKE_PHYSICS_API IJoint
	{
	public:
		/**
			@todo Make actor references weak ptrs!
		*/
		struct DescBase
		{
			DescBase(	const JointType type_, 
						IActor* pFirst,
						IActor* pSecond ) :
				type( type_ ),
				actor0( pFirst ),
				actor1( pSecond ),
				breakable( false ),
				breakingForce( real(0) ),
				breakingTorque( real(0) )
			{
			    // we want to allow to create joint descriptions when actors are not yet created,
			    // like on parsing stage...
			    //	YAKE_ASSERT( pFirst || pSecond ).warning("need at least one actor!");
			}

			virtual ~DescBase() {}

			JointType type; // not really needed...
			IActor* actor0;
			IActor* actor1;
			bool breakable;
			real breakingForce;
			real breakingTorque;
			LimitList limits;
		};

		struct DescFixed : DescBase
		{
			DescFixed(	IActor* pFirst,
						IActor* pSecond ) :
				DescBase( JT_FIXED,pFirst,pSecond )
			{}
		};

		struct DescHinge : DescBase
		{
			DescHinge(	IActor* pFirst,
						IActor* pSecond,
						const Vector3& rAxis,
						const Vector3& rAnchor ) :
				DescBase( JT_HINGE,pFirst,pSecond ),
				axis( rAxis ),
				anchor( rAnchor )
			{}
			Vector3		axis;
			Vector3		anchor;
		};

		struct DescHinge2 : DescBase
		{
			DescHinge2(	IActor* pFirst,
						IActor* pSecond,
						const Vector3& rAxis0, 
						const Vector3& rAxis1,
						const Vector3& rAnchor ) :
				DescBase( JT_HINGE2,pFirst,pSecond ),
				axis0( rAxis0 ),
				axis1( rAxis1 ),
				anchor( rAnchor )
			{}
			Vector3		axis0;
			Vector3		axis1;
			Vector3		anchor;
		};

		struct DescBall : DescBase
		{
			DescBall(	IActor* pFirst,
						IActor* pSecond,
						const Vector3& rAnchor ) :
				DescBase( JT_BALL, pFirst, pSecond ),
				anchor( rAnchor )
			{}
			Vector3		anchor;
		};
	
		struct DescSlider : DescBase
		{
			DescSlider(	IActor* pFirst,
						IActor* pSecond,
						const Vector3& rAxis ) :
				DescBase( JT_SLIDER, pFirst, pSecond ),
				axis( rAxis )
			{}
			Vector3		axis;
		};
	
		struct DescUniversal : DescBase
		{
			DescUniversal(	IActor* pFirst,
							IActor* pSecond,
							const Vector3& rAxis0, 
							const Vector3& rAxis1,
							const Vector3& rAnchor ) :
				DescBase( JT_UNIVERSAL, pFirst, pSecond ),
				axis0( rAxis0 ),
				axis1( rAxis1 ),
				anchor( rAnchor )
 			{}
 			Vector3		axis0;
 			Vector3		axis1;
			Vector3		anchor;
 		};
	public:
		virtual ~IJoint() {}

		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(void), OnBreaking )
	public:

		virtual JointType getType() const = 0;
		virtual size_t getNumAxis() const = 0;
		virtual void setAxis(size_t axisIndex, const Vector3& rAxis) = 0;
		virtual size_t getNumAnchors() const = 0;
		virtual void setAnchor(size_t anchorIndex, const Vector3& rAnchor) = 0;
		virtual void setMotor(size_t axisIndex, real velocityTarget, real maximumForce) = 0;
		virtual void setMotorEnabled(size_t axisIndex, bool enabled) = 0;
		virtual void setLimits(size_t axisIndex, real low, real high) = 0;
		virtual void setBreakable(bool enabled) = 0;
		virtual void setBreakableForce(real force) = 0;
		virtual void setBreakableTorque(real torque) = 0;
		virtual void setConnectedBodiesCollide(bool enabled) = 0;
		//virtual void addForce(const size_t axisIndex, const real force, const real duration = real(-1)) = 0;
		//virtual void addTorque(const size_t axisIndex, const real torque, const real duration = real(-1)) = 0;
		///setLow/HiStop
		//setSpring/Damper
		virtual void setSpring(real) = 0;
		virtual real getSpring() const = 0;
		virtual void setDamping(real) = 0;
		virtual real getDamping() const = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( IJoint );

}
}
#endif
