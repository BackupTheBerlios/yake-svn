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
#ifndef YAKE_ODEWORLD_H
#define YAKE_ODEWORLD_H

namespace yake {

	using namespace base::math;

namespace physics {

		class OdeBody;
		class OdeTriMesh;
		
		class OdeWorld : public IWorld
		{
		public:
			OdeWorld();
			virtual ~OdeWorld();

			virtual SharedPtr<IJoint> createJoint( IJoint::DescBase const& rJointDesc );
			virtual SharedPtr<IActor> createActor( IActor::Desc const& rActorDesc = IActor::Desc() );
			virtual SharedPtr<IMaterial> createMaterial( IMaterial::Desc const& rMatDesc );
			virtual SharedPtr<IAvatar> createAvatar( IAvatar::Desc const& rAvatarDesc ) 
			{
				YAKE_ASSERT( 0 ).warning("not supported.");
				return SharedPtr<IAvatar>();
			}
			
			virtual TriangleMeshId createTriangleMesh( TriangleMeshDesc const& rTrimeshDesc );

			virtual Deque<ShapeType> getSupportedShapes( bool bStatic = true, bool bDynamic = true) const;
			virtual Deque<JointType> getSupportedJoints() const;
			virtual Deque<String> getSupportedSolvers() const;
			virtual bool useSolver( String const& rSolver );
			virtual String getCurrentSolver() const;
			virtual const PropertyNameList& getCurrentSolverParams() const;
			virtual void setCurrentSolverParam( String const& rName, boost::any const& rValue );

			virtual void step( const real timeElapsed );
			virtual void setGlobalGravity( Vector3 const& rAcceleration );


			///TODO what are these for?
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

			typedef Signal1< void(real) > PostStepSignal;
			base::templates::SignalConnection connectToPostStep( const PostStepSignal::slot_type & slot )
			{
				return mPostStepSignal.connect( slot );
			}
			
			dTriMeshDataID getMeshDataById(  TriangleMeshId id ) const;
		
		protected:
			real				mStepSize;
			dWorld*				mOdeWorld;
			dJointGroup*		mOdeContactGroup;
			dSpace*				mOdeSpace;

			typedef std::vector< OdeBody* > BodyList;
			BodyList			mBodies;
		
			TriangleMeshId		mNextMeshId;	
			typedef base::templates::AssocVector< TriangleMeshId, dTriMeshDataID > MeshDataMap;
			MeshDataMap			mMeshDataMap;

			PostStepSignal		mPostStepSignal;
			PropertyNameList	mCurrentSolverParams;
		};
	}
}

#endif
