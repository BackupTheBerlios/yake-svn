#ifndef ODE_ACTOR_H
#define ODE_ACTOR_H

namespace yake
{	
namespace physics
{
	class OdeWorld;
	class OdeGeom;
	
	class OdeActor : public IActor
	{
	public:
		OdeActor( OdeWorld* pOdeWorld, bool bDynamic );
		virtual ~OdeActor();

		IBody& getBody() const;
		IBody* getBodyPtr() const;

		virtual IWorld* getCreator() const;

		virtual void setPosition( const Vector3& rPosition );
		virtual void setOrientation( const Quaternion& rOrientation );
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;
		
		virtual IShape* createShape( const IShape::Desc& rShapeDesc, real massOrDensity = 0.0f, IBody::quantityType type = IBody::QT_MASS );
		virtual void destroyShape( IShape* pShape );
		virtual IShapePtrList getShapes() const;

		virtual void setEnabled(const bool enabled);
		virtual bool isEnabled() const;

		YAKE_MEMBERSIGNAL_VIRTUALIMPL(public, void(ActorCollisionInfo&), CollisionEntered)
		YAKE_MEMBERSIGNAL_FIRE_FN1(public, CollisionEntered, ActorCollisionInfo& info, info)

		YAKE_MEMBERSIGNAL_VIRTUALIMPL(public, void(ActorCollisionInfo&), CollisionExited)
		YAKE_MEMBERSIGNAL_FIRE_FN1(public, CollisionExited, ActorCollisionInfo& info, info)
	
		// Ode helpers
		void _collide( OdeActor* pOther, dGeomID geomA, dGeomID geomB, dJointGroup* pContactJointGroup );
	
	protected:	
		OdeGeom* createShapeFromDesc( const IShape::Desc& rShapeDesc );
		OdeGeom* createTransformGeomIfNeeded( OdeGeom* pGeom, const Vector3& rOffset, const Quaternion& rRelOrientation );

		Vector3							mPosition;
		Quaternion						mOrientation;
		
		OdeBody*						mBody; 
		OdeWorld*						mOdeWorld;

		typedef Deque<SharedPtr<OdeGeom> > ShapeList;
		ShapeList						mShapes;
		
		typedef real CollisionInfo;
		typedef std::map< OdeActor*, CollisionInfo > CollisionCache;
		CollisionCache					mCollisions;
	};

} // physics
} // yake

#endif
