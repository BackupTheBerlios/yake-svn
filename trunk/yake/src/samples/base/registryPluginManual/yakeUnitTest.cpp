//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <inc/base/yake.h>
#include <inc/registryPluginInterface/interface.h>

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
YAKE_DECLARE_GLOBAL
int main()
{
	YAKE_DECLARE_FUNCTION( main )
	using namespace yake::base;
	using namespace yake::base::templates;

	// Just load it manually
	YAKE_LOG_INFORMATION( "Loading dll..." )		
	Library* pDynLib = new Library( "registryPluginConcrete.dll" );
	YAKE_ASSERT( pDynLib ).debug( "Out of memory." );

	// Get the concrete implementation
	YAKE_LOG_INFORMATION( "Creating concrete implementation..." )
	Pointer< INetworkDriver > pNetDriver = create< INetworkDriver >();

	// Huhu my new friend
	DebugOutputStream::instance() << pNetDriver->sayHelloMyFriend();

	// Destroy the object BEFORE the dll is unloaded, otherwise 
	// the destructor code would be not available
	pNetDriver.reset();

	// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

  YAKE_LOG_INFORMATION( "Unloading..." )

	// Unload dll
	delete pDynLib;

	return 0;
}