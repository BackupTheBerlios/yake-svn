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
#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeJoint.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeActor.h>
#include <yake/plugins/physicsODE/OdeShapes.h>

#if (YAKE_PLATFORM == PLATFORM_WIN32) && (YAKE_COMPILER == COMPILER_MSVC)
#	define ADJUST_FPU_PRECISION
#endif

namespace yake {
namespace physics {

		//-----------------------------------------------------
		OdeWorld::OdeWorld() : mNextMeshId( 0 )
		{
			mStepSize = real(1. / 100.);	// default: 50Hz
			mOdeWorld = new dWorld();
			YAKE_ASSERT( mOdeWorld );
			mOdeSpace = new dSimpleSpace( 0 );
			YAKE_ASSERT( mOdeSpace );
			
			mOdeContactGroup = new dJointGroup( 0 );

			mOdeWorld->setGravity( 0., -9.81, 0. );
			mOdeWorld->setCFM( 0.0005 );
			mOdeWorld->setERP( 0.99 );
			mOdeWorld->setAutoDisableFlag( 1 );
			mOdeWorld->setAutoDisableAngularThreshold( 0.005 ); // ODE default: 0.01
			mOdeWorld->setAutoDisableLinearThreshold( 0.005 ); // ODE default: 0.01
			mOdeWorld->setAutoDisableSteps( 10 ); // ODE default: 10
			mOdeWorld->setAutoDisableTime( 0 ); // ODE default: 0. (= ignore time)

			dWorldSetQuickStepNumIterations( mOdeWorld->id(), 20 );

			//dWorldSetContactMaxCorrectingVel( mOdeWorld->id(), 2. );
			dWorldSetContactSurfaceLayer( mOdeWorld->id(), 0.0075 );
		}

		//-----------------------------------------------------
 		OdeWorld::~OdeWorld()
 		{
			for ( MeshDataMap::iterator i = mMeshDataMap.begin(); i != mMeshDataMap.end(); ++i )
			{
				dGeomTriMeshDataDestroy( i->second );
			}

 			YAKE_SAFE_DELETE( mOdeContactGroup );
 			YAKE_SAFE_DELETE( mOdeSpace );
 			YAKE_SAFE_DELETE( mOdeWorld );
 		}
			
		//-----------------------------------------------------
		void OdeWorld::setGlobalGravity( const Vector3& g )
		{
			YAKE_ASSERT( mOdeWorld );
			mOdeWorld->setGravity( g.x, g.y, g.z );
		}

		//-----------------------------------------------------
		Vector3 OdeWorld::getGlobalGravity() const
		{
			YAKE_ASSERT( mOdeWorld );
			dVector3 g;
			mOdeWorld->getGravity( g );
			return Vector3(real(g[0]), real(g[1]), real(g[2]));
		}

		//-----------------------------------------------------
		Deque<ShapeType> OdeWorld::getSupportedShapes( bool includeStatic, bool includeDynamic ) const
		{
			/// TODO Add real implementation! FIXME
			Deque<ShapeType> supportedShapes;
			
			return supportedShapes;
		}
		
		//-----------------------------------------------------
		Deque<JointType> OdeWorld::getSupportedJoints() const
		{
			//TODO Add real implementation
			Deque<JointType> supportedJoints;
			
			return supportedJoints;
		}
		
		//-----------------------------------------------------
		Deque<String> OdeWorld::getSupportedSolvers() const
		{
			//TODO Add real implementation
			Deque<String> supportedSolvers;
			
			supportedSolvers.push_back( "quickStep" );
			supportedSolvers.push_back( "StepFast" );
			
			return supportedSolvers;
		}
		
		//-----------------------------------------------------
		bool OdeWorld::useSolver( String const& rSolver )
		{
			//TODO add solver functor!
			return false;
		}
		
		//-----------------------------------------------------
		String OdeWorld::getCurrentSolver() const
		{
			//TODO add current solver name variable
			return "quickStep";
		}
			
		//-----------------------------------------------------
		const StringVector OdeWorld::getCurrentSolverParams() const
		{
			return mCurrentSolverParams;
		}
		
		//-----------------------------------------------------
		void OdeWorld::setCurrentSolverParam( String const& rName, boost::any const& rValue )
		{
			//TODO add implementation
		}

		//-----------------------------------------------------
		IJointPtr OdeWorld::createJoint( IJoint::DescBase const& rJointDesc )
		{
			//@todo add joints "actor vs static env"
			YAKE_ASSERT( rJointDesc.actor0 && rJointDesc.actor1 );
			if (!rJointDesc.actor0 || !rJointDesc.actor1)
				return 0;

			/// Bodies to attach
			IBody& rBody1 = rJointDesc.actor0->getBody();
			IBody& rBody2 = rJointDesc.actor1->getBody();
			
			/// Joint to return
			OdeJoint* pJoint = 0;

			switch( rJointDesc.type )
			{
			case JT_FIXED:
				{
				pJoint = new OdeFixedJoint( this );
				}
				break;
			case JT_HINGE:
				{
				pJoint = new OdeHingeJoint( this );
				const IJoint::DescHinge& desc = static_cast<const IJoint::DescHinge&>( rJointDesc );
				pJoint->setAnchor( 0, desc.anchor );
				pJoint->setAxis( 0, desc.axis );
				}
				break;
			case JT_HINGE2:
				{
				pJoint = new OdeHinge2Joint( this );
				const IJoint::DescHinge2& desc = static_cast<const IJoint::DescHinge2&>( rJointDesc );
				pJoint->setAnchor( 0, desc.anchor );
				pJoint->setAxis( 0, desc.axis0 );
				pJoint->setAxis( 1, desc.axis1 );
				}
				break;
			case JT_UNIVERSAL:
				{
				pJoint = new OdeUniversalJoint( this );
				const IJoint::DescUniversal& desc = static_cast<const IJoint::DescUniversal&>( rJointDesc );
				pJoint->setAnchor( 0, desc.anchor );
				pJoint->setAxis( 0, desc.axis0 );
				pJoint->setAxis( 1, desc.axis1 );
				}
				break;
			case JT_BALL:
				{
				pJoint = new OdeBallJoint( this );
				const IJoint::DescBall& desc = static_cast<const IJoint::DescBall&>( rJointDesc );
				pJoint->setAnchor( 0, desc.anchor );
				}
				break;
			case JT_SLIDER:
				{
				pJoint = new OdeSliderJoint( this );
				const IJoint::DescSlider& desc = static_cast<const IJoint::DescSlider&>( rJointDesc );
				pJoint->setAxis( 0, desc.axis );
				}
				break;
			default:
				YAKE_ASSERT( 0 && "unhandled type" )(rJointDesc.type).warning("");
				return 0;
			};
			YAKE_ASSERT( pJoint != NULL ).error( "Unknown type of joint or ... some other error :(" );
			
			pJoint->attach( rBody1, rBody2 );
			mJoints.push_back( SharedPtr<OdeJoint>(pJoint) );
			
			return pJoint;
		}
		
		//-----------------------------------------------------
		IActorPtr OdeWorld::createActor( const IActor::Desc& rActorDesc )
		{
			OdeActor* pActor = new OdeActor( this, (rActorDesc.type == ACTOR_DYNAMIC) );
			YAKE_ASSERT( pActor );
			
			typedef Deque< SharedPtr<IShape::Desc> > ActorShapesCollection;
			const ActorShapesCollection& shapes = rActorDesc.shapes;
			
			for ( ActorShapesCollection::const_iterator i = shapes.begin(); i != shapes.end(); ++i )
			{
				SharedPtr<IShape::Desc> pShapeDesc = *i;
				pActor->createShape(  *pShapeDesc );
			}  
			mActors.push_back( SharedPtr<OdeActor>( pActor ) );
			return pActor;
		}
		bool operator==(const SharedPtr<OdeJoint>& lhs, const OdeJoint* rhs)
		{
			return (lhs.get() == rhs);
		}
		void OdeWorld::destroyJoint( IJointPtr pJoint )
		{
			YAKE_ASSERT( pJoint );
			mJoints.erase( std::find(mJoints.begin(), mJoints.end(), dynamic_cast<OdeJoint*>(pJoint) ) );
		}
		bool operator==(const SharedPtr<OdeActor>& lhs, const OdeActor* rhs)
		{
			return (lhs.get() == rhs);
		}
		void OdeWorld::destroyActor( IActorPtr pActor )
		{
			YAKE_ASSERT( pActor );
			mActors.erase( std::find(mActors.begin(), mActors.end(), dynamic_cast<OdeActor*>(pActor) ) );
		}
		void OdeWorld::destroyAvatar( IAvatarPtr pAvatar )
		{
			YAKE_ASSERT( 0 && "NOT IMPLEMENTED" );
			//YAKE_ASSERT( !pAvatar.expired() );
			//mAvatars.erase( std::find(mAvatars.begin(), mAvatars.end(), dynamic_cast<OdeAvatar*>(pAvatar .lock().get()) ) );
		}
		bool operator==(const SharedPtr<OdeMaterial>& lhs, const OdeMaterial* rhs)
		{
			return (lhs.get() == rhs);
		}
		void OdeWorld::destroyMaterial( IMaterialPtr pMaterial )
		{
			YAKE_ASSERT( pMaterial );
			mMaterials.erase( std::find(mMaterials.begin(), mMaterials.end(), dynamic_cast<OdeMaterial*>(pMaterial) ) );
		}

		//-----------------------------------------------------
		IMaterialPtr OdeWorld::createMaterial( IMaterial::Desc const& rMatDesc )
		{
			SharedPtr<OdeMaterial> pMat( new OdeMaterial );
			mMaterials.push_back( pMat );
			return pMat.get();
		}

		//-----------------------------------------------------
		void OdeWorld::step( const real timeElapsed )
		{
			static real overflow = 0.;
			real t = overflow + timeElapsed;
			firePreStep();
			while ( t > mStepSize )
			{
				t -= mStepSize;
		
		#ifdef ADJUST_FPU_PRECISION
				_controlfp(_PC_64, _MCW_PC);
		#endif
				
				// Now collide the objects in the world
				dSpaceCollide( mOdeSpace->id(), this, &_OdeNearCallback );

				//mOdeWorld->step( mStepSize );
				//mOdeWorld->stepFast1( mStepSize, 4 );
				dWorldQuickStep( mOdeWorld->id(), mStepSize );
				//dWorldStepFast1( mOdeWorld->id(), mStepSize, 4 );

		#ifdef ADJUST_FPU_PRECISION
				_controlfp(_PC_24, _MCW_PC);
		#endif

				// Clear contacts
				mOdeContactGroup->empty();
			}
			
			firePostStep();

			overflow = t;
		}
		
		//-----------------------------------------------------
		TriangleMeshId OdeWorld::createTriangleMesh( TriangleMeshDesc const& rTriMeshDesc )
		{
			dTriMeshDataID dataId = OdeTriMesh::buildMeshData(	rTriMeshDesc.vertices,
																rTriMeshDesc.indices,
																rTriMeshDesc.normals );

			mMeshDataMap.insert( MeshDataMap::value_type( mNextMeshId, dataId ) );
			
			return mNextMeshId++;
		}
		//-----------------------------------------------------
		dTriMeshDataID OdeWorld::getMeshDataById(  TriangleMeshId id ) const
		{
			MeshDataMap::const_iterator i = mMeshDataMap.find( id );
			
			YAKE_ASSERT( i != mMeshDataMap.end() ).error( "No mesh data matches supplied mesh id!" );
				
			return i->second;
		}

		//-----------------------------------------------------
		dWorldID OdeWorld::_getOdeID() const
		{
			return mOdeWorld->id();
		}
		
		//-----------------------------------------------------
		void OdeWorld::_OdeNearCallback ( void* data, dGeomID o1, dGeomID o2 )
		{
			//YAKE_LOG("OdeWorld::->NearCallBack()");
			// make sure we traverse through all spaces and sub-spaces
			if ( dGeomIsSpace (o1) || dGeomIsSpace (o2) )
			{
				// colliding a space with something
				dSpaceCollide2 ( o1, o2, data, &_OdeNearCallback );

				// collide all geoms internal to the space(s)
				if ( dGeomIsSpace(o1) ) dSpaceCollide( dSpaceID(o1), data, &_OdeNearCallback );
				if ( dGeomIsSpace(o2) ) dSpaceCollide( dSpaceID(o2), data, &_OdeNearCallback );
			}

			// colliding two non-space geoms, so generate contact joints
			dJointGroup* odeContactGroup = ( static_cast<OdeWorld*>(data) )->_getOdeContactJointGroup();
			dWorldID odeWorldID = ( static_cast<OdeWorld*>(data) )->_getOdeID();

			// exit without doing anything if the two bodies are connected by a joint
			dBodyID b1 = dGeomGetBody(o1);
			dBodyID b2 = dGeomGetBody(o2);
			
			if ( b1 && b2 && dAreConnected( b1, b2 ) )
				return;

			int t1 = dGeomGetClass( o1 );
			int t2 = dGeomGetClass( o2 );

			OdeActor* pA = static_cast<OdeGeom*>( dGeomGetData( o1 ) )->getOwner();
			OdeActor* pB = static_cast<OdeGeom*>( dGeomGetData( o2 ) )->getOwner();
			
			if ( pA && pB )
			{
				// collide
				pA->_collide( pB, o1, o2, odeContactGroup );
				return;
			}
		}

} // physics
} // yake
