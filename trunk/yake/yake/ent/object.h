/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
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

	/** @todo Do not publicly inherit from CoHolder and VMHolder!?
	*/
	struct YAKE_ENT_API Entity : public Object, public CoHolder
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

		boost::signals::connection subsribeToVmAttached(const VMHolder::VmAttachedSignal::slot_type&);
		boost::signals::connection subscribeToVmDetached(const VMHolder::VmDetachedSignal::slot_type&);

		bool attachVM(scripting::IVM*,const std::string& tag);
		scripting::IVM* detachVM(const std::string& tag);
		scripting::IVM* getVM(const std::string& tag) const;

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
		VMHolder				vms_;
	};

} // namespace ent
} // namespace yake

#endif
