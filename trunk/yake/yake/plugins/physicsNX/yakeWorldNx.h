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
#ifndef YAKE_NX_WORLD_H
#define YAKE_NX_WORLD_H

namespace yake {
namespace physics {

	class ActorNx;
	class MaterialNx;
	class WorldNx : public IWorld, public NxUserContactReport
	{
	public:
		WorldNx();
		virtual ~WorldNx();

		virtual SharedPtr<IJoint> createJoint( const IJoint::DescBase & rkJointDesc );
		virtual SharedPtr<IActor> createActor( const IActor::Desc & rkActorDesc );
		virtual SharedPtr<IAvatar> createAvatar( const IAvatar::Desc & rkAvatarDesc );
		virtual SharedPtr<IMaterial> createMaterial( const IMaterial::Desc & rkMatDesc );

		virtual TriangleMeshId createTriangleMesh( const TriangleMeshDesc & rkTrimeshDesc );

		virtual Deque<ShapeType> getSupportedShapes(bool bStatic = true, bool bDynamic = true) const;
		virtual Deque<JointType> getSupportedJoints() const;
		virtual Deque<String> getSupportedSolvers() const;
		virtual bool useSolver( const String & rkSolver );
		virtual String getCurrentSolver() const;
		virtual const PropertyNameList& getCurrentSolverParams() const;
		virtual void setCurrentSolverParam( const String & rkName, const boost::any & rkValue );

		virtual void step(const real timeElapsed);

		//-- NxUserContactReport interface
		virtual void onContactNotify (NxContactPair &pair, NxU32 events);

		//-- helpers
		bool init_();
		void regActor_(NxActor* pNxActor, ActorNx* pActor);
		void unregActor_(NxActor* pNxActor);

		typedef Signal1<void(const real)> StepSignal;
		StepSignal	stepSignal;

	private:
		String					mCurrentSolver;
		NxScene*				mpScene;

		typedef AssocVector< NxActor*, ActorNx* > ContactActorMap;
		ContactActorMap			mContactActors;
	};

}
}


#endif