#ifndef YAKE_OBJECT_H
#define YAKE_OBJECT_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object_listener.h"
#include "yake/ent/vm_holder.h"

namespace yake {
namespace object {

#define DECL_GO(CLASS,DESC) \
	DECL_CLASS_RTTI(CLASS,DESC) \
public: \
	static CLASS* cast(::yake::object::Co*);

	struct YAKE_ENT_API Object : public CoHolder, public VMHolder, public yake::PropHolder<Object>, public object_listener_mgr
	{
		DECL_GO(Object,"Object/Entity base")
		Object() {}
		virtual ~Object() {}

		typedef object_fsm::state_type fsm_state_type;
		typedef object_fsm::event_type fsm_event_type;

		// operations
		void init()
		{
			listeners_init(*this);
		}
		void tick()
		{
			listeners_tick(*this);
		}
		void addFsmState(const fsm_state_type&);
		void addFsmTransition(const fsm_state_type&, const fsm_event_type&, const fsm_state_type&);
		void setFsmState(const fsm_state_type&);
		void processFsmEvent(const fsm_event_type& evt);
		const fsm_state_type& getCurrentFsmState() const
		{ return m_.current(); }


		scripting::IVM* getFsmVM() const
		{
			 return this->getVM("main");
		}
		// signals
		typedef boost::signal<void(const std::string&,scripting::IVM*)> VmInitializedSignal;
		boost::signals::connection subscribeToVmInitialized(const VmInitializedSignal::slot_type&);

	private:
		void onEnter(const object_fsm&, const object_fsm::state_type& state);
		void onExit(const object_fsm&, const object_fsm::state_type& state);
	private:
		object_fsm			m_;
	};

} // namespace object
} // namespace yake

#endif
