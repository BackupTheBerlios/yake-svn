#include "windows.h"
#include <iostream>
#include <virtualfs/inc/resolve.hpp>
#include <inc/native_file_system.hpp>

// todo linux _init() equivalence
// or __attribute__((constructor))  and  __attribute__((destructor))
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
	using namespace filesystem::virtualfs;

	// register a reference to our singleton file system
  if( ul_reason_for_call == DLL_PROCESS_ATTACH ) 
		resolve::instance().load_fs( archive_id, 
			file_system_ptr( &native_file_system::instance() ) );

	// unregister file system
  if( ul_reason_for_call == DLL_PROCESS_DETACH ) 
		resolve::instance().unload_fs( archive_id );

	return true;
}