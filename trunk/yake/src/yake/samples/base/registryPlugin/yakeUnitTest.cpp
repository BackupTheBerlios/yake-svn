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
#include <yake/base/yake.h>
#include <yake/registryPluginInterface/interface.h>

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
YAKE_DECLARE_GLOBAL
int main()
{
	YAKE_DECLARE_FUNCTION(main)
	using namespace yake::base;
	using namespace yake::base::templates;

	// Just load it - automagically!
	YAKE_LOG_INFORMATION("Loading dll...")
	YAKE_USE_LIB(registryPluginConcrete);
		
	// Get the concrete implementation
	YAKE_LOG_INFORMATION("Creating concrete implementation...")
	SharedPtr<INetworkDriver> pNetDriver = create_default<INetworkDriver>();
	// Huhu my new friend
	DebugOutputStream::instance() << pNetDriver->sayHelloMyFriend();

	// Get the concrete implementation
	YAKE_LOG_INFORMATION("Creating second concrete implementation...")
	SharedPtr<INetworkDriver2> pNetDriver2 = create_default<INetworkDriver2>(12345);
	// Huhu my new friend
	DebugOutputStream::instance() << pNetDriver2->sayHelloMyFriend();

	// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

  YAKE_LOG_INFORMATION( "Unloading..." )
	return 0;
}