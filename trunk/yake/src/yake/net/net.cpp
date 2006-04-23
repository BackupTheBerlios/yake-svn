#include <yake/net/pch.h>
#include <enet/enet.h>
#include <yake/net/net.h>
#include <yake/net/detail/netInternal.h>

#if YAKE_PLATFORM == PLATFORM_WIN32

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif // PLATFORM_WIN32

namespace net {

	bool initialize()
	{
		if (enet_initialize() < 0)
		{
			fprintf(stderr, "An error occurred while initializing ENet.\n");
			return false;
		}
		impl::UpdateThread::create();
		return true;
	}
	void shutdown()
	{
		impl::UpdateThread::destroy();
		enet_deinitialize();
	}

} // namespace net
