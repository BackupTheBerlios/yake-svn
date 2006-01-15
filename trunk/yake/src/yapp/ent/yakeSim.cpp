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
		Simulation* g_sim = 0;
	}
	Simulation& Simulation::getSim()
	{
		return *(private_::g_sim);
	}

	Simulation::Simulation() :
		mpGWorld(0),
		mpAWorld(0),
		mpPWorld(0),
		mpScripting(0),
		mEvtEntityVMSpawned("onEntitySpawned"),
		mEvtEntityVMCreated("onEntityVMCreated"),
		mSimTime(0),
		mTickTime(0.1f),
		mCurrTickTime(0.f),
		mSimTimeInSecs(0.f)
	{
	}
	Simulation::~Simulation()
	{
		stop();
	}
	bool Simulation::start(	scripting::IScriptingSystem* pScripting /*= 0*/,
					graphics::IWorld* pGWorld /*= 0*/,
					audio::IWorld* pAWorld /*= 0*/,
					physics::IWorld* pPWorld /*= 0*/)
	{
		private_::g_sim = this;
		mpScripting = pScripting;
		mpGWorld = pGWorld;
		mpAWorld = pAWorld;
		mpPWorld = pPWorld;
		return true;
	}
	void Simulation::stop()
	{
		removeAndDestroyObjects();
		mNonOwnedBinders.clear();
		mBinders.clear();
		private_::g_sim = 0;
	}
	void Simulation::tick(const real timeElapsed)
	{
		mCurrTickTime += timeElapsed;
		while (mCurrTickTime > 0)
		{
			mMsgMgr.execute();

			ConstVectorIterator<ObjectList> itEntity( mObjs );
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
	Object* Simulation::createObject(const String& clsName,
												const StringPairVector& components,
												const StringVector& scriptFiles )
	{
		std::pair<object::ResultCode,ClassId> clsId = mObjMgr.getClassId( clsName );
		YAKE_ASSERT( clsId.first == object::RC_OK );
		if (clsId.first != object::RC_OK)
			return 0;
		return createObject( clsId.second, components, scriptFiles );
	}
	Object* Simulation::createObject(const ClassId clsId,
												const StringPairVector& components,
												const StringVector& scriptFiles)
	{
		Object* pObject = mObjMgr.createObject( clsId );
		if (!pObject)
			return 0;
		Entity* pEntity = Entity::cast(pObject);

		if (!pEntity)
			mObjs.push_back( pObject );
		else
		{

			// create VMs & load & execute entity scripts
			const size_t numVMs = (scriptFiles.size() > 0) ? scriptFiles.size() : 1; // at least one VM
			ParamList params;
			params["object"] = pObject;
			params["entity"] = pEntity;
			if (mpScripting)
			{
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
						AutoPtr<scripting::IScript> pScript( mpScripting->createScriptFromFile(scriptFile) );
						YAKE_ASSERT( pScript.get() );
						pVM->execute( pScript.get() );
					} // for each script
				} // for each VM
			} // if mpScripting

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
			mObjs.push_back( pObject );

			// fire "entity spawned" for each VM

			params.clear();
			params["object"] = pObject;
			params["entity"] = pEntity;
			for (size_t i=0; i<pEntity->getVMCount(); ++i)
			{
				params["vm"] = pEntity->getVM(i);
				mEvtEntityVMSpawned.fire(params);
			}

			// spawn entity itself

			pEntity->spawn();

			ObjectMessage* pMsg = pEntity->createMessage( MSGID_EntitySpawned );
			pMsg->set("entity", pEntity );
			mMsgMgr.postMessage( pMsg );
		}
		return pObject;
	}
	void Simulation::addEntityVMBinder( scripting::IBinder* pBinder, const bool bTransferOwnership )
	{
		YAKE_ASSERT( pBinder );
		if (!pBinder)
			return;
		if (bTransferOwnership)
			mBinders.push_back( BinderSharedPtr( pBinder ) );
		else
			mNonOwnedBinders.push_back( pBinder );
	}
	scripting::IVM* Simulation::createEntityVM()
	{
		YAKE_ASSERT( mpScripting );
		if (!mpScripting)
			return 0;
		scripting::IVM* pVM = mpScripting->createVM();
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
	void Simulation::destroyObject( Object* obj )
	{
		if (!obj)
			return;
		// remove from list
		ObjectList::iterator itFind = std::find( mObjs.begin(), mObjs.end(), obj );
		if (itFind != mObjs.end())
			mObjs.erase( itFind );
		// destroy
		mObjMgr.destroyObject( obj );
	}
	void Simulation::removeAndDestroyObjects()
	{
		ConstDequeIterator<ObjectList> itObj( mObjs );
		while (itObj.hasMoreElements())
		{
			Object* obj = itObj.getNext();
			mObjMgr.destroyObject(obj);
		}
		mObjs.clear();
	}
	void Simulation::postMessage(ObjectMessage* pMessage, Object* target)
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
	void Simulation::setTicksPerSecond(const real ticksPerSecond)
	{
		YAKE_ASSERT( ticksPerSecond > 0. );
		if (ticksPerSecond > 0.)
			mTickTime = 1. / ticksPerSecond;
	}
	Event& Simulation::getEvent_onEntityVMSpawned()
	{
		return mEvtEntityVMSpawned;
	}
	Event& Simulation::getEvent_onEntityVMCreated()
	{
		return mEvtEntityVMCreated;
	}
	simtime Simulation::getTime() const
	{
		return mSimTime;
	}
	real Simulation::getTimeAsSeconds() const
	{
		return mSimTimeInSecs;
	}
} // namespace yake
} // namespace ent
