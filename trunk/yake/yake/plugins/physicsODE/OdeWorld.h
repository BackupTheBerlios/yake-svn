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
#ifndef YAKE_ODEWORLD_H
#define YAKE_ODEWORLD_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>
#include "yake/plugins/physicsODE/OdeShapes.h"

namespace yake {

	using namespace math;

namespace physics {

		class OdeBody;
		class OdeTriMesh;
		class OdeActor;
		class OdeJoint;
		class OdeAvatar;
		class OdeMaterial;
		
		class YAKE_PHYSICSODE_API OdeWorld : public IWorld
		{
		public:
			OdeWorld();
			virtual ~OdeWorld();

			virtual IJointPtr createJoint( const IJoint::DescBase & rkJointDesc );
			virtual IActorPtr createActor( const IActor::Desc& rActorDesc = IActor::Desc(ACTOR_MOVABLE) );
			virtual IAvatarPtr createAvatar( const IAvatar::Desc & rkAvatarDesc )
			{
				YAKE_ASSERT( 0 ).warning("not supported.");
				return IAvatarPtr();
			}
			virtual IMaterialPtr createMaterial( const IMaterial::Desc & rkMatDesc );
			virtual void destroyJoint( IJointPtr pJoint );
			virtual void destroyActor( IActorPtr pActor );
			virtual void destroyAvatar( IAvatarPtr pAvatar );
			virtual void destroyMaterial( IMaterialPtr pMaterial );
			
			virtual TriangleMeshId createTriangleMesh( TriangleMeshDesc const& rTrimeshDesc );

			virtual Deque<ShapeType> getSupportedShapes( bool bStatic = true, bool bDynamic = true) const;
			virtual Deque<JointType> getSupportedJoints() const;
			virtual Deque<String> getSupportedSolvers() const;
			virtual bool useSolver( String const& rSolver );
			virtual String getCurrentSolver() const;
			virtual const StringVector getCurrentSolverParams() const;
			virtual void setCurrentSolverParam( String const& rName, boost::any const& rValue );

			virtual void step( const real timeElapsed );
			virtual void setGlobalGravity( const Vector3& g );
			virtual Vector3 getGlobalGravity() const;

			YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, PreStep )
			YAKE_MEMBERSIGNAL_FIRE_FN0( public, PreStep )
			YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, PostStep )
			YAKE_MEMBERSIGNAL_FIRE_FN1( public, PostStep, real t, t )
			YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, PreStepInternal )
			YAKE_MEMBERSIGNAL_FIRE_FN1( public, PreStepInternal, const real dt, dt )
		public:

			// helpers
			dWorldID _getOdeID() const;
			dJointGroup* _getOdeContactJointGroup() const 
			{ return mOdeContactGroup; }
			dSpace* _getOdeSpace() const
			{ return mOdeSpace; }

			static void _OdeNearCallback (void *data, dGeomID o1, dGeomID o2);

			void _addBody( OdeBody* body );
			void _removeBody( OdeBody* body );

			real _getStepSize() const
			{ return mStepSize; }

			OdeTriMesh::MeshData getMeshDataById(  TriangleMeshId id ) const;
		
		protected:
			real				mTime;
			real				mStepSize;
			dWorld*				mOdeWorld;
			dJointGroup*		mOdeContactGroup;
			dSpace*				mOdeSpace;

			typedef std::vector< OdeBody* > BodyList;
			BodyList			mBodies;
		
			TriangleMeshId		mNextMeshId;	
			typedef AssocVector< TriangleMeshId, OdeTriMesh::MeshData > MeshDataMap;
			MeshDataMap			mMeshDataMap;

			StringVector		mCurrentSolverParams;

			typedef Deque<SharedPtr<OdeActor> > OdeActorVector;
			OdeActorVector			mActors;

			typedef Deque<SharedPtr<OdeJoint> > OdeJointVector;
			OdeJointVector			mJoints;

			typedef Deque<SharedPtr<OdeMaterial> > OdeMaterialVector;
			OdeMaterialVector		mMaterials;

			typedef Deque<SharedPtr<OdeAvatar> > OdeAvatarVector;
			OdeAvatarVector			mAvatars;
		};
	}
}

#endif
