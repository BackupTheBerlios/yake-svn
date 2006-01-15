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
#include <yapp/plugins/entLua/pch.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <yapp/plugins/entLua/yakeLuaBinder.h>
#include <yapp/plugins/entLua/yakeEvent_lua.h>
#include <yapp/plugins/entLua/yakeEntLua.h>
#include <yapp/plugins/entLua/yakeStateCb_lua.h>
#include <luabind/luabind.hpp>

namespace yake {
namespace ent {
namespace lua {

	struct EntityVMSpawnedCb : public ent::EventCallback
	{
		EntityVMSpawnedCb () {}
		virtual void onFire(const ent::EventData& evtData)
		{
			scripting::IVM* pVM = 0;
			try {
				pVM = boost::any_cast<scripting::IVM*>( evtData.params.find("vm")->second );
			} catch (...)
			{
				std::cout << "myLuaScripting::EntitySpawnedCb::onFire() ERROR\n";
				std::cout << "   Could not extract VM!\n";
			}
			YAKE_ASSERT( pVM );

			ent::Entity* pEntity = 0;
			try {
				pEntity = boost::any_cast<ent::Entity*>( evtData.params.find("entity")->second );
			} catch (...)
			{
				std::cout << "myLuaScripting::EntitySpawnedCb::onFire() ERROR\n";
				std::cout << "   Could not extract entity!\n";
			}
			YAKE_ASSERT( pEntity );
			if (pVM && pEntity)
			{
				scripting::LuaVM* pLuaVM = static_cast<scripting::LuaVM*>(pVM);
				YAKE_ASSERT( pLuaVM );

				// basic built-in events
				pEntity->getEventByName("onSpawn")->addCallback( new LuaEntityEventCallback(pLuaVM) );
				pEntity->getEventByName("onTick")->addCallback( new LuaEntityEventCallback(pLuaVM) );
			}
		}
	};
	ent::EventCallback* createEntityVMSpawnedCb()
	{
		return new EntityVMSpawnedCb();
	}
	struct EntityVMCreatedCb : public ent::EventCallback
	{
		EntityVMCreatedCb() {}
		virtual void onFire(const ent::EventData& evtData)
		{
			scripting::IVM* pVM = 0;
			try {
				pVM = boost::any_cast<scripting::IVM*>( evtData.params.find("vm")->second );
			} catch (...)
			{
				std::cout << "myLuaScripting::EntityVMCreatedCb::onFire() ERROR\n";
				std::cout << "   Could not extract VM!\n";
			}
			YAKE_ASSERT( pVM );

			ent::Entity* pEntity = 0;
			try {
				pEntity = boost::any_cast<ent::Entity*>( evtData.params.find("entity")->second );
			} catch (...)
			{
				std::cout << "myLuaScripting::EntityVMCreatedCb::onFire() ERROR\n";
				std::cout << "   Could not extract entity!\n";
			}
			YAKE_ASSERT( pEntity );
			if (pVM && pEntity)
			{
				scripting::LuaVM* pLuaVM = static_cast<scripting::LuaVM*>(pVM);
				lua_State* L = pLuaVM->getLuaState();
				YAKE_ASSERT( L );

				luabind::object globals = luabind::globals(L);

				//
				//globals["theVM"] = pVM;

				// create basic tables
				if ( luabind::type( globals["events"] ) != LUA_TTABLE)
					globals["events"] = luabind::newtable(L);

				luabind::object eventsTbl = globals["events"];
				if ( luabind::type( eventsTbl["client"] ) != LUA_TTABLE)
					eventsTbl["client"] = luabind::newtable(L);
				if ( luabind::type( eventsTbl["server"] ) != LUA_TTABLE)
					eventsTbl["server"] = luabind::newtable(L);

				if ( luabind::type( globals["states"] ) != LUA_TTABLE)
					globals["states"] = luabind::newtable(L);
				luabind::object statesTbl = globals["states"];

				// bind "theEntity" object
				YAKE_ASSERT( pEntity );
				globals["theEntity_asEntity"] = pEntity;
				/*OR:
				if (pEntity->isA() == Light::getClass())
					luabind::get_globals(L)["theEntity"] = Light::cast(pEntity);
				else
					luabind::get_globals(L)["theEntity"] = pEntity;
				*/

				//@todo move up the class hierarchy and look for an existing bind.
				//		last fallback is getAs_entity(), obviously.
				//If we could use the approach above, then the following would be superfluous:
				bool done = false;
				object_class* theClass = pEntity->isA();
				YAKE_LOG_INFORMATION("Looking for a Lua meta class converter for class: " + theClass->getName());
				while (!done)
				{
					if ( luabind::type( globals["getAs_"+theClass->getName()] ) == LUA_TFUNCTION)
					{
						done = true;
					}
					else
					{
						if (theClass != Entity::getClass())
							theClass = theClass->getParents().front();
						else
						{
							done = true;
						}
					}
				}
				YAKE_LOG_INFORMATION("=> Using class: " + theClass->getName());
				const String cmd = "theEntity=getAs_" + theClass->getName() + "(theEntity_asEntity)";
				pVM->execute(cmd);
				YAKE_LOG_INFORMATION("=> Entity VM set up.");
			}
		}
	};
	ent::EventCallback* createEntityVMCreatedCb()
	{
		return new EntityVMCreatedCb();
	}

	bool createEvent(ent::Object* pObject, const String& name)
	{
		YAKE_ASSERT( pObject );
		YAKE_ASSERT( !name.empty() );
		if (!pObject || name.empty())
			return false;

		ent::Event* pEvent = new ent::Event(name);
		pObject->addEvent(pEvent);

		if (ent::Entity* pEntity = Entity::cast(pObject))
		{
			scripting::IVM* pVM = pEntity->getVM(0);
			YAKE_ASSERT( pVM );
			if (!pVM)
				return false;
			pEvent->addCallback( new LuaEntityEventCallback( static_cast<scripting::LuaVM*>(pVM) ) );
		}

		return true;
	}
	bool fireEvent(ent::Object* pObject, const String& name)
	{
		YAKE_ASSERT( pObject );
		YAKE_ASSERT( !name.empty() );
		if (!pObject || name.empty())
			return false;

		return pObject->fireEvent(name);
	}

	template<class ConcreteClass>
	ConcreteClass* getCasted(ent::Object* pObject)
	{
		ConcreteClass* ret = 0;
		try {
			ret = dynamic_cast<ConcreteClass*>(pObject);
		} catch ( std::exception& e )
		{
			std::cout << "EXCEPTION: getCasted() failed for casting a " << pObject->isA()->getName().c_str();
			std::cout << "  to a " << ConcreteClass::getClass()->getName().c_str() << ".\n";
			std::cout << "Cause: " << e.what() << "\n";
		}
		return  ret;
	}

	real randReal(const real lo, const real hi)
	{
		static yake::math::RandomNumberGeneratorMT g_rand;
		return lo + g_rand.randReal() * (hi - lo);
	}

	void graphical_setPosition(GraphicalModel* pModel, const real x, const real y, const real z)
	{
		if (!pModel)
			return;
		const Vector3 pos(x,y,z);
		GraphicalModel::SceneNodeList nodes = pModel->getSceneNodes();
		ConstVectorIterator<GraphicalModel::SceneNodeList> it(nodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext();
			YAKE_ASSERT( pSN ).warning("pSN is 0. shouldn't be.");
			if (!pSN)
				continue;
			pSN->setPosition( pos );
		}
	}
	void pawn_loadGraphical(Pawn* pPawn, const String& fn)
	{
		if (!pPawn)
			return;
		EntityComponent* pVisual = pPawn->getComponent("visual");
		YAKE_ASSERT( pVisual ).warning("Could not retrieve visual entity component!");
		if (!pVisual)
			return;
		GraphicalModel* pModel = new GraphicalModel();
		YAKE_ASSERT( pModel );
		if (!pModel)
			return;
		static_cast<PawnVisual*>(pVisual)->setGraphical( pModel );
		pModel->fromDotScene( fn, Simulation::getSim().getGraphicsWorld() );
	}
	void entity_machineChangeTo(Entity* pEntity, const String& machine, const String& targetState)
	{
		YAKE_ASSERT( pEntity ).warning("null entity");
		if (!pEntity)
			return;
		ObjectMessage* pMsg = pEntity->createMessage(CMDID_MachineChangeTo);
		YAKE_ASSERT( pMsg ).warning("could not create message!");
		if (!pMsg)
			return;
		pMsg->set("machine",machine);
		pMsg->set("to",targetState);
		Simulation::getSim().postMessage( pMsg, pEntity );
	}
	void entity_machineChangeTo_default(Entity* pEntity, const String& targetState)
	{
		entity_machineChangeTo(pEntity,"default",targetState);
	}

	void entity_machineAddState(Entity* pEntity, const String& machine, const String& state)
	{
		YAKE_ASSERT( pEntity ).warning("null entity");
		if (!pEntity)
			return;
		EntityMachine* pMachine = (machine == "default") ? pEntity->getDefaultMachine() : pEntity->getMachine(state);
		YAKE_ASSERT( pMachine )(machine)(state).warning("machine not found");
		if (!pMachine)
			return;
		EntityMachineState* pState = new EntityMachineState();
		pMachine->addState(state,pState);
		pState->addCallback(
			new EntityMachineStateCallbackLua(
				state,pEntity,machine,static_cast<scripting::LuaVM*>(pEntity->getVM(0))
				)
			);
	}
	void entity_machineAddState_default(Entity* pEntity, const String& state)
	{
		entity_machineAddState(pEntity, "default", state);
	}
	String entity_machineGetState_default(Entity* pEntity)
	{
		YAKE_ASSERT( pEntity ).warning("null entity");
		if (!pEntity)
			return String();
		EntityMachine* pMachine = pEntity->getDefaultMachine();
		YAKE_ASSERT( pMachine ).warning("machine not found");
		if (!pMachine)
			return String();
		return pMachine->getCurrentStateId();
	}

	YAKE_REGISTER_CONCRETE( Binder );
			
	void Binder::bind( scripting::IVM* pVM )
	{
		scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
		YAKE_ASSERT( pL );
		if (!pL)
			return;

		using namespace luabind;
		using namespace ::yake::ent;

		module( pL->getLuaState() )
		[
			class_<ObjectId>("ObjectId")
				.def("classId", &ObjectId::classId)
				.def("serialNo", &ObjectId::serialNo)
		];
		module( pL->getLuaState() )
		[
			class_<Simulation>("Simulation")
				.def("getTime", &Simulation::getTimeAsSeconds)
		];
		module( pL->getLuaState() )
		[
			class_<EntityMachine>("EntityMachine")
				.def("getCurrentState", &EntityMachine::getCurrentStateId)
		];
		module( pL->getLuaState() )
		[
			class_<Event>("Event")
				.def("getName", &Event::getName)
		];
		module( pL->getLuaState() )
		[
			def("getAs_Object", &getCasted<Object>),
			class_<Object>("Object")
				.def("getId", &Object::getId)
				.def("getEventByName", &Object::getEventByName)
				.def("addEvent", &Object::addEvent)
				.def("createEvent",&createEvent)
				.def("fireEvent",&fireEvent)
		];
		module( pL->getLuaState() )
		[
			def("getAs_Entity", &getCasted<Entity>),
			class_<Entity,Object>("Entity")
				.def("getAge", &Entity::getAge)
				.def("addState", &entity_machineAddState_default)
				.def("changeStateTo", &entity_machineChangeTo_default)
				.def("getState", &entity_machineGetState_default)
				.def("getMachine", &Entity::getDefaultMachine)
				.def("getMachineByName", &Entity::getMachine)
		];
		module( pL->getLuaState() )
		[
			def("getAs_Light", &getCasted<Light>),
			class_<Light,Entity>("Light")
				.def("cast", (Light*(*)(Object*))&Light::cast)
				.def("cast", (const Light*(*)(const Object*))&Light::cast)
				.def("enable", &Light::enableLight)
				.def("isEnabled", &Light::isLightEnabled)
		];
		module( pL->getLuaState() )
		[
			class_<GraphicalModel>("GraphicalModel")
				.def("setPosition", &graphical_setPosition)
		];
		module( pL->getLuaState() )
		[
			def("getAs_Pawn", &getCasted<Pawn>),
			class_<Pawn,Entity>("Pawn")
				//.def("setGraphical", &pawn_setGraphical)
				//.def("getGraphical", &pawn_getGraphical)
				.def("createGraphical", &pawn_loadGraphical)
		];
		module( pL->getLuaState() )
		[
			def("getSim",&Simulation::getSim),
			def("createEvent",&createEvent),
			def("fireEvent",&fireEvent),
			def("rand",&randReal)
		];
	}

	YAKE_REGISTER_CONCRETE( MathBinder );
	
	void MathBinder::bind( scripting::IVM* pVM )
	{
		scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
		YAKE_ASSERT( pL );
		if (!pL)
			return;

		using namespace luabind;
		using namespace ::yake::math;

		module( pL->getLuaState() )
		[
			class_<Vector3>("Vector3")
				.def(constructor<>())
				.def(constructor<real,real,real>())
				.def_readwrite("x", &Vector3::x)
				.def_readwrite("y", &Vector3::y)
				.def_readwrite("z", &Vector3::z)
		];
	}

	//
	bool bindSim( Simulation& theSim )
	{
		//Binder* pBinder = create<scripting::IBinder>("yake::ent::luabinder");
		Binder* pBinder = new Binder();
		// add Lua binder
		YAKE_ASSERT( pBinder );
		if (!pBinder)
			return false;
		theSim.addEntityVMBinder( pBinder, false );
		//mSim.addEntityVMBinder( new yake::ent::lua::MathBinder() );

		// add special callback (no direct deps on lua via interface!)
		theSim.getEvent_onEntityVMSpawned().addCallback( ent::lua::createEntityVMSpawnedCb() );
		theSim.getEvent_onEntityVMCreated().addCallback( ent::lua::createEntityVMCreatedCb() );
		return true;
	}

} // namespace lua
} // namespace ent
} // namespace yake
