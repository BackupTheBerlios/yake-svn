#ifndef YAKE_ENT_OBJECTMANAGER_H
#define YAKE_ENT_OBJECTMANAGER_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object.h"
#include "yake/ent/object_mgr_listener.h"

namespace yake {
namespace ent {

	typedef object::ResultCode RegistrationResultCode;
	typedef std::pair<RegistrationResultCode,ClassId> RegistrationResult;

	struct YAKE_ENT_API ObjectManager : public ListenerManager<ObjectManagerListener>
	{
		// operations
		Object* makeObject(const ClassId clsId);
		Object* makeObject(const std::string strClsId);
		void destroyObject(Object*);
		void tick();

		template<typename T>
		inline RegistrationResult registerClass(const std::string& name)
		{
			return objMgr_.registerClass(name,&T::create,&T::destroy);
		}
	protected:
		//
		typedef ListenerManager<ObjectManagerListener> listener_mgr_type;
		void listeners_onObjectCreated(Object* obj);
		void listeners_onObjectInitialized(Object* obj);
		void listeners_onDestroyObject(Object* obj);
	private:
		typedef std::deque<Object*> object_ptr_list;
		object_ptr_list						objs_;

		typedef object::ObjectManager<Object> obj_mgr_type;
		obj_mgr_type							objMgr_;
	};

} // namespace ent
} // namespace yake

#endif
