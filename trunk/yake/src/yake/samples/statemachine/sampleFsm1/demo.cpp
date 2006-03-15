//#include "yake/base/yakePrerequisites.h"
#include "yake/base/yake.h"
#include "yake/statemachine/fsm_core.h"
#include "yake/statemachine/fsm_oo_machine.h"

struct test_state_base
{
	virtual ~test_state_base() {}
	virtual void onEnter() {}
	virtual void onExit() {}
};
typedef test_state_base* test_state_base_ptr;
//namespace yake {
namespace fsm {
	template<>
	inline const test_state_base_ptr& get_null_state()
	{ 
		static test_state_base* ms_null = 0;
		return ms_null; 
	}
}
//}
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

int main(int argc, char* argv[])
{
	using namespace yake;
	{  // machine demo #1 - 3 states - 2 transitions - 2 events
		typedef fsm::machine<String,String> machine_t;

		const String ksAwakening = "awakening";
		const String ksAlive = "alive";
		const String ksDead = "dead";

		machine_t	m;
		m.addState( ksAwakening );
		m.addState( ksAlive );
		m.addState( ksDead );
		m.addTransition( ksAwakening, "spawn", ksAlive );
		m.addTransition( ksAlive, "die", ksDead );
		m.setState( ksAwakening ); // initial state

		m.processEvent( "spawn" );
		YAKE_LOG_INFORMATION( m.current() );
		YAKE_ASSERT( m.current() == ksAlive );

		m.processEvent( "die" );
		YAKE_LOG_INFORMATION( m.current() );
		YAKE_ASSERT( m.current() == ksDead );
	}
	{  // machine with object states with onEnter() and onExit() callback methods.
		typedef fsm::oomachine<test_state_base,String> machine_t;
		test_state_base* first = new test_state("first");
		test_state_base* second = new test_state("second");

		machine_t machine;
		machine.addState( first );
		machine.addState( second );
		machine.setState( first );
		machine.addTransition( first, "go", second );
		YAKE_ASSERT( machine.current() == first );
		machine.processEvent("go");
		YAKE_ASSERT( machine.current() == second );
	}
}

