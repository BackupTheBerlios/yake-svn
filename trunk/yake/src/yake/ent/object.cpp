#include "yake/ent/pch.h"
#include "yake/ent/ent.h"

namespace yake {
namespace object {

	IMPL_CLASS_RTTI(Object)

	void Object::processFsmEvent(const fsm_event_type& evt)
	{
		m_.processEventCb(evt,boost::bind(&Object::onEnter,this,_1,_2),boost::bind(&Object::onExit,this,_1,_2));
		listeners_fsmEventHandled(*this,m_,evt);
	}
	void Object::addFsmState(const fsm_state_type& state)
	{
		m_.addState(state);
	}
	void Object::addFsmTransition(const fsm_state_type& from, const fsm_event_type& evt, const fsm_state_type& to)
	{
		m_.addTransition(from,evt,to);
	}
	void Object::setFsmState(const fsm_state_type& state)
	{
		m_.setState(state);
	}
	void Object::onEnter(const object_fsm& fsm, const object_fsm::state_type& state)
	{
		//std::cout << "object.state.enter '" << state << "'\n";
		listeners_fsmEnterState(*this,fsm,state);
	}
	void Object::onExit(const object_fsm& fsm, const object_fsm::state_type& state)
	{
		//std::cout << "object.state.exit '" << state << "'\n";
		listeners_fsmExitState(*this,fsm,state);
	}

}
}