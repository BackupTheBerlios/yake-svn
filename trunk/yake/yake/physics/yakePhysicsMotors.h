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
#ifndef YAKE_PHYSICS_MOTORS_H
#define YAKE_PHYSICS_MOTORS_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
namespace physics {

	class IMotor
	{
	public:
		struct Desc
		{
			Desc( bool enabled = true ) : enabled_(enabled)
			{}
			bool enabled_;
		};
	public:
		virtual ~IMotor() {}
	};

	class IAngularMotor : public IMotor
	{
	public:
		struct Desc : public IMotor::Desc
		{
			Desc( const real velTarget, const real maxForce, bool enabled = true ) :
				IMotor::Desc(enabled),
				velTarget_(velTarget),
				maxForce_(maxForce)
			{}
			bool	enabled_;
			real	velTarget_;
			real	maxForce_;
		};
	public:
		virtual ~IAngularMotor() {}
		virtual void setEnabled( bool enabled ) = 0;
		virtual void setAffectedBody( IBody& rBody ) = 0;
		virtual void setVelocityTarget( const real velTarget ) = 0;
		virtual void setMaximumForce( const real maxForce ) = 0;
		virtual void setAxis0( const Vector3& rAxis ) = 0;
		virtual void setAxis1( const Vector3& rAxis ) = 0;
		virtual void set( const real velTarget, const real maxForce, bool enabled = true ) = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( IAngularMotor );

	class ILinearMotor : public IMotor
	{
	public:
		virtual ~ILinearMotor() {}
		virtual void setEnabled( bool enabled ) = 0;
		virtual void setAffectedBody( IBody& rBody ) = 0;
		virtual void setVelocityTarget( const real velTarget ) = 0;
		virtual void setMaximumForce( const real maxForce ) = 0;
		virtual void setAxis( const Vector3& rAxis ) = 0;
		virtual void set( const real velTarget, const real maxForce, bool enabled = true ) = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( ILinearMotor );
}
}

#endif