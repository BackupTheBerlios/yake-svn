//#include "yake/base/yakePrerequisites.h"
#include "yake/base/yake.h"
#include "yake/ent/ent.h"
#include "yake/plugins/entLua/entLua.h"

using namespace yake;

namespace exapp {
	// Commonly used states for this application's objects.
	const std::string ksAwakening = "awakening";
	const std::string ksAlive = "alive";
	const std::string ksDead = "dead";
	
	/** Listens to object manager and initializes newly created objects by adding
		a Lua VM, adding default states and transitions to the state machine etc.
	*/
	struct ExampleAppObjectMgrListener : public ent::ObjectManagerListener
	{
		ExampleAppObjectMgrListener(scripting::IScriptingSystem& scriptingSys) : scriptingSys_(scriptingSys)
		{}
		virtual void onObjectCreated(ent::Object* obj)
		{
			scripting::IVM* vm = scriptingSys_.createVM();
			YAKE_ASSERT( vm );

			ent::Entity* ent = ent::Entity::cast(obj);
			YAKE_ASSERT( ent );
			ent->attachVM(vm,"main");
		}
		virtual void onDestroyObject(ent::Object* obj)
		{
			ent::Entity* ent = ent::Entity::cast(obj);
			YAKE_ASSERT( ent );

			scripting::IVM* vm = ent->detachVM("main");
			YAKE_ASSERT( vm );
			delete vm;
		}
		virtual void onObjectInitialized(ent::Object* obj)
		{
			ent::Entity* ent = ent::Entity::cast(obj);
			YAKE_ASSERT( ent );

			// Add default FSM states and transitions:

			ent->addFsmState(ksAwakening);
			ent->addFsmState(ksAlive);
			ent->addFsmState(ksDead);

			ent->addFsmTransition(ksAwakening,"spawn",ksAlive);
			ent->addFsmTransition(ksAlive,"die",ksDead);

			// Set initial state:

			ent->setFsmState(ksAwakening);
		}
	private:
		scripting::IScriptingSystem&	scriptingSys_;
	};

	struct SimpleGraphicsCo : public ent::Co
	{
		DECL_CO(SimpleGraphicsCo,"SimpleGraphicsCo")
	};
	IMPL_CO_1(SimpleGraphicsCo,"Co")
} // namespace exapp

int main(int argc, char* argv[])
{

	// Create the scripting system we want to use (Lua).
	SharedPtr<scripting::IScriptingSystem> luaSys = templates::create<scripting::IScriptingSystem>("lua");
	YAKE_ASSERT( luaSys.get() );

	// Create an object manager which manages creation, initialization and destruction of objects.
	ent::ObjectManager objMgr;

	// Create & attach the listener which handles the connection to the Lua VM and the bindings.
	ent::LuaObjectManagerListener luaFmsObjListener(*luaSys);
	objMgr.attachListener( &luaFmsObjListener, "luaFsm", ent::ObjectManager::kKeepOwnership );

	// Example listener which sets the objects up for our example application :)
	// The listener adds default states and transitions to the object's state machine, for example.
	SharedPtr<exapp::ExampleAppObjectMgrListener> exappObjInitializer( new exapp::ExampleAppObjectMgrListener(*luaSys) );
	objMgr.attachListener( exappObjInitializer.get(), "exapp_listener", ent::ObjectManager::kKeepOwnership );

	// Register object class
	ent::RegistrationResult ret = objMgr.registerClass<ent::Entity>("Entity");
	YAKE_ASSERT( ret.first == object::RC_OK );

	// Let's create an object.
	//ent::Object* o = objMgr.makeObject(ret.second); // create by ClassId
	ent::Object* o = objMgr.makeObject("Entity"); // create by class name
	YAKE_ASSERT( o );
	ent::Entity* e = ent::Entity::cast(o); // cast to Entity*
	YAKE_ASSERT( e );

	// NB At this point the FSM has been set up by the exapp listener!

	// Do some FSM event processing:
	e->processFsmEvent("spawn");
	objMgr.tick(); // triggers o->tick()
	objMgr.tick();
	e->processFsmEvent("die");

	//@todo use scheduler for yield/resume functionality. (code in prototype is in fsm2_lua.cpp)
	//e->getVM().resume();

	// Let's have some fun with components
	/*
	RttiClassMgr clsMgr;
	clsMgr.insert( ent::Object::cls() );
	clsMgr.insert( ent::Co::cls() );
	exapp::SimpleGraphicsCo::cls()->add( PropDef("position","position",Vector3(0,0,0)) );
	clsMgr.insert( exapp::SimpleGraphicsCo::cls() );

	e->addCo("gfx",new exapp::SimpleGraphicsCo());
	e->getCo("gfx")->init();
	if (!e->getCo("gfx")->setValue("position",Vector3(10,0,0)))
		std::cerr << "could not set position\n";
	Vector3 pos(0,0,0);
	if (e->getCo("gfx")->getValue<Vector3>("position",pos))
		std::cout << "position=" << pos << "\n";
	else
		std::cerr << "could not get position\n";
	*/
	// clean up

	exappObjInitializer.reset();
	luaSys.reset();
}

