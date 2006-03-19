/*
   ------------------------------------------------------------------------------------
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
	using namespace ::yake::math;
namespace physics {

	class IActorListener
	{
	public:
		virtual ~IActorListener() {}

		virtual void onCollision(IActor* pThis, IActor* pOther);
		virtual void onSleeping(const bool sleeping);
		virtual void onAddShape(IShape* pShape);
	};

	/** Structure describes collision event between two actors.
	 * Respective collided shapes are supplied.
	 */
	struct ActorCollisionInfo
	{
		IActor*	pThis;
		IShape* pThisShape_;

		IActor*	pOther;
		IShape* pOtherShape_;

		ActorCollisionInfo( IActor* pThisActor = 0, IShape* pThisShape = 0, 
				    IActor* pOtherActor = 0, IShape* pOtherShape = 0 ) :
			pThis( pThisActor ), pThisShape_( pThisShape ),
			pOther( pOtherActor ), pOtherShape_( pOtherShape )
		{}
	};

	enum ActorType {
		ACTOR_STATIC,
		ACTOR_MOVABLE,
		ACTOR_DYNAMIC
	};
	class IWorld;
	class YAKE_PHYSICS_API IActor : public Movable, public ListenerManager<IActorListener>
	{
	public:
		struct Desc
		{
			explicit Desc(const ActorType t) : type(t) {}
			explicit Desc(const Desc& desc) { this->shapes = desc.shapes; this->type = desc.type; }
			virtual ~Desc() {}
			Deque< SharedPtr<IShape::Desc> >		shapes;
			ActorType								type;
		};
	public:
		virtual ~IActor() {}

		virtual IWorld* getCreator() const = 0;

		virtual IShape* createShape( const IShape::Desc& rShapeDesc, real massOrDensity = 0.0f, IBody::quantityType type = IBody::QT_MASS ) = 0;
		virtual void destroyShape( IShape* pShape ) = 0;
		virtual IShapePtrList getShapes() const = 0;

		virtual IBody* getBodyPtr() const = 0;
		virtual IBody& getBody() const = 0;

		// inherited from yake::Movable
	//	virtual void setPosition(const math::Vector3& position) = 0;
	//	virtual math::Vector3 getPosition() const = 0;

	//	virtual void setOrientation( const math::Quaternion & rkOrientation ) = 0;
	//	virtual math::Quaternion getOrientation() const = 0;

		virtual void setEnabled(const bool enabled) = 0;
		virtual bool isEnabled() const = 0;

		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(const ActorCollisionInfo&), CollisionEntered )
		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(const ActorCollisionInfo&), CollisionExited )
	};
	YAKE_PHYSICS_COMMON_POINTERS( IActor );

}
}
#endif

