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
#ifndef YAKE_PHYSICS_ACTORNX_H
#define YAKE_PHYSICS_ACTORNX_H

namespace yake {
namespace physics {

	class BodyNx;
	class ShapeNx;
	class WorldNx;
	class ActorNx : public IActor
	{
	private:
		ActorNx(const ActorNx&);
		ActorNx& operator=(const ActorNx&);
	public:
		ActorNx( NxScene & rScene, WorldNx& rWorld, bool bDynamic );
		virtual ~ActorNx();

		virtual IWorld* getCreator() const;

		virtual IShape* createShape( const IShape::Desc & rkShapeDesc, real massOrDensity = 0.0f, IBody::quantityType type = IBody::QT_MASS );
		virtual void destroyShape( IShape* pShape );
		virtual IShapePtrList getShapes() const;

		virtual void setEnabled(const bool enabled);
		virtual bool isEnabled() const;

		virtual IBody* getBodyPtr() const;
		virtual IBody& getBody() const;

		virtual void setPosition( const Vector3 & rkPosition );
		virtual Vector3 getPosition() const;

		virtual void setOrientation( const Quaternion & rkOrientation );
		virtual Quaternion getOrientation() const;

		YAKE_MEMBERSIGNAL_VIRTUALIMPL(public, void(ActorCollisionInfo&), CollisionEntered)
		YAKE_MEMBERSIGNAL_FIRE_FN1(public, CollisionEntered, ActorCollisionInfo& info, info)

		YAKE_MEMBERSIGNAL_VIRTUALIMPL(public, void(ActorCollisionInfo&), CollisionExited)
		YAKE_MEMBERSIGNAL_FIRE_FN1(public, CollisionExited, ActorCollisionInfo& info, info)
			

		// helpers
		void onLastShapeIsAboutToDestroy();
		NxActor* getNxActorPtr_() const
		{ 
			YAKE_ASSERT( mpNxActor );
			return mpNxActor; 
		}
	private:
		NxActor*			mpNxActor;
		NxScene&			mNxScene;
		WorldNx&			mWorld;

		typedef Deque< SharedPtr<ShapeNx> > Shapes;
		Shapes		mShapes;

		BodyNx*		mpBody;

		NxShape*	mpNxDefaultShape; ///WORKAROUND: used for a work-a-round: in order to create an actor one needs a shape in Nx. In Yake, you don't.
	};

}
}

#endif