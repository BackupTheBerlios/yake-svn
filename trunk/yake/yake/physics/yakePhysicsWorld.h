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
#ifndef YAKE_PHYSICS_WORLD_H
#define YAKE_PHYSICS_WORLD_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

#ifndef YAKE_PHYSICS_AVATAR_H
#	include <yake/physics/yakePhysicsAvatar.h>
#endif
#ifndef YAKE_PHYSICS_JOINT_H
#	include <yake/physics/yakePhysicsJoint.h>
#endif
#ifndef YAKE_PHYSICS_ACTOR_H
#	include <yake/physics/yakePhysicsActor.h>
#endif
#ifndef YAKE_PHYSICS_MOTORS_H
#	include <yake/physics/yakePhysicsMotors.h>
#endif
//#include <yake/base/mpl/yakeAbstractFactory.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	class IWorld	//: public mpl::AbstractFactory< mpl::sequences::list< IActor, IAvatar > >
					//: public IPropertyQueryHandler
	{
	public:
		virtual ~IWorld() {}

		virtual WeakIJointPtr createJoint( const IJoint::DescBase& rkJointDesc ) = 0;
		virtual WeakIStaticActorPtr createStaticActor( const IActor::Desc& rActorDesc = IActor::Desc() ) = 0;
		virtual WeakIMovableActorPtr createMovableActor( const IMovableActor::Desc& rActorDesc = IMovableActor::Desc() ) = 0;
		virtual WeakIDynamicActorPtr createDynamicActor( const IDynamicActor::Desc& rActorDesc = IDynamicActor::Desc() ) = 0;
		virtual WeakIAvatarPtr createAvatar( const IAvatar::Desc& rkAvatarDesc ) = 0;
		virtual WeakIMaterialPtr createMaterial( const IMaterial::Desc& rkMatDesc ) = 0;
		virtual void destroyJoint( WeakIJointPtr& rJoint ) = 0;
		virtual void destroyActor( WeakIActorPtr& rActor ) = 0;
		virtual void destroyAvatar( WeakIAvatarPtr& rAvatar ) = 0;
		virtual void destroyMaterial( WeakIMaterialPtr& rMaterial ) = 0;

		virtual TriangleMeshId createTriangleMesh( const TriangleMeshDesc& rTrimeshDesc ) = 0;

		virtual Deque<ShapeType> getSupportedShapes( bool bStatic = true, bool bDynamic = true ) const = 0;
		virtual Deque<JointType> getSupportedJoints() const = 0;
		virtual Deque<String> getSupportedSolvers() const = 0;
		virtual bool useSolver( const String& rSolver ) = 0;
		virtual String getCurrentSolver() const = 0;
		virtual const PropertyNameList getCurrentSolverParams() const = 0;
		virtual void setCurrentSolverParam( const String& rName, const boost::any& rValue ) = 0;

		virtual void step(const real timeElapsed) = 0;

		typedef Signal0<void> PostStepSignal;
		virtual void subscribeToPostStep(const PostStepSignal::slot_type& slot) = 0;
	};

}
}
#endif