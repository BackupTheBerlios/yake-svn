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
#ifndef FSM_OOMACHINE_H
#define FSM_OOMACHINE_H

#include "yake/statemachine/fsm_core.h"

namespace fsm {

	template<typename _state_class_type /*= oostate */, typename _event_type>
	struct oomachine
	{
		typedef _event_type event_type;
		typedef _state_class_type* state_type;

		oomachine() :
			fnEnter_( boost::bind(&oomachine::onEnter,this,_1,_2) ),
			fnExit_( boost::bind(&oomachine::onExit,this,_1,_2) )
		{
		}

		void addState(const state_type);
		void addTransition(const state_type, const event_type&, const state_type);
		void setState(const state_type);
		void processEvent(const event_type& evt);
		const state_type& current() const
		{ return machine_.current(); }
	private:
		typedef ::fsm::machine<state_type,event_type> machine_type;
		void onEnter(machine_type&,const state_type&);
		void onExit(machine_type&,const state_type&);
	private:
		machine_type			machine_;
		typedef boost::function<void(machine_type&,const state_type&)> CbFn;
		CbFn						fnEnter_;
		CbFn						fnExit_;
	};

	template<typename _state_class_type, typename _event_type>
	void oomachine<_state_class_type,_event_type>::onEnter(machine_type&,const state_type& state)
	{
		YAKE_ASSERT( state );
		state->onEnter();
	}
	template<typename _state_class_type, typename _event_type>
	void oomachine<_state_class_type,_event_type>::onExit(machine_type&,const state_type& state)
	{
		YAKE_ASSERT( state );
		state->onExit();
	}
	template<typename _state_class_type, typename _event_type>
	void oomachine<_state_class_type,_event_type>::addState(const state_type s)
	{
		machine_.addState(s);
	}
	template<typename _state_class_type, typename _event_type>
	void oomachine<_state_class_type,_event_type>::addTransition(const state_type from, const event_type& ev, const state_type to)
	{
		machine_.addTransition( from, ev, to );
	}
	template<typename _state_class_type, typename _event_type>
	void oomachine<_state_class_type,_event_type>::setState(const state_type s)
	{
		if (machine_.current())
			fnExit_( machine_, machine_.current() );
		fnEnter_( machine_, s );
		machine_.setState( s );
	}
	template<typename _state_class_type, typename _event_type>
	void oomachine<_state_class_type,_event_type>::processEvent(const event_type& evt)
	{
		YAKE_ASSERT( !fnEnter_.empty() );
		machine_.processEventCb(evt, fnEnter_, fnExit_);
	}

} // namespace fsm

#endif
