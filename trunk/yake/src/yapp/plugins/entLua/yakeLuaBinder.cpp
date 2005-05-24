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

namespace yake {
namespace ent {
namespace lua {

	struct EntitySpawnedCb : public ent::EventCallback
	{
		EntitySpawnedCb() {}
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

			ent::entity* pEntity = 0;
			try {
				pEntity = boost::any_cast<ent::entity*>( evtData.params.find("entity")->second );
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
	ent::EventCallback* createEntitySpawnedCb()
	{
		return new EntitySpawnedCb();
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

			ent::entity* pEntity = 0;
			try {
				pEntity = boost::any_cast<ent::entity*>( evtData.params.find("entity")->second );
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

				luabind::object globals = luabind::get_globals(L);

				// create basic tables
				if (globals["events"].type() != LUA_TTABLE)
					globals["events"] = luabind::newtable(L);

				luabind::object eventsTbl = globals["events"];
				if (eventsTbl["client"].type() != LUA_TTABLE)
					eventsTbl["client"] = luabind::newtable(L);
				if (eventsTbl["server"].type() != LUA_TTABLE)
					eventsTbl["server"] = luabind::newtable(L);

				if (globals["states"].type() != LUA_TTABLE)
					globals["states"] = luabind::newtable(L);
				luabind::object statesTbl = globals["states"];

				// bind "theEntity" object
				YAKE_ASSERT( pEntity );
				globals["theEntity_asEntity"] = pEntity;
				/*OR:
				if (pEntity->isA() == light::getClass())
					luabind::get_globals(L)["theEntity"] = light::cast(pEntity);
				else
					luabind::get_globals(L)["theEntity"] = pEntity;
				*/

				//@todo move up the class hierarchy and look for an existing bind.
				//		last fallback is getAs_entity(), obviously.
				//If we could use the approach above, then the following would be superfluous:
				const String cmd = "theEntity=getAs_" + pEntity->isA()->getName() + "(theEntity_asEntity)";
				pVM->execute(cmd);
			}
		}
	};
	ent::EventCallback* createEntityVMCreatedCb()
	{
		return new EntityVMCreatedCb();
	}

	bool createEvent(ent::entity* pEntity, const String& name)
	{
		YAKE_ASSERT( pEntity );
		YAKE_ASSERT( !name.empty() );
		if (!pEntity || name.empty())
			return false;
		YAKE_ASSERT( pEntity->getVM() );
		if (!pEntity->getVM())
			return false;

		ent::Event* pEvent = new ent::Event(name);
		pEntity->addEvent(pEvent);
		pEvent->addCallback( new LuaEntityEventCallback( static_cast<scripting::LuaVM*>(pEntity->getVM()) ) );

		return true;
	}
	bool fireEvent(ent::entity* pEntity, const String& name)
	{
		YAKE_ASSERT( pEntity );
		YAKE_ASSERT( !name.empty() );
		if (!pEntity || name.empty())
			return false;

		return pEntity->fireEvent(name);
	}

	template<class ConcreteEntityClass>
	ConcreteEntityClass* getCasted(ent::entity* pEntity)
	{
		ConcreteEntityClass* ret = 0;
		try {
			ret = dynamic_cast<ConcreteEntityClass*>(pEntity);
		} catch ( std::exception& e )
		{
			std::cout << "EXCEPTION: getCasted() failed for casting a " << pEntity->isA()->getName().c_str();
			std::cout << "  to a " << ConcreteEntityClass::getClass()->getName().c_str() << ".\n";
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
		GraphicalModel::SceneNodeList nodes = pModel->getRootSceneNodes();
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
	void pawn_loadGraphical(pawn* pPawn, const String& fn)
	{
		if (!pPawn)
			return;
		GraphicalModel* pModel = new GraphicalModel();
		YAKE_ASSERT( pModel );
		if (!pModel)
			return;
		pPawn->setGraphical( pModel );
		pModel->fromDotScene( fn, sim::getSim().getGraphicsWorld() );
	}

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
			class_<sim>("sim")
				.def("getTime", &sim::getTimeAsSeconds)
		];
		module( pL->getLuaState() )
		[
			class_<Event>("event")
				.def("getName", &Event::getName)
		];
		module( pL->getLuaState() )
		[
			def("getAs_entity", &getCasted<entity>),
			class_<entity>("entity")
				//.def("cast", &entity::cast)
				.def("getAge", &entity::getAge)
				.def("getId", &entity::getId)
				.def("getEventByName", &entity::getEventByName)
				.def("addEvent", &entity::addEvent)
				.def("createEvent",&createEvent)
				.def("fireEvent",&fireEvent)
		];
		module( pL->getLuaState() )
		[
			def("getAs_light", &getCasted<light>),
			class_<light,entity>("light")
				.def("cast", (light*(*)(entity*))&light::cast)
				.def("cast", (const light*(*)(const entity*))&light::cast)
				.def("enable", &light::enableLight)
				.def("isEnabled", &light::isLightEnabled)
		];
		module( pL->getLuaState() )
		[
			class_<GraphicalModel>("GraphicalModel")
				.def("setPosition", &graphical_setPosition)
		];
		module( pL->getLuaState() )
		[
			def("getAs_pawn", &getCasted<pawn>),
			class_<pawn,entity>("pawn")
				.def("setGraphical", &pawn::setGraphical)
				.def("getGraphical", &pawn::getGraphical)
				.def("createGraphical", &pawn_loadGraphical)
		];
		module( pL->getLuaState() )
		[
			def("getSim",&sim::getSim),
			def("createEvent",&createEvent),
			def("fireEvent",&fireEvent),
			def("rand",&randReal)
		];
	}

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

} // namespace lua
} // namespace ent
} // namespace yake
