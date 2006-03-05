//#include "yake/base/yakePrerequisites.h"
#include "yake/base/yake.h"
#include "yake/ent/ent.h"
#include "yake/plugins//entLua/entLua.h"

using namespace yake;

namespace exapp {
	// Commonly used states for this application's objects.
	const std::string ksAwakening = "awakening";
	const std::string ksAlive = "alive";
	const std::string ksDead = "dead";
	
	/** Listens to object manager and initializes newly created objects by adding
		a Lua VM, adding default states and transitions to the state machine etc.
	*/
	struct ExampleAppObjectMgrListener : public object::ObjectManagerListener
	{
		ExampleAppObjectMgrListener(scripting::IScriptingSystem& scriptingSys) : scriptingSys_(scriptingSys)
		{}
		virtual void onObjectCreated(object::Object* obj)
		{
			scripting::IVM* vm = scriptingSys_.createVM();
			YAKE_ASSERT( vm );

			obj->attachVM(vm,"main");
		}
		virtual void onDestroyObject(object::Object* obj)
		{
			scripting::IVM* vm = obj->detachVM("main");
			YAKE_ASSERT( vm );
			delete vm;
		}
		virtual void onObjectInitialized(object::Object* obj)
		{
			obj->addFsmState(ksAwakening);
			obj->addFsmState(ksAlive);
			obj->addFsmState(ksDead);

			obj->addFsmTransition(ksAwakening,"spawn",ksAlive);
			obj->addFsmTransition(ksAlive,"die",ksDead);

			obj->setFsmState(ksAwakening);
		}
	private:
		scripting::IScriptingSystem&	scriptingSys_;
	};
} // namespace exapp

int main(int argc, char* argv[])
{

	// Create the scripting system we want to use (Lua).
	SharedPtr<scripting::IScriptingSystem> luaSys = templates::create<scripting::IScriptingSystem>("lua");
	YAKE_ASSERT( luaSys.get() );

	// Create an object manager which manages creation, initialization and destruction of objects.
	object::ObjectManager objMgr;

	// Create & attach the listener which handles the connection to the Lua VM and the bindings.
	object::LuaObjectManagerListener luaFmsObjListener(*luaSys);
	objMgr.attachListener( &luaFmsObjListener, "luaFsm", object::ObjectManager::kKeepOwnership );

	// Example listener which sets the objects up for our example application :)
	// The listener adds default states and transitions to the object's state machine, for example.
	SharedPtr<exapp::ExampleAppObjectMgrListener> exappObjInitializer( new exapp::ExampleAppObjectMgrListener(*luaSys) );
	objMgr.attachListener( exappObjInitializer.get(), "exapp_listener", object::ObjectManager::kKeepOwnership );

	// Let's create an object.
	object::Object* o = objMgr.makeObject();
	YAKE_ASSERT( o );

	// NB At this point the FSM is set up by the exapp listener!

	// Do some processing:
	o->processFsmEvent("spawn");
	objMgr.tick(); // triggers o->tick()
	objMgr.tick();
	o->processFsmEvent("die");

	//@todo use scheduler for yield/resume functionality. (code in prototype is in fsm2_lua.cpp)
	//o->getVM().resume();

	exappObjInitializer.reset();
	luaSys.reset();
}

