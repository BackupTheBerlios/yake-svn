#ifndef NET_PREREQUISITES_H
#define NET_PREREQUISITES_H

#include <yake/base/yakePrerequisites.h>
#include <yake/base/yake.h>

#define NET_DLL

#ifdef YAKE_NET_EXPORTS
#	ifdef NET_DLL
#		define NET_API DLLEXPORT
#	else
#		define NET_API
#	endif
#else
#	ifdef NET_DLL
#		define NET_API DLLIMPORT
#	else
#		define NET_API
#	endif
#	if defined( _DEBUG )
#		pragma comment(lib,"yake_net-vc80-d.lib")
#	else
#		pragma comment(lib,"yake_net-vc80.lib")
#	endif
#endif

#include <cassert>
#include <string>
#include <vector>
#include <deque>
#include <boost/function.hpp>
#include "netTypes.h"

#define NET_ASSERT(X) \
	YAKE_ASSERT(X)


#endif
