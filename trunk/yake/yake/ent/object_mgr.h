#ifndef YAKE_OBJECTMANAGER_H
#define YAKE_OBJECTMANAGER_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object.h"
#include "yake/ent/object_mgr_listener.h"

namespace yake {
namespace object {

	struct YAKE_ENT_API ObjectManager : public ListenerManager<ObjectManagerListener>
	{
		// operations
		Object* makeObject();
		void destroyObject(Object*);
		void tick();
	protected:
		typedef ListenerManager<ObjectManagerListener> listener_mgr_type;
		void listeners_onObjectCreated(Object* obj);
		void listeners_onObjectInitialized(Object* obj);
		void listeners_onDestroyObject(Object* obj);
	private:
		typedef std::deque<Object*> object_ptr_list;
		object_ptr_list	objs_;
	};

} // namespace object
} // namespace yake

#endif
