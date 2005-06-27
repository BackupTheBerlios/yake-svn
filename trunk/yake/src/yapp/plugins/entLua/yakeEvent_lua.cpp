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
#include <yapp/plugins/entLua/pch.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <yapp/plugins/entLua/yakeLuaBinder.h>
#include <yapp/plugins/entLua/yakeEvent_lua.h>
#include <yapp/plugins/entLua/yakeEntLua.h>
#include <luabind/luabind.hpp>

namespace yake {
namespace ent {
namespace lua {

	bool executeEntityEvent(ent::Entity* pEntity, scripting::IVM* pVM, lua_State* L, const String& evtName)
	{
		try {
			//std::cout << "executeEntityEvent(\"" << evtName.c_str() << "\")\n";
			luabind::object evtTbl = luabind::get_globals(L)["events"];
			if (!evtTbl.is_valid())
				throw "event table is not valid";

			luabind::object fnTbl = pEntity->isServer() ? evtTbl.at("server") : evtTbl.at("client");
			//luabind::object fnTbl = evtTbl["server"];
			if (!fnTbl.is_valid())
				throw "server event functions table is not valid";

			luabind::call_member<void>( fnTbl, evtName.c_str() );
			return true;
		}
		catch (luabind::error& e)
		{
			std::cout << "\nCATCHED Luabind EXCEPTION:\n" << e.what() << "\n";
			return false;
		}
		catch (const char* msg)
		{
			std::cout << "\nCATCHED (...) EXCEPTION:\n" << msg << "\n";
			return false;
		}
	}

	LuaEntityEventCallback::LuaEntityEventCallback(yake::scripting::LuaVM* pVM) :
				mpVM(pVM)
	{}
	void LuaEntityEventCallback::onFire( const ent::EventData& evtData )
	{
		ent::Entity* pEntity = 0;
		try {
			pEntity = boost::any_cast<ent::Entity*>( evtData.params.find("entity")->second );
		} catch (...)
		{
			std::cout << "LuaEntityEventCallback::onFire() ERROR\n";
			std::cout << "   Could not extract entity!\n";
		}
		YAKE_ASSERT( pEntity );
		if (pEntity && mpVM)
			executeEntityEvent( pEntity, mpVM, mpVM->getLuaState(), evtData.evt->getName() );
	}

} // namespace lua
} // namespace ent
} // namespace yake
