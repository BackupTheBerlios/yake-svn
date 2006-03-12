#ifndef YAKE_ENT_OBJECT_H
#define YAKE_ENT_OBJECT_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object_listener.h"
#include "yake/ent/vm_holder.h"

namespace yake {
namespace ent {

#define DECL_OBJECT(CLASS,DESC) \
	DECL_CLASS_RTTI(CLASS,DESC) \
public: \
	static ::yake::ent::Object* create(const ObjectId& id) \
	{ \
		::yake::ent::Object* obj = new CLASS(); \
		obj->setId(id); \
		return obj; \
	} \
	static void destroy(::yake::ent::Object* o) { if(o) delete o; } \
	static CLASS* cast(::yake::ent::Object*);

#define IMPL_OBJECT(CLASS) \
	IMPL_CLASS_RTTI(CLASS) \
	CLASS* CLASS::cast(::yake::ent::Object* obj) \
	{ \
		return (obj ? ((obj->isA() == CLASS::cls()) ? dynamic_cast<CLASS*>(obj) : 0 ) : 0); \
	}

	struct YAKE_ENT_API Object : public yake::PropHolder<Object>, public ObjectListenerManager
	{
		DECL_OBJECT(Object,"Object")
		Object() {}
		virtual ~Object() {}

		// id operations
		void setId(const ObjectId& id);
		const ObjectId& getId() const;
		const ObjectId& id() const
		{ return id_; }

		// operations
		void init()
		{
			listeners_init(*this);
			onInit();
		}
		void tick()
		{
			listeners_tick(*this);
			onTick();
		}
	protected:
		virtual void onInit() {}
		virtual void onTick() {}
	private:
		ObjectId				id_;
	};

	struct YAKE_ENT_API Entity : public Object, public CoHolder, public VMHolder
	{
		DECL_OBJECT(Entity,"Entity")
		Entity() {}
		virtual ~Entity() {}

		typedef object_fsm::state_type fsm_state_type;
		typedef object_fsm::event_type fsm_event_type;

		// id operations
		void setId(const ObjectId& id);
		const ObjectId& getId() const;

		// FSM operations

		void addFsmState(const fsm_state_type&);
		void addFsmTransition(const fsm_state_type&, const fsm_event_type&, const fsm_state_type&);
		void setFsmState(const fsm_state_type&);
		void processFsmEvent(const fsm_event_type& evt);
		const fsm_state_type& getCurrentFsmState() const
		{ return m_.current(); }

		// VM operations
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

} // namespace ent
} // namespace yake

#endif
