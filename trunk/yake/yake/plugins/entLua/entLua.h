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
