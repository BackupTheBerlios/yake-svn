//****************************************************************************
//**
//**    Sample - EXPERIMENTAL CODE
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yapp/samples/base/statemachine/yakePCH.h>
#include <yapp/base/yapp.h>

using namespace yake;
using namespace app;

//---------------------------------------------------------
// Helper defines
#define CHECK_EC_OK( X ) \
{ \
	ErrorCode ec = X; \
	if (ec != eOk) \
	{ \
		COUTLN("AOK FAILED! (" << #X << ")"); \
		COUTLN("  FILE: " << __FILE__ << " @ " << __LINE__ ); \
		COUTLN("  " << ec); \
	} \
}
#define COUTLN( X ) \
	std::cout << X << std::endl;

//---------------------------------------------------------
// Types / Classes

typedef state::StateMachine<String> EntityStateMachine;

//---------------------------------------------------------
// Sample entity base class providing a state machine
class Entity
{
public:
	EntityStateMachine& getCharacterMachine();
private:
	EntityStateMachine	mCharacterMachine;
};
EntityStateMachine& Entity::getCharacterMachine()
{
	return mCharacterMachine;
}

//---------------------------------------------------------
// Sample derived entity class making use of a state machine
class Hero : /*protected*/public Entity
{
public:
	// events:

	void event_birth()
	{
		CHECK_EC_OK( getCharacterMachine().changeStateTo( "alive" ) );
	}
	void event_walk()
	{
		CHECK_EC_OK( getCharacterMachine().changeStateTo( "walking" ) );
	}
	void event_die()
	{
		CHECK_EC_OK( getCharacterMachine().changeStateTo( "dead" ) );
		CHECK_EC_OK( getCharacterMachine().changeStateTo( "afterlife" ) );
	}

	// states: (some of them)
	void stateAlive()
	{
		COUTLN( "  STATE: stateAlive() (member function)" );
	}

	// behaviours:
};

void stateDead()
{
	COUTLN( "  STATE: stateDead()" );
}
void stateAfterLife()
{
	COUTLN( "  STATE: stateAfterLife()" );
}
void transToAfterLife()
{
	COUTLN( "  TRANS: transToAfterLife()" );
}
void transAliveReset()
{
	COUTLN( "  TRANS: transAliveReset()" );
}
void transExitAlive()
{
	COUTLN( "  TRANS: exitAlive()" );
}
void transFlashLife()
{
	COUTLN( "  TRANS: transFlashLifeBeforeInnerEyes()" );
}
void transDying()
{
	COUTLN( "  TRANS: dying()" );
}

template<typename StateIdType>
std::ostream& operator << (std::ostream& lhs, const state::StateMachine<StateIdType> & m)
{
	m.dump( lhs );
	return lhs;
}

void test_fsm()
{
	Hero theHero;

	// register states
	theHero.getCharacterMachine().regState( "alive", boost::bind( &Hero::stateAlive, &theHero ) );
	theHero.getCharacterMachine().regState( "dead", boost::bind( &stateDead ) );
	theHero.getCharacterMachine().regState( "afterlife", boost::bind( &stateAfterLife ) );

	// register transitions

	// transition kStateNone -> "alive" (entry transition)
	theHero.getCharacterMachine().regTransition( EntityStateMachine::kStateNone, "alive" );

	// transition "alive" -> "dead"
	EntityStateMachine::TransitionFnList fns;
	fns.push_back( boost::bind(&transFlashLife) );
	fns.push_back( boost::bind(&transExitAlive) );
	fns.push_back( boost::bind(&transDying) );
	theHero.getCharacterMachine().regTransition( "alive", "dead", fns );
	fns.clear();

	// transition "dead" -> "afterlife"
	theHero.getCharacterMachine().regTransition( "dead", "afterlife", boost::bind(&transToAfterLife) );

	// transition "alive" -> "alive"
	theHero.getCharacterMachine().regTransition( "alive", "alive", boost::bind(&transAliveReset) );

	// debug dump
	std::cout << theHero.getCharacterMachine();

	// go
	COUTLN( std::endl << "Running..." << std::endl );

	// States can be changed like this:
	//    theHero.getCharacterMachine().changeStateTo( "alive" );
	// Here we use events which will initiate the state changes as appropriattheHero.
	// As we don't yet use yake/rx events we simulate firing them like this:
	//    theHero.event_NAME();   // fire event NAME

	COUTLN( std::endl << "event: birth! (transition from kStateNone to 'alive')" );
	theHero.event_birth(); // has to be the first event! only kStateNone -> "alive" is a registered initial transition!

	COUTLN( std::endl << "event: birth! (transition from 'alive' to 'alive' again (really!))" );
	theHero.event_birth();

	COUTLN( std::endl << "event: die! (transition from 'alive' to 'dead' and then on to 'afterlife')" );
	theHero.event_die();
}

int main()
{
	try
	{
		std::cout << std::endl << "Demo provided for YAKE by Stephan Kaiser" << std::endl << std::endl;
		test_fsm();
	}
	catch (const yake::base::Exception & e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
