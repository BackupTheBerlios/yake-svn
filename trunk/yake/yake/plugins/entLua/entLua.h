/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#ifndef YAKE_ENTLUA_H
#define YAKE_ENTLUA_H

#if defined(YAKE_ENTLUA_EXPORTS)
#	define YAKE_ENTLUA_API DLLEXPORT
#else
#	define YAKE_ENTLUA_API DLLIMPORT
#endif

#include "yake/base/yakePrerequisites.h"
#include "yake/scripting/yakeScriptingSystem.h"
#include "yake/ent/ent.h"

namespace yake {
namespace ent {
	/** @todo Move into private namespace. */
	struct LuaFsmObjectListener : public ObjectListener
	{
		LuaFsmObjectListener(scripting::IScriptingSystem& scriptingSystem);
		virtual void onInit(Object& obj);
		virtual void onTick(Object& obj);
		virtual void onFsmEventHandled(Object& obj, const object_fsm&, const object_fsm::event_type& evt);
		virtual void onFsmEnterState(Object& obj,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type& state);
		virtual void onFsmExitState(Object& obj,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type& state);
	private:
		scripting::IScriptingSystem&				scriptingSystem_;
		boost::shared_ptr<scripting::IScript>	startScript_;
	};

	struct YAKE_ENTLUA_API LuaObjectManagerListener : public ObjectManagerListener
	{
		LuaObjectManagerListener(scripting::IScriptingSystem& scriptingSystem);
		virtual void onObjectCreated(Object* obj);
	private:
		LuaFsmObjectListener	fsmListener_;
	};
} // namespace ent
} // namespace yake

#endif
