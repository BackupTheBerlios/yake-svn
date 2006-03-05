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
#include <yapp/statemachine/yakeFiniteStateMachine.h>

using namespace yake;

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

typedef ::yake::state::Machine<String> EntityStateMachine;

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
	Hero()
	{
	}
	//
	void step()
	{
		CHECK_EC_OK( getCharacterMachine().step() );
	}
	// events:
	void event_birth()
	{
		CHECK_EC_OK( getCharacterMachine().changeTo( "alive" ) );
	}
	void event_walk()
	{
		CHECK_EC_OK( getCharacterMachine().changeTo( "walking" ) );
	}
	void event_die()
	{
		CHECK_EC_OK( getCharacterMachine().changeTo( "dead" ) );
		event_afterLife();
	}
	void event_afterLife()
	{
		std::cout << "(event_afterLife: right before changing to 'afterlife')\n";
		std::cout << getCharacterMachine();
		CHECK_EC_OK( getCharacterMachine().changeTo( "afterlife" ) );
	}

	// states: (some of them)
	class StateAlive : public state::State
	{
	public:
		StateAlive( Hero& rHero ) : mHero( rHero ), mTtl(3)
		{}
		virtual void onEnter()
		{
		}
		virtual void onStep()
		{
			COUTLN( "  STATE: stateAlive() (member function) time before walking = " << mTtl-- );
			if (mTtl < 0)
				mHero.event_die();
		}
	private:
		Hero&	mHero;
		int		mTtl;
	};
	class StateDead : public state::State
	{
	public:
		virtual void onStep()
		{
			COUTLN( "  STATE: stateDead() (member function)" );
		}
	};
	class StateAfterLife : public state::State
	{
	public:
		virtual void onStep()
		{
			COUTLN( "  STATE: stateAfterAlive() (member function)" );
		}
	};

	// behaviours:
	// transition functions:
	void transToAfterLife()
	{
		COUTLN( "  TRANS: Hero::transToAfterLife()" );
	}
};

void stateDead()
{
	COUTLN( "  STATE: stateDead()" );
}
void stateAfterLife()
{
	COUTLN( "  STATE: stateAfterLife()" );
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

void test_fsm()
{
	COUTLN("\nDemonstrating a FSM with sets of free and member functions as transitions:\n");
	Hero theHero;

	// register states
	theHero.getCharacterMachine().addState( "alive", new Hero::StateAlive(theHero) );
	theHero.getCharacterMachine().addState( "dead", new Hero::StateDead() );
	theHero.getCharacterMachine().addState( "afterlife", new Hero::StateAfterLife() );

	// register transitions

	// transition kStateNone -> "alive" (entry transition)
	theHero.getCharacterMachine().addTransition( EntityStateMachine::kStateNone, "alive", new state::Transition() );

	// transition "alive" -> "dead" [ Note: using "free" functions! ]
	theHero.getCharacterMachine().addTransition( "alive", "dead",
		new state::Transition(
							boost::bind(&transFlashLife),
							boost::bind(&transExitAlive),
							boost::bind(&transDying)
						));

	// transition "dead" -> "afterlife" [ Note: using member functions! ]
	theHero.getCharacterMachine().addTransition( "dead", "afterlife", new state::Transition(boost::bind(&Hero::transToAfterLife,&theHero)) );

	// transition "alive" -> "alive"
	theHero.getCharacterMachine().addTransition( "alive", "alive", new state::Transition(boost::bind(&transAliveReset)) );

	// go
	COUTLN( std::endl << "Running..." << std::endl );

	// States can be changed like this:
	//    theHero.getCharacterMachine().changeTo( "alive" );
	// Here we use events which will initiate the state changes as appropriattheHero.
	// As we don't yet use yake/rx events we simulate firing them like this:
	//    theHero.event_NAME();   // fire event NAME

	COUTLN( std::endl << "event: birth! (transition from kStateNone to 'alive')" );
	theHero.event_birth(); // has to be the first event! only kStateNone -> "alive" is a registered initial transition!
	theHero.step();

	std::cout << theHero.getCharacterMachine(); // debug dump

	COUTLN( std::endl << "event: birth! (transition from 'alive' to 'alive' again (really!))" );
	theHero.event_birth();
	theHero.step();
	theHero.step();
	theHero.step();
	theHero.step();
	theHero.step();
	theHero.step();
	theHero.step();
	theHero.step();
/*
	COUTLN( std::endl << "event: die! (transition from 'alive' to 'dead' and then on to 'afterlife')" );
	theHero.event_die();
	theHero.step();*/
}

int main()
{
	try
	{
		std::cout << std::endl << "Demo provided for YAKE by Stephan Kaiser" << std::endl << std::endl;
		test_fsm();
	}
	catch (const yake::Exception & e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
