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
#include <inc/base/templates/yakeRegistry.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base;
using namespace yake::base::templates;

namespace yake
{
namespace testsuite
{
	
struct INetworkDriver
{
	// Destructor.
	virtual ~INetworkDriver() {}

	// Resolve host.
	virtual const String resolveHost() const = 0;

YAKE_DECLARE_REGISTRY_0( INetworkDriver, String )
};

YAKE_IMPLEMENT_REGISTRY( INetworkDriver )


namespace win32
{

class NetworkDriver : public INetworkDriver
{		
YAKE_DECLARE_CLASS( yake::testsuite::win32::NetworkDriver )
// Class
public:
	// Destructor.
	virtual ~NetworkDriver() {}

// Methods
public:
	// Resolve host.
	const String resolveHost() const
	{
		YAKE_DECLARE_FUNCTION( ResolveHost )
		return String( YAKE_HERE + ": Hello, its me - the win32 implementation :)" );
	}

YAKE_DECLARE_CONCRETE( NetworkDriver, "win32" )
};

YAKE_REGISTER_CONCRETE( NetworkDriver )

} // win32
} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	using namespace yake::testsuite;

	Pointer< INetworkDriver > pNetDriver = create< INetworkDriver >();

	DebugOutputStream::instance() << pNetDriver->resolveHost();

	// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}