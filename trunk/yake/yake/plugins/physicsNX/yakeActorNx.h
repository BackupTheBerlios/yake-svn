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
#ifndef YAKE_PHYSICS_ACTORNX_H
#define YAKE_PHYSICS_ACTORNX_H

namespace yake {
namespace physics {

	class BodyNx;
	class ShapeNx;
	class WorldNx;
	class ActorNx : public IActor
	{
	public:
		ActorNx( NxScene & rScene, WorldNx & rWorld );
		virtual ~ActorNx();

		virtual bool hasBody() const;
		virtual bool createBody();
		virtual IBody* getBody() const;

		virtual SharedPtr<IShape> createShape( const IShape::Desc & rkShapeDesc );
		virtual void destroyShape( SharedPtr<IShape> & rShape );
		virtual const ShapePtrVector getShapes() const;

		typedef Signal0<void> SignalCollisionEntered;
		virtual void subscribeToCollisionEnteredSignal( const SignalCollisionEntered::slot_type & rkSlot );
		virtual void subscribeToCollisionExitedSignal( const SignalCollisionEntered::slot_type & rkSlot );

		virtual void setPosition( const Vector3 & rkPosition );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & rkOrientation );
		virtual Quaternion getOrientation() const;

		// helpers
		void onLastShapeIsAboutToDestroy();
		NxActor* getNxActorPtr_() const
		{ 
			YAKE_ASSERT( mpNxActor );
			return mpNxActor; 
		}
		SignalCollisionEntered	fireCollisionEntered;
	private:
		NxActor*	mpNxActor;
		NxScene&	mNxScene;
		WorldNx&	mWorld;

		typedef Deque< SharedPtr<ShapeNx> > Shapes;
		Shapes		mShapes;

		BodyNx*		mpBody;

		NxShape*	mpNxDefaultShape; ///WORKAROUND: used for a work-a-round: in order to create an actor one needs a shape in Nx. In Yake, you don't.
	};

}
}

#endif