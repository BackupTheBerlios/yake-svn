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

namespace yake {
	using namespace ::yake::math;
namespace physics {

	class YAKE_PHYSICS_API IWorld
	{
	public:
		virtual ~IWorld() {}

		virtual IJointPtr createJoint( const IJoint::DescBase& rkJointDesc ) = 0;
		virtual IActorPtr createActor( const IActor::Desc& rActorDesc = IActor::Desc(ACTOR_MOVABLE) ) = 0;
		IActorPtr createActor( const ActorType type = ACTOR_MOVABLE )
		{ 
			IActor::Desc desc(type);
			return createActor( desc ); 
		}

		virtual IAvatarPtr createAvatar( const IAvatar::Desc& rkAvatarDesc ) = 0;
		virtual IMaterialPtr createMaterial( const IMaterial::Desc& rkMatDesc, const String& id = "" ) = 0;
		virtual IMaterialPtr getMaterial( const String& id ) const = 0;
		virtual void destroyJoint( IJointPtr pJoint ) = 0;
		virtual void destroyActor( IActorPtr pActor ) = 0;
		virtual void destroyAvatar( IAvatarPtr pAvatar ) = 0;
		virtual void destroyMaterial( IMaterialPtr pMaterial ) = 0;

		virtual TriangleMeshId createTriangleMesh( const TriangleMeshDesc& rTrimeshDesc ) = 0;

		virtual Deque<ShapeType> getSupportedShapes( bool bStatic = true, bool bDynamic = true ) const = 0;
		virtual Deque<JointType> getSupportedJoints() const = 0;
		virtual StringVector getSupportedSolvers() const = 0;
		virtual bool useSolver( const String& rSolver ) = 0;
		virtual String getCurrentSolver() const = 0;
		virtual const StringVector getCurrentSolverParams() const = 0;
		virtual void setCurrentSolverParam( const String& rName, const boost::any& rValue ) = 0;

		virtual void setGlobalGravity( const math::Vector3& g ) = 0;
		virtual math::Vector3 getGlobalGravity() const = 0;

		virtual void step(const real timeElapsed) = 0;
		virtual real getStepSize() const = 0;

		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(void), PreStep )
		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(real), PostStep )
		YAKE_MEMBERSIGNAL_PUREINTERFACE( public, void(const real), PreStepInternal )
	};

}
}
#endif

