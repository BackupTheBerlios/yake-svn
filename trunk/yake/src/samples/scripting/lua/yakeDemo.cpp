//****************************************************************************
//**
//**    Sample
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <inc/base/yake.h>
#include <inc/samples/common/yakeExampleApplication.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::templates;
using namespace yake::scripting;

//============================================================================
//    Application
//============================================================================

class TheApp : public yake::exapp::ExampleApplication
{
public:
	TheApp() : ExampleApplication( false, false, true, false, true )
	{
	}
	virtual void run()
	{
		yake::scripting::IVM* pVM = getScriptingSystem().createVM();
		YAKE_ASSERT( pVM );

		getScriptingBindings().bind( pVM );

		// This prints 'hello from lua scripting ;)' to stdout using
		// the Lua function print().
		pVM->execute("print('hello from lua scripting ;)');");

		// Execute a script created from a file.
		yake::scripting::IScript* pScript = getScriptingSystem().createScriptFromFile("../../media/samples/scriptingLua/test.lua");
		pVM->execute( pScript );
		delete pScript;

		delete pVM;
	}
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

YAKE_DECLARE_GLOBAL
int main()
{
	YAKE_DECLARE_FUNCTION( main )

	{
		TheApp theApp;
		theApp.initialise();
		theApp.run();
	}

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}