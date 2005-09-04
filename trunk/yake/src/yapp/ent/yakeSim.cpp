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
#include <yapp/ent/yakePCH.h>
#include <yapp/ent/yakeCommon.h>
#include <yapp/ent/yakeEvent.h>
#include <yapp/ent/yakeMessaging.h>
#include <yapp/ent/yakeObject.h>
#include <yapp/ent/yakeEntityMachine.h>
#include <yapp/ent/yakeEntityComponent.h>
#include <yapp/ent/yakeEntity.h>
#include <yapp/ent/yakeSim.h>

namespace yake {
namespace ent {

	namespace private_ {
		sim* g_sim = 0;
	}
	sim& sim::getSim()
	{
		return *(private_::g_sim);
	}

	sim::sim(	scripting::IScriptingSystem& scriptingSystem,
				const real ticksPerSecond, 
				graphics::IWorld* pGWorld /*= 0*/,
				audio::IWorld* pAWorld /*= 0*/,
				physics::IWorld* pPWorld /*= 0*/
			) : 
		mSimTime(0),
		mpGWorld(pGWorld),
		mpAWorld(pAWorld),
		mpPWorld(pPWorld),
		mScripting(scriptingSystem),
		mEvtEntitySpawned("onEntitySpawned"),
		mEvtEntityVMCreated("onEntityVMCreated"),
		mTickTime(0.1f),
		mCurrTickTime(0.f),
		mSimTimeInSecs(0.f),
		mLastNumericClassId(1)
	{
		private_::g_sim = this;

		if (ticksPerSecond > 0)
			mTickTime = 1.f / ticksPerSecond;
	}
	sim::~sim()
	{
		mNonOwnedBinders.clear();
		mBinders.clear();
		mObjectCreators.clear();
		mObjects.clear();
		private_::g_sim = 0;
	}
	Event& sim::getEvent_onEntitySpawned()
	{
		return mEvtEntitySpawned;
	}
	Event& sim::getEvent_onEntityVMCreated()
	{
		return mEvtEntityVMCreated;
	}
	simtime sim::getTime() const
	{
		return mSimTime;
	}
	real sim::getTimeAsSeconds() const
	{
		return mSimTimeInSecs;
	}
	void sim::tick(const real timeElapsed)
	{
		mCurrTickTime += timeElapsed;
		while (mCurrTickTime > 0)
		{
			mMsgMgr.execute();

			ConstVectorIterator<ObjectList> itEntity( mObjects );
			while (itEntity.hasMoreElements())
			{
				itEntity.getNext()->tick();
			}

			// prepare for next step
			++mSimTime;
			mCurrTickTime -= mTickTime;
			mSimTimeInSecs += mTickTime;
		}
	}
	void sim::removeAndDestroyAll()
	{
		mObjects.clear();
	}
	void sim::regEntityCreator( const ObjectClassId& id, const ObjectCreatorFn& creatorFn )
	{
		mObjectCreators[ id ] = creatorFn;
		mNumericClassIds[ id ] = mLastNumericClassId++;
	}
	void sim::addEntityVMBinder( scripting::IBinder* pBinder, const bool bTransferOwnership )
	{
		YAKE_ASSERT( pBinder );
		if (!pBinder)
			return;
		if (bTransferOwnership)
			mBinders.push_back( BinderPtr( pBinder ) );
		else
			mNonOwnedBinders.push_back( pBinder );
	}
	scripting::IVM* sim::createEntityVM()
	{
		scripting::IVM* pVM = mScripting.createVM();
		YAKE_ASSERT( pVM );

		// bind complete entity system to VM
		for (BinderList::const_iterator itBinder = mBinders.begin(); itBinder != mBinders.end(); ++itBinder)
		{
			scripting::IBinder* pBinder = (*itBinder).get();
			YAKE_ASSERT( pBinder );
			if (!pBinder)
				continue;
			pBinder->bind( pVM );
		}
		for (PureBinderPtrList::const_iterator itBinder = mNonOwnedBinders.begin(); itBinder != mNonOwnedBinders.end(); ++itBinder)
		{
			scripting::IBinder* pBinder = (*itBinder);
			YAKE_ASSERT( pBinder );
			if (!pBinder)
				continue;
			pBinder->bind( pVM );
		}

		return pVM;
	}
	Entity* sim::createEntity( const ObjectClassId& id, const StringPairVector& components, const StringVector& scriptFiles )
	{
		ObjectCreateMap::const_iterator itFind = mObjectCreators.find( id );
		if (itFind == mObjectCreators.end())
		{
			std::stringstream msg;
			msg << "Could not find creator for object type: " << id.c_str();
			YAKE_EXCEPT(msg.str());
		}
		Object* pObject = (itFind->second)();
		Entity* pEntity = Entity::cast( pObject );
		YAKE_ASSERT( pEntity ).debug("The created object is an Object but not an Entity!");
		if (!pEntity)
		{
			delete pObject;
			return 0;
		}

		// create VMs & load & execute entity scripts
		const size_t numVMs = (scriptFiles.size() > 0) ? scriptFiles.size() : 1; // at least one VM
		ParamList params;
		params["object"] = pObject;
		params["entity"] = pEntity;
		for (size_t i=0; i<numVMs; ++i)
		{
			// create VM
			scripting::IVM* pVM = createEntityVM();
			pEntity->addVM( pVM );

			// fire "entity VM created"
			params["vm"] = pVM;
			mEvtEntityVMCreated.fire(params);

			// load & execute entity script
			if (i <= scriptFiles.size())
			{
				const String scriptFile = scriptFiles.at(i);
				if (scriptFile.empty())
					continue;
				AutoPtr<scripting::IScript> pScript( mScripting.createScriptFromFile(scriptFile) );
				YAKE_ASSERT( pScript.get() );
				pVM->execute( pScript.get() );
			}
		}

		// create components
		ConstVectorIterator<StringPairVector> itC( components );
		while (itC.hasMoreElements())
		{
			const StringPair stringPair = itC.getNext();
			pEntity->addComponent( stringPair.first, create<EntityComponent>( stringPair.second, *pEntity ) );
		}

		// minimal first time setup
		object_creation_context ctx;
		ctx.mpGWorld = mpGWorld;
		ctx.mpAWorld = mpAWorld;
		ctx.mpPWorld = mpPWorld;
		pEntity->initialise( ctx );

		// store entity & fire events
		mObjects.push_back( SharedPtr<Entity>(pEntity) );

		// fire "entity spawned"

		params.clear();
		params["object"] = pObject;
		params["entity"] = pEntity;
		for (size_t i=0; i<pEntity->getVMCount(); ++i)
		{
			params["vm"] = pEntity->getVM(i);
			mEvtEntitySpawned.fire(params);
		}

		// spawn entity itself

		pEntity->spawn();

		ObjectMessage* pMsg = pEntity->createMessage( MSGID_EntitySpawned );
		pMsg->set("entity", pEntity );
		mMsgMgr.postMessage( pMsg );

		return pEntity;
	}
	bool operator == (const SharedPtr<Object>& lhs, const Object* rhs)
	{ return lhs.get() == rhs; }
	void sim::destroyEntity( Entity* pEnt )
	{
		if (!pEnt)
			return;
		ObjectList::iterator itFind = std::find( mObjects.begin(), mObjects.end(), pEnt );
		if (itFind != mObjects.end())
			mObjects.erase( itFind );
	}
	void sim::postMessage(ObjectMessage* pMessage, Object* target)
	{
		YAKE_ASSERT( pMessage ).warning("null message");
		if (!pMessage)
			return;
		YAKE_ASSERT( target ).warning("null target");
		if (!target)
			return;
		// filter

		// forward/post
		target->postMessage( pMessage );
	}

} // namespace yake
} // namespace ent
