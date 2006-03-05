#ifndef YAKE_OBJECTMANAGERLISTENER_H
#define YAKE_OBJECTMANAGERLISTENER_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object.h"

namespace yake {
namespace object {

	struct YAKE_ENT_API ObjectManagerListener
	{
		virtual ~ObjectManagerListener() {}
		virtual void onObjectCreated(Object*) {}
		virtual void onObjectInitialized(Object*) {}
		virtual void onDestroyObject(Object*) {}
	};

} // namespace object
} // namespace yake

#endif
