#ifndef NET_PREREQUISITES_H
#define NET_PREREQUISITES_H

#define NET_DLL

#ifdef YAKE_NET_EXPORTS
#	ifdef NET_DLL
#		define NET_API __declspec(dllexport)
#	else
#		define NET_API
#	endif
#else
#	ifdef NET_DLL
#		define NET_API __declspec(dllimport)
#	else
#		define NET_API
#	endif
#	if defined( _DEBUG )
#		pragma comment(lib,"net_d.lib")
#	else
#		pragma comment(lib,"net.lib")
#	endif
#endif

#include <cassert>
#include <string>
#include <vector>
#include <deque>
#include <boost/function.hpp>
#include "netTypes.h"

#define NET_ASSERT(X) \
	assert(X)


#endif