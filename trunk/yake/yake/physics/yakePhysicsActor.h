/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#ifndef YAKE_PHYSICS_ACTOR_H
#define YAKE_PHYSICS_ACTOR_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace base::math;
namespace physics {

	class IActor
	{
	public:
		struct Desc
		{
			Desc() {}
			explicit Desc(const Desc& desc) {}
			virtual ~Desc() {}
			Deque< SharedPtr<IShape::Desc> >	shapes;
		};
	public:
		virtual ~IActor() {}

		typedef Vector<IShape*> ShapePtrVector;

		virtual IShape* createShape( const IShape::Desc& rShapeDesc ) = 0;
		virtual void destroyShape( IShape* pShape ) = 0;
		virtual const ShapePtrVector getShapes() const = 0;

		typedef Signal0<void> SignalCollision;
		virtual void subscribeToCollisionEnteredSignal( const SignalCollision::slot_type& rSlot ) = 0;
		virtual void subscribeToCollisionExitedSignal( const SignalCollision::slot_type& rSlot ) = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( IActor );

	class IStaticActor : public IActor
	{
	public:
		struct Desc : IActor::Desc
		{
			explicit Desc(const Desc& desc);
		};
	};
	YAKE_PHYSICS_COMMON_POINTERS( IStaticActor );

	class IMovableActor : public IStaticActor, public Movable
	{
	public:
		struct Desc : IActor::Desc
		{
			Desc() {}
			explicit Desc(const Desc& desc) {}
		};
	};
	YAKE_PHYSICS_COMMON_POINTERS( IMovableActor );

	class IDynamicActor : public IMovableActor
	{
	public:
		struct Desc : IActor::Desc
		{
			Desc() {}
			explicit Desc(const Desc& desc) {}
		};
		
		virtual IBody& getBody() const = 0;
		//virtual void updateMassFromShapes() = 0;
	};
	YAKE_PHYSICS_COMMON_POINTERS( IDynamicActor );
}
}
#endif