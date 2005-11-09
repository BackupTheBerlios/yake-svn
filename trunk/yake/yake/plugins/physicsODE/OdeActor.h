/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef ODE_ACTOR_H
#define ODE_ACTOR_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>

namespace yake
{	
namespace physics
{
	class OdeWorld;
	class OdeGeom;
	
	class YAKE_PHYSICSODE_API OdeActor : public IActor
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
		static const int MAX_CONTACTS= 40;

		IBody::MassDesc* createMassDescFromShapeDesc( const IShape::Desc& rShapeDesc, real massOrDensity, IBody::quantityType qType );
		
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
