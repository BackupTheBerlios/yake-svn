#ifndef ODE_ACTOR_H
#define ODE_ACTOR_H

namespace yake
{	
namespace physics
{
	class OdeWorld;
	class OdeGeom;
	
	class OdeActor
	{
	public:
		OdeActor( OdeWorld* pOdeWorld );
		virtual ~OdeActor();

		//virtual SharedPtr<IShape> createShape( const IShape::Desc& rShapeDesc );
		virtual void destroyShape_( IShape* pShape );
 		virtual const IActor::ShapePtrVector getShapes_() const;

		virtual void subscribeToCollisionEnteredSignal( IActor::SignalCollision::slot_type const& rSlot );
		virtual void subscribeToCollisionExitedSignal( IActor::SignalCollision::slot_type const& rSlot );
	
		// Ode helpers
		void _collide( OdeActor* pOther, dGeomID geomA, dGeomID geomB, dJointGroup* pContactJointGroup );
	
	protected:	
		OdeGeom* createShapeFromDesc( const IShape::Desc& rShapeDesc );
		OdeGeom* createTransformGeomIfNeeded( OdeGeom* pGeom, const Vector3& rOffset, const Quaternion& rRelOrientation );
		
		OdeWorld*								mOdeWorld;

		typedef Deque<SharedPtr<OdeGeom> > ShapeList;
		ShapeList								mShapes;
		
		IActor::SignalCollision					mEnterCollisionSignal;
		IActor::SignalCollision					mExitCollisionSignal;
		
		typedef real CollisionInfo;
		typedef std::map< OdeActor*, CollisionInfo > CollisionCache;
		CollisionCache							mCollisions;
	};
	
	class OdeStaticActor : public OdeActor, public IStaticActor
	{
	public:
		OdeStaticActor( OdeWorld* pOdeWorld );
		virtual ~OdeStaticActor();
		
		virtual IShape* createShape( const IShape::Desc& rShapeDesc );
		virtual void destroyShape( IShape* pShape )
		{ return OdeActor::destroyShape_( pShape ); }
		virtual const ShapePtrVector getShapes() const
		{ return OdeActor::getShapes_(); }

		virtual void subscribeToCollisionEnteredSignal( IActor::SignalCollision::slot_type const& rSlot );
		virtual void subscribeToCollisionExitedSignal( IActor::SignalCollision::slot_type const& rSlot );
	};
	
	class OdeMovableActor : public OdeActor, public IMovableActor
	{
	public:
		OdeMovableActor( OdeWorld* pOdeWorld );
		virtual ~OdeMovableActor();
		
		virtual void setPosition( const Vector3& rPosition );
		virtual void setOrientation( const Quaternion& rOrientation );
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;
		
		virtual IShape* createShape( const IShape::Desc& rShapeDesc );
		virtual void destroyShape( IShape* pShape )
		{ return OdeActor::destroyShape_( pShape ); }
		virtual const ShapePtrVector getShapes() const
		{ return OdeActor::getShapes_(); }

		virtual void subscribeToCollisionEnteredSignal( IActor::SignalCollision::slot_type const& rSlot );
		virtual void subscribeToCollisionExitedSignal( IActor::SignalCollision::slot_type const& rSlot );
	protected:
		// Movable actor "manually" tracks its position and orientation
		Vector3			mPosition;
		Quaternion		mOrientation;
	};
	
	class OdeDynamicActor : public OdeActor, public IDynamicActor
	{
	public:
		OdeDynamicActor( OdeWorld* pOdeWorld );
		virtual ~OdeDynamicActor();
	
		IBody& getBody() const;
		
		virtual void setPosition( const Vector3& rPosition );
		virtual void setOrientation( const Quaternion& rOrientation );
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;
		
		virtual IShape* createShape( const IShape::Desc& rShapeDesc );
		virtual void destroyShape( IShape* pShape )
		{ return OdeActor::destroyShape_( pShape ); }
		virtual const ShapePtrVector getShapes() const
		{ return OdeActor::getShapes_(); }

		virtual void subscribeToCollisionEnteredSignal( IActor::SignalCollision::slot_type const& rSlot );
		virtual void subscribeToCollisionExitedSignal( IActor::SignalCollision::slot_type const& rSlot );
	protected:
		OdeBody*			mBody; 
	};
	
} // physics
} // yake

#endif
