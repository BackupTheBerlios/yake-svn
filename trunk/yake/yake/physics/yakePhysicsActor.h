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
#ifndef YAKE_PHYSICS_ACTOR_H
#define YAKE_PHYSICS_ACTOR_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

namespace yake {
	using namespace base;
	using namespace math;
namespace physics {

	class IActorListener
	{
	public:
		virtual ~IActorListener() {}

		virtual void onCollision(IActor* pThis, IActor* pOther);
		virtual void onSleeping(const bool sleeping);
		virtual void onAddShape(IShape* pShape);
	};

	struct ActorCollisionInfo
	{
		IActor*	pThis;
		IActor*	pOther;
		ActorCollisionInfo( IActor* pThisActor = 0, IActor* pOtherActor = 0) :
			pThis( pThisActor ),
			pOther( pOtherActor )
		{}
	};

	enum ActorType {
		ACTOR_STATIC,
		ACTOR_MOVABLE,
		ACTOR_DYNAMIC
	};
	class IWorld;
	class IActor : public Movable, public ListenerManager<IActorListener>
	{
	public:
		struct Desc
		{
			explicit Desc(const ActorType t) : type(t) {}
			explicit Desc(const Desc& desc) {}
			virtual ~Desc() {}
			Deque< SharedPtr<IShape::Desc> >		shapes;
			ActorType								type;
		};
	public:
		virtual ~IActor() {}

		virtual IWorld* getCreator() const = 0;

		virtual IShape* createShape( const IShape::Desc& rShapeDesc ) = 0;
		virtual void destroyShape( IShape* pShape ) = 0;
		virtual IShapePtrList getShapes() const = 0;

		virtual IBody* getBodyPtr() const = 0;
		virtual IBody& getBody() const = 0;

		//virtual void setPosition(const Vector3& position) = 0;
		//virtual Vector3 getPosition() const = 0;

		//virtual void setOrientation( const Quaternion & rkOrientation ) = 0;
		//virtual Quaternion getOrientation() const = 0;

		virtual void setEnabled(const bool enabled) = 0;
		virtual bool isEnabled() const = 0;

		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(ActorCollisionInfo&), CollisionEntered )
		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(ActorCollisionInfo&), CollisionExited )
	};
	YAKE_PHYSICS_COMMON_POINTERS( IActor );

}
}
#endif
