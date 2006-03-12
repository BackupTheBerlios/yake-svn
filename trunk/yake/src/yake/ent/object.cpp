#include "yake/ent/pch.h"
#include "yake/ent/ent.h"

namespace yake {
namespace ent {

	IMPL_OBJECT(Object)

	void Object::setId(const ObjectId& id)
	{
		id_ = id;
	}
	const ObjectId& Object::getId() const
	{
		return id_;
	}

	IMPL_OBJECT(Entity)

	void Entity::processFsmEvent(const fsm_event_type& evt)
	{
		m_.processEventCb(evt,boost::bind(&Entity::onEnter,this,_1,_2),boost::bind(&Entity::onExit,this,_1,_2));
		listeners_fsmEventHandled(*this,m_,evt);
	}
	void Entity::addFsmState(const fsm_state_type& state)
	{
		m_.addState(state);
	}
	void Entity::addFsmTransition(const fsm_state_type& from, const fsm_event_type& evt, const fsm_state_type& to)
	{
		m_.addTransition(from,evt,to);
	}
	void Entity::setFsmState(const fsm_state_type& state)
	{
		m_.setState(state);
	}
	void Entity::onEnter(const object_fsm& fsm, const object_fsm::state_type& state)
	{
		//std::cout << "Entity.state.enter '" << state << "'\n";
		listeners_fsmEnterState(*this,fsm,state);
	}
	void Entity::onExit(const object_fsm& fsm, const object_fsm::state_type& state)
	{
		//std::cout << "Entity.state.exit '" << state << "'\n";
		listeners_fsmExitState(*this,fsm,state);
	}
}
}