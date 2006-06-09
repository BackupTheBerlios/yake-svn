// Yake
#include <yake/base/yake.h>
#include <yake/base/templates/yakeRegistry.h>

using namespace yake;
using namespace yake::templates;

namespace yake {
namespace testsuite {
	
	struct INetworkDriver
	{
		YAKE_DECLARE_REGISTRY_0( INetworkDriver, String )

		// Destructor.
		virtual ~INetworkDriver() {}

		// Resolve host.
		virtual const String resolveHost() const = 0;
	};

	YAKE_IMPLEMENT_REGISTRY( INetworkDriver )


namespace win32 {

	class NetworkDriver : public INetworkDriver
	{		
	public:
		YAKE_DECLARE_CONCRETE( NetworkDriver, "win32" )

		virtual ~NetworkDriver() {}

		virtual const String resolveHost() const
		{
			return String( "Hello, its me - the win32 implementation :)" );
		}
	};

	YAKE_REGISTER_CONCRETE( NetworkDriver )

} // namespace win32

namespace Linux {

	class NetworkDriver : public INetworkDriver
	{
	public:
		YAKE_DECLARE_CONCRETE( NetworkDriver, "linux" )

		virtual ~NetworkDriver() {}


		virtual const String resolveHost() const
		{
			return String( "Hello, its me - the Linux implementation ;)" );
		}
	};

	YAKE_REGISTER_CONCRETE( NetworkDriver )

} // namespace linux

} // namespace testsuite
} // namespace yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	using namespace yake;
	using namespace yake::testsuite;

	// Create a 'default' driver. (Can be 'Win32' or 'linux', whatever is registered first (or at all)).
	SharedPtr< INetworkDriver > pNetDriver = create_default< INetworkDriver >();
	std::cout << pNetDriver->resolveHost();

	// Explicitely create the 'Win32' driver.
	SharedPtr< INetworkDriver > pWin32NetDriver = create< INetworkDriver >("win32");
	std::cout << pWin32NetDriver->resolveHost();

	// Explicitely create the 'Linux' driver.
	SharedPtr< INetworkDriver > pLinuxNetDriver = create< INetworkDriver >("linux");
	std::cout << pLinuxNetDriver->resolveHost();

	// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}