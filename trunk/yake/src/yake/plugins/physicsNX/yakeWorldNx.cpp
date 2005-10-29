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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakeShapeNx.h>
#include <yake/plugins/physicsNX/yakeActorNx.h>
#include <yake/plugins/physicsNX/yakeJointNx.h>
#include <yake/plugins/physicsNX/yakeAvatarNx.h>
#include <yake/plugins/physicsNX/yakePhysicsTrimeshNx.h>
#include <yake/plugins/physicsNX/yakeMaterialNx.h>
#include <yake/plugins/physicsNX/yakeWorldNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	WorldNx::WorldNx()
	{
	}
	WorldNx::~WorldNx()
	{
		std::for_each( mAvatars.begin(), mAvatars.end(), Deleter<AvatarNxVector>() );
		mAvatars.clear();

		std::for_each( mActors.begin(), mActors.end(), Deleter<ActorNxVector>() );
		mActors.clear();

		std::for_each( mJoints.begin(), mJoints.end(), Deleter<JointNxVector>() );
		mJoints.clear();

		ConstVectorIterator< MaterialNxVector > itMat( mMaterials );
		while (itMat.hasMoreElements())
			delete itMat.getNext().second;
		mMaterials.clear();

		TrimeshNxManager::destroyAll();
		getNxSDK()->releaseScene( *mpScene );
		mpScene = 0;
	}
	void WorldNx::regActor_(NxActor* pNxActor, ActorNx* pActor)
	{
		YAKE_LOG("WorldNx::regActor_()");
		ContactActorMap::iterator it = mContactActors.find( pNxActor );
		YAKE_ASSERT(it == mContactActors.end()).debug("Attempted double registration!");

		VectorIterator<ContactActorMap> itA( mContactActors.begin(), mContactActors.end() );
		while (itA.hasMoreElements())
		{
			YAKE_LOG("	setting pair flags");
			mpScene->setActorPairFlags( *pNxActor, *(itA.getNext().first),
							NX_NOTIFY_ON_START_TOUCH|NX_NOTIFY_ON_END_TOUCH|NX_NOTIFY_ON_TOUCH );
		}

		mContactActors[ pNxActor ] = pActor;
	}
	void WorldNx::unregActor_(NxActor* pNxActor)
	{
		ContactActorMap::iterator it = mContactActors.find( pNxActor );
		YAKE_ASSERT(it != mContactActors.end()).debug("Unregistered actor cannot be unregistered!");
		if (it != mContactActors.end())
			mContactActors.erase( it );
	}
	NxU32 WorldNx::onPairCreated(NxActor& s1, NxActor& s2)
	{
		YAKE_LOG("WorldNx::onPairCreated()");
		return NX_NOTIFY_ON_START_TOUCH;
	}
	void WorldNx::onContactNotify(NxContactPair &pair, NxU32 events)
	{
		//YAKE_LOG("WorldNx::onContactNotify()");
		ContactActorMap::iterator it0 = mContactActors.find( pair.actors[0] );
		ContactActorMap::iterator it1 = mContactActors.find( pair.actors[1] );
		ActorCollisionInfo cInfo;
		if (it0 != mContactActors.end())
		{
			YAKE_ASSERT( it0->second );
			cInfo.pThis = it0->second;
		}
		if (it1 != mContactActors.end())
		{
			YAKE_ASSERT( it1->second );
			cInfo.pOther = it1->second;
		}
		if (cInfo.pThis)
		{
			if (NX_NOTIFY_ON_START_TOUCH & events)
				it0->second->fireCollisionEntered(cInfo);
			else if (NX_NOTIFY_ON_END_TOUCH & events)
				it0->second->fireCollisionExited(cInfo);
		}
		if (cInfo.pOther)
		{
			YAKE_ASSERT( it1->second );
			if (NX_NOTIFY_ON_START_TOUCH & events)
				it1->second->fireCollisionEntered(cInfo);
			else if (NX_NOTIFY_ON_END_TOUCH & events)
				it1->second->fireCollisionExited(cInfo);
		}
	}
	void WorldNx::setGlobalGravity( const Vector3& g )
	{
		YAKE_ASSERT( mpScene );
		mpScene->setGravity( toNx( g ) );
	}
	Vector3 WorldNx::getGlobalGravity() const
	{
		YAKE_ASSERT( mpScene );
		NxVec3 g;
		mpScene->getGravity(g);
		return fromNx( g );
	}
	bool WorldNx::init_()
	{
		if (!getNxSDK())
			return false;

		YAKE_ASSERT( getSupportedSolvers().size() > 0 );
		if (getSupportedSolvers().size() == 0)
			return false;

		if (!useSolver( getSupportedSolvers().front() ))
			return false;

		// create Nx scene //  // NX_BROADPHASE_COHERENT or NX_BROADPHASE_QUADRATIC or NX_BROADPHASE_FULL 
		NxSceneDesc sceneDesc;
		sceneDesc.gravity				= NxVec3(0,NxReal(-9.81),0);
		sceneDesc.broadPhase			= NX_BROADPHASE_COHERENT;
		sceneDesc.collisionDetection	= true;
		sceneDesc.userTriggerReport		= 0;
		sceneDesc.userNotify			= 0;
		//sceneDesc.maxTimestep			= ;
		//sceneDesc.maxIter				= ;
		//sceneDesc.boundsPlanes			= ;
		sceneDesc.groundPlane			= false;
		sceneDesc.userContactReport		= this;
		sceneDesc.timeStepMethod		= NX_TIMESTEP_FIXED;
		YAKE_ASSERT( sceneDesc.isValid() );
		mpScene = getNxSDK()->createScene(sceneDesc);
		YAKE_ASSERT( mpScene );

		getNxSDK()->setActorGroupPairFlags(0, 0, NX_NOTIFY_ON_START_TOUCH/*|NX_NOTIFY_ON_TOUCH*/|NX_NOTIFY_ON_END_TOUCH);

		mCurrTime = 0.;

		// default material
		NxMaterial*	defaultMaterial = mpScene->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(0.0f);
		defaultMaterial->setStaticFriction(0.9f);
		defaultMaterial->setDynamicFriction(0.9f);

		NxMaterialDesc defaultMaterialDesc;
		defaultMaterial->saveToDesc(defaultMaterialDesc);
		YAKE_ASSERT( defaultMaterialDesc.isValid() );

		return true;
	}
	void WorldNx::step(const real timeElapsed)
	{
		YAKE_ASSERT( mpScene );
		if (!mpScene)
			return;
		if (timeElapsed < 0.0001)
			return;
		firePreStep();
		firePreStepInternal( timeElapsed );
		mpScene->simulate( timeElapsed );
		mpScene->flushStream();
		mpScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

		mCurrTime += timeElapsed;
		firePostStep( mCurrTime );
	}
	/*
	WeakPtr<IMaterial> WorldNx::createMaterial( const IMaterial::Desc & rkMatDesc )
	{
		SharedPtr<MaterialNx> pMaterial( new MaterialNx() );
		YAKE_ASSERT( pMaterial );
		if (!pMaterial)
			return WeakPtr<IMaterial>();
		pMaterial->_createFromDesc( rkMatDesc );

		mMaterials.push_back( pMaterial );
		return pMaterial;
	}
	WeakPtr<IJoint> WorldNx::createJoint( const IJoint::DescBase & rkJointDesc )
	{
		YAKE_ASSERT( mpScene );
		if (!mpScene)
			return WeakPtr<IJoint>();
		SharedPtr<JointNx> pJoint( JointNx::createFromDesc_( *mpScene, rkJointDesc ) );
		if (!pJoint)
			return WeakPtr<IJoint>();
		mJoints.push_back( pJoint );
		return pJoint;
	}
	WeakPtr<IActor> WorldNx::createActor( const IActor::Desc & rkActorDesc )
	{
		YAKE_ASSERT( mpScene );
		if (!mpScene)
			return WeakPtr<IActor>();

		bool bDynamic = true;
		try {
			dynamic_cast<const IDynamicActor::Desc&>( rkActorDesc );
		} catch (std::bad_cast) {
			bDynamic = false;
		}
		std::cout << "NX: new actor is dynamic: " << bDynamic << "\n";
		SharedPtr<ActorNx> pActor( new ActorNx( *mpScene, *this, bDynamic ) );
		YAKE_ASSERT( pActor );
		if (!pActor)
			return WeakPtr<IActor>();

		ConstDequeIterator< Deque< SharedPtr< IShape::Desc > > > itShape( rkActorDesc.shapes.begin(), rkActorDesc.shapes.end() );
		while (itShape.hasMoreElements())
		{
			const SharedPtr<IShape::Desc> & pShapeDesc = itShape.getNext();
			YAKE_ASSERT( pShapeDesc );
			if (!pShapeDesc)
				continue;
			pActor->createShape( *pShapeDesc );
		}

		mActors.push_back( pActor );
		return pActor;
	}
	WeakPtr<IAvatar> WorldNx::createAvatar( const IAvatar::Desc & rkAvatarDesc )
	{
		YAKE_ASSERT( mpScene );
		SharedPtr<AvatarNx> pAvatar( new AvatarNx(*mpScene, *this) );
		YAKE_ASSERT( pAvatar );
		if (!pAvatar)
			return WeakPtr<IAvatar>();

		pAvatar->setPosition( rkAvatarDesc.position );
		pAvatar->setOrientation( rkAvatarDesc.orientation );

		mAvatars.push_back( pAvatar );
		return pAvatar;
	}
	void WorldNx::destroyJoint( WeakPtr<IJoint>& rJoint )
	{
		YAKE_ASSERT( !rJoint.expired() );
		mJoints.erase( std::find(mJoints.begin(), mJoints.end(), rJoint.lock() ) );
	}
	void WorldNx::destroyActor( WeakPtr<IActor>& rActor )
	{
		YAKE_ASSERT( !rActor.expired() );
		mActors.erase( std::find(mActors.begin(), mActors.end(), rActor.lock() ) );
	}
	void WorldNx::destroyAvatar( WeakPtr<IAvatar>& rAvatar )
	{
		YAKE_ASSERT( !rAvatar.expired() );
		mAvatars.erase( std::find(mAvatars.begin(), mAvatars.end(), rAvatar.lock() ) );
	}
	void WorldNx::destroyMaterial( WeakPtr<IMaterial>& rMaterial )
	{
		YAKE_ASSERT( !rMaterial.expired() );
		mMaterials.erase( std::find(mMaterials.begin(), mMaterials.end(), rMaterial.lock() ) );
	}
	*/
	IMaterialPtr WorldNx::createMaterial( const IMaterial::Desc & rkMatDesc, const String& id /*= ""*/ )
	{
		MaterialNx* pMaterial = new MaterialNx();
		YAKE_ASSERT( pMaterial );
		if (!pMaterial)
			return 0;
		pMaterial->_createFromDesc( rkMatDesc, mpScene );

		const String matId = (id.empty()) ? uniqueName::create("ode_mat_") : id;
		mMaterials.insert( std::make_pair(matId,pMaterial) );
		return pMaterial;
	}
	IMaterialPtr WorldNx::getMaterial( const String& id ) const
	{
		MaterialNxVector::const_iterator itFind = mMaterials.find( id );
		if (itFind == mMaterials.end())
			return 0;
		return itFind->second;
	}
	IJointPtr WorldNx::createJoint( const IJoint::DescBase & rkJointDesc )
	{
		YAKE_ASSERT( mpScene );
		if (!mpScene)
			return 0;
		JointNx* pJoint = JointNx::createFromDesc_( *mpScene, rkJointDesc );
		if (!pJoint)
			return 0;
		mJoints.push_back( pJoint );
		return pJoint;
	}
	ActorNx* WorldNx::_createActor(const IActor::Desc & rkActorDesc)
	{
		ActorNx* pActor = new ActorNx( *mpScene, *this, (rkActorDesc.type == ACTOR_DYNAMIC) );
		YAKE_ASSERT( pActor );
		if (!pActor)
			return 0;

		ConstDequeIterator< Deque< SharedPtr< IShape::Desc > > > itShape( rkActorDesc.shapes.begin(), rkActorDesc.shapes.end() );
		while (itShape.hasMoreElements())
		{
			const SharedPtr<IShape::Desc> & pShapeDesc = itShape.getNext();
			YAKE_ASSERT( pShapeDesc );
			if (!pShapeDesc)
				continue;
			pActor->createShape( *pShapeDesc );
		}

		mActors.push_back( pActor );
		return pActor;
	}
	IActorPtr WorldNx::createActor( const IActor::Desc& rActorDesc /*= IActor::Desc(ACTOR_MOVABLE)*/ )
	{
		YAKE_ASSERT( mpScene );
		if (!mpScene)
			return 0;

		std::cout << "NX: new actor\n";
		return _createActor(rActorDesc);
	}
	IAvatarPtr WorldNx::createAvatar( const IAvatar::Desc & rkAvatarDesc )
	{
		YAKE_ASSERT( mpScene );
		AvatarNx* pAvatar = new AvatarNx(*mpScene, *this);
		YAKE_ASSERT( pAvatar );

		pAvatar->setPosition( rkAvatarDesc.position );
		pAvatar->setOrientation( rkAvatarDesc.orientation );

		mAvatars.push_back( pAvatar );
		return pAvatar;
	}
	bool operator ==(const SharedPtr<JointNx>& lhs, const JointNx* rhs)
	{
		return (lhs.get() == rhs);
	}
	bool operator ==(const SharedPtr<ActorNx>& lhs, const ActorNx* rhs)
	{
		return (lhs.get() == rhs);
	}
	bool operator ==(const SharedPtr<AvatarNx>& lhs, const AvatarNx* rhs)
	{
		return (lhs.get() == rhs);
	}
	bool operator ==(const SharedPtr<MaterialNx>& lhs, const MaterialNx* rhs)
	{
		return (lhs.get() == rhs);
	}
	void WorldNx::destroyJoint( IJointPtr pJoint )
	{
		YAKE_ASSERT( pJoint );
		mJoints.remove( static_cast<JointNx*>(pJoint) );
		delete pJoint;
	}
	void WorldNx::destroyActor( IActorPtr pActor )
	{
		YAKE_ASSERT( pActor );
		mActors.remove( static_cast<ActorNx*>(pActor) );
		delete pActor;
	}
	void WorldNx::destroyAvatar( IAvatarPtr pAvatar )
	{
		YAKE_ASSERT( pAvatar );
		mAvatars.remove( static_cast<AvatarNx*>(pAvatar) );
		delete pAvatar;
	}
	void WorldNx::destroyMaterial( IMaterialPtr pMaterial )
	{
		YAKE_ASSERT( pMaterial );
		MaterialNxVector::iterator itFind = mMaterials.find( static_cast<MaterialNx*>(pMaterial)->getId() );
		if (itFind == mMaterials.end())
			return;
		mMaterials.erase( itFind );
		delete pMaterial;
	}
	TriangleMeshId WorldNx::createTriangleMesh( const TriangleMeshDesc & rkTrimeshDesc )
	{
		TriangleMeshId id = kTriangleMeshIdNone;
		NxTriangleMesh* pMesh = TrimeshNxManager::createMesh( rkTrimeshDesc, id );
		YAKE_ASSERT( pMesh );
		if (!pMesh)
			return kTriangleMeshIdNone;
		YAKE_ASSERT( kTriangleMeshIdNone != id );
		return id;
	}
	Deque<ShapeType> WorldNx::getSupportedShapes(bool bStatic, bool bDynamic) const
	{
		static Deque<ShapeType> shapeTypes;
		shapeTypes.push_back( ST_SPHERE );
		shapeTypes.push_back( ST_BOX );
		shapeTypes.push_back( ST_PLANE );
		shapeTypes.push_back( ST_CAPSULE );
		return shapeTypes;
	}
	Deque<JointType> WorldNx::getSupportedJoints() const
	{
		return JointNx::getSupportedTypes_();
	}
	Deque<String> WorldNx::getSupportedSolvers() const
	{
		static Deque<String> solvers;
		solvers.push_back("default");
		return solvers;
	}
	bool WorldNx::useSolver( const String & rkSolver )
	{
		Deque<String> solvers = getSupportedSolvers();
		if (std::find( solvers.begin(), solvers.end(), rkSolver ) == solvers.end())
			return false;
		//@todo switch solver...
		mCurrentSolver = rkSolver;
		return true;
	}
	String WorldNx::getCurrentSolver() const
	{
		return mCurrentSolver;
	}
	const StringVector WorldNx::getCurrentSolverParams() const
	{
		static StringVector propNames;
		return propNames;
	}
	void WorldNx::setCurrentSolverParam( const String & rkName, const boost::any & rkValue )
	{
	}

}
}
