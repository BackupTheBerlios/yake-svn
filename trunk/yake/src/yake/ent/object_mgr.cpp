#include "yake/ent/pch.h"
#include "yake/ent/ent.h"

namespace yake {
namespace ent {

	Object* ObjectManager::makeObject(const std::string strClsId)
	{
		YAKE_ASSERT( !strClsId.empty() )(strClsId);
		std::pair<object::ResultCode,ClassId> ret = objMgr_.getClassId(strClsId);
		YAKE_ASSERT( ret.first == object::RC_OK )(ret.first)(ret.second);
		return makeObject( ret.second );
	}
	Object* ObjectManager::makeObject(const ClassId clsId)
	{
		Object* obj = objMgr_.createObject(clsId);
		YAKE_ASSERT( obj )(clsId);
		objs_.push_back( obj );

		listeners_onObjectCreated(obj);

		obj->init();

		listeners_onObjectInitialized(obj);

		return obj;
	}
	void ObjectManager::destroyObject(Object* obj)
	{
		YAKE_ASSERT( obj );

		listeners_onDestroyObject( obj );
		//obj->shutdown();

		object_ptr_list::iterator it = std::find(objs_.begin(),objs_.end(),obj);
		if (it != objs_.end())
			objs_.erase( it );

		objMgr_.destroyObject( obj );
	}
	void ObjectManager::tick()
	{
		yake::ConstVectorIterator<object_ptr_list> it(objs_);
		while (it.hasMoreElements())
			it.getNext()->tick();
	}
	void ObjectManager::listeners_onObjectCreated(Object* obj)
	{
		assert( obj );
		if (!obj)
			return;
		yake::ConstVectorIterator<listener_mgr_type::listener_list> it(listeners_);
		while (it.hasMoreElements())
			it.getNext().first->onObjectCreated(obj);
	}
	void ObjectManager::listeners_onObjectInitialized(Object* obj)
	{
		assert( obj );
		if (!obj)
			return;
		yake::ConstVectorIterator<listener_mgr_type::listener_list> it(listeners_);
		while (it.hasMoreElements())
			it.getNext().first->onObjectInitialized(obj);
	}
	void ObjectManager::listeners_onDestroyObject(Object* obj)
	{
		assert( obj );
		if (!obj)
			return;
		yake::ConstVectorIterator<listener_mgr_type::listener_list> it(listeners_);
		while (it.hasMoreElements())
			it.getNext().first->onDestroyObject(obj);
	}

}
}