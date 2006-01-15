#ifndef NET_H
#define NET_H

#include "netPrerequisites.h"
#include "netCommon.h"
#include "netPacket.h"
#include "netEvent.h"

namespace net {

	NET_API bool initialize();
	NET_API void shutdown();

} // namespace net

#endif
