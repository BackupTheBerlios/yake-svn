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

	struct SimpleGraphicsCo : public ent::Component
	{
		DECL_CO(SimpleGraphicsCo,"SimpleGraphicsCo")
	};
	IMPL_CO_1(SimpleGraphicsCo,"Component")
} // namespace exapp
#ifdef OOSTATE
struct oostate 
{
	virtual ~oostate() {}
	virtual void onEnter() {}
	virtual void onExit() {}
};
template<typename _state_class_type /*= oostate */, typename _event_type>
struct oomachine
{
	typedef _event_type event_type;
	typedef _state_class_type* state_type;

	oomachine() :
		fnEnter_( boost::bind(&oomachine::onEnter,this,_1,_2) ),
		fnExit_( boost::bind(&oomachine::onExit,this,_1,_2) )
	{
	}

	void addState(const state_type);
	void addTransition(const state_type, const event_type&, const state_type);
	void setState(const state_type);
	void processEvent(const event_type& evt);
	const state_type& getCurrentFsmState() const
	{ return m_.current(); }
private:
	typedef fsm::machine<state_type,event_type> machine_type;
	void onEnter(machine_type&,const state_type&);
	void onExit(machine_type&,const state_type&);
private:
	machine_type			machine_;
	typedef boost::function<void(machine_type&,const state_type&)> CbFn;
	CbFn						fnEnter_;
	CbFn						fnExit_;
};
template<typename _state_class_type, typename _event_type>
void oomachine<_state_class_type,_event_type>::onEnter(machine_type&,const state_type& state)
{
	YAKE_ASSERT( state );
	state->onEnter();
}
template<typename _state_class_type, typename _event_type>
void oomachine<_state_class_type,_event_type>::onExit(machine_type&,const state_type& state)
{
	YAKE_ASSERT( state );
	state->onExit();
}
template<typename _state_class_type, typename _event_type>
void oomachine<_state_class_type,_event_type>::addState(const state_type s)
{
	machine_.addState(s);
}
template<typename _state_class_type, typename _event_type>
void oomachine<_state_class_type,_event_type>::addTransition(const state_type from, const event_type& ev, const state_type to)
{
	machine_.addTransition( from, ev, to );
}
template<typename _state_class_type, typename _event_type>
void oomachine<_state_class_type,_event_type>::setState(const state_type s)
{
	machine_.setState( s );
}
template<typename _state_class_type, typename _event_type>
void oomachine<_state_class_type,_event_type>::processEvent(const event_type& evt)
{
	YAKE_ASSERT( !fnEnter_.empty() );
	machine_.processEventCb(evt, fnEnter_, fnExit_);
}
struct test_state_base
{
	virtual ~test_state_base() {}
	virtual void onEnter() {}
	virtual void onExit() {}
};
typedef test_state_base* test_state_base_ptr;
namespace fsm {
	template<>
	inline const test_state_base_ptr& get_null_state()
	{ 
		static test_state_base* ms_null = 0;
		return ms_null; 
	}
}
struct test_state : public test_state_base
{
	test_state(const std::string& name) : name_(name)
	{}
	virtual void onEnter()
	{
		std::cout << name_ << ".onEnter()\n";
	}
	virtual void onExit()
	{
		std::cout << name_ << ".onExit()\n";
	}
private:
	std::string	name_;
};
void test()
{
	typedef oomachine<test_state_base,std::string> machine_t;
	test_state_base* first = new test_state("first");
	test_state_base* second = new test_state("second");

	machine_t machine;
	machine.addState( first );
	machine.addState( second );
	machine.setState( first );
	machine.addTransition( first, "go", second );
	machine.processEvent("go");
}
#endif
int main(int argc, char* argv[])
{
	//test();

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
	clsMgr.insert( ent::Component::cls() );
	exapp::SimpleGraphicsCo::cls()->add( PropDef("position","position",Vector3(0,0,0)) );
	clsMgr.insert( exapp::SimpleGraphicsCo::cls() );

	e->addComponent("gfx",new exapp::SimpleGraphicsCo());
	e->getComponent("gfx")->init();
	if (!e->getComponent("gfx")->setValue("position",Vector3(10,0,0)))
		std::cerr << "could not set position\n";
	Vector3 pos(0,0,0);
	if (e->getComponent("gfx")->getValue<Vector3>("position",pos))
		std::cout << "position=" << pos << "\n";
	else
		std::cerr << "could not get position\n";
	*/
	// clean up

	exappObjInitializer.reset();
	luaSys.reset();
}

