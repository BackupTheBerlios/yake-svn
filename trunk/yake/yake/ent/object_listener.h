#ifndef YAKE_ENT_OBJECTLISTENER_H
#define YAKE_ENT_OBJECTLISTENER_H

#include "yake/ent/prerequisites.h"

namespace yake {
namespace ent {

	struct Object;
	struct YAKE_ENT_API ObjectListener
	{
		virtual ~ObjectListener() {}
		virtual void onTick(Object&) {}
		virtual void onInit(Object&) {}
		virtual void onFsmEventHandled(Object&, const object_fsm&, const object_fsm::event_type&) {}
		virtual void onFsmEnterState(Object&,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type&) {}
		virtual void onFsmExitState(Object&,/*const std::string& fsmName, */const object_fsm&, const object_fsm::state_type&) {}
	};

	struct YAKE_ENT_API ObjectListenerManager : public ListenerManager<ObjectListener>
	{
	protected:
		void listeners_init(Object& obj)
		{
			yake::ConstVectorIterator<listener_list> it(listeners_);
			while (it.hasMoreElements())
				it.getNext().first->onInit(obj);
		}
		void listeners_tick(Object& obj)
		{
			yake::ConstVectorIterator<listener_list> it(listeners_);
			while (it.hasMoreElements())
				it.getNext().first->onTick(obj);
		}
		void listeners_fsmEventHandled(Object& obj,/*const std::string& fsmName, */const object_fsm& fsm, const object_fsm::event_type& evt)
		{
			yake::ConstVectorIterator<listener_list> it(listeners_);
			while (it.hasMoreElements())
				it.getNext().first->onFsmEventHandled(obj,fsm,evt);
		}
		void listeners_fsmEnterState(Object& obj,/*const std::string& fsmName, */const object_fsm& fsm, const object_fsm::state_type& state)
		{
			yake::ConstVectorIterator<listener_list> it(listeners_);
			while (it.hasMoreElements())
				it.getNext().first->onFsmEnterState(obj,fsm,state);
		}
		void listeners_fsmExitState(Object& obj,/*const std::string& fsmName, */const object_fsm& fsm, const object_fsm::state_type& state)
		{
			yake::ConstVectorIterator<listener_list> it(listeners_);
			while (it.hasMoreElements())
				it.getNext().first->onFsmExitState(obj,fsm,state);
		}
	};

} // namespace ent
} // namespace yake

#endif
