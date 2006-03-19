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
#ifndef FSM_CORE_H
#define FSM_CORE_H

#include <cassert> // assert()
#include <string>
#include <iostream> // std::cout in debug mode
#include <map>

// If defined the initial state is a null state.
// What a null state is and whether a state is a null state
// is determined by two templated functions for which specializations
// may or may not be needed depending on the target type.
// Specializations for common built-in and STL types are provided.
// The functions are: get_null_state<>(), is_state_null<>(..).
//
// If not defined, the initial state will be set to whatever the constructor
// of the state type sets it to.
//
// The initial state can be set to any state already registered with the machine
// using machine<>::setState(..).
//

#define FSM_USE_NULL_STATE

namespace fsm {
#if defined(FSM_USE_NULL_STATE)
	/** Returns the state representing a 'null state' for a given type (as template parameter).
		This function can be specialized for custom types. Specializations for common types are
		provided (string, int ...).
		See the documentation of machine<> for more information on null states.
		@see machine<>
	*/
	template<typename state_type_>
	const state_type_& get_null_state();

	/** Returns whether a state is equal to the null state for this type.
		This function can be specialized for custom types when necessary.
	*/
	template<typename state_type_>
	inline bool is_state_null(const state_type_& state)
	{
		return (state == get_null_state<state_type_>());
	}

	/** Specialization for 'int'. */
	template<>
	inline const int& get_null_state()
	{
		static int ms_null = 0xffffffff;
		return ms_null;
	}
	/** Specialization for 'unsigned char'. */
	template<>
	inline const unsigned char& get_null_state()
	{
		static unsigned char ms_null = 0xff;
		return ms_null;
	}
	/** Specialization for 'std::string'. */
	template<>
	inline const std::string& get_null_state()
	{
		static std::string ms_null = "null";
		return ms_null;
	}
#endif
	/** Generic operator object with a single parameter that does nothing (as the name implies). */
	template<typename T1>
	struct do_nothing_on_one
	{
		void operator()(const T1&)
		{}
	};
	/** Generic operator object with two parameters that does nothing (as the name implies). */
	template<typename T1, typename T2>
	struct do_nothing_on_two
	{
		void operator()(const T1&,const T2&)
		{}
	};

	/** Basic generic finite state machine. State and event types can be configured per machine type at compile-time.
		Machine instances are configured at run-time (i.e. states and transitions are added at run-time).
		@Remarks boost::statechart looks like a good candidate for a compile-time only state machine.
		@todo add findPathToState() which finds a transition path from the current to the specified destination state
				and enables the client to execute processEvent() for each event in the path.
		@see oomachine
	*/
	template<typename _state_type, typename _event_type>
	struct machine
	{
		// types

		/** Type of this machine's states. */
		typedef _state_type state_type;

		/** Type of this machine's events. */
		typedef _event_type event_type;

		/** Type to self. */
		typedef machine machine_type;

		/** Do nothing operation - used as default in processEvent(). */
		typedef do_nothing_on_one<state_type> do_nothing_state_op;

		/** Do nothing operation - used as default in processEvent(). */
		typedef do_nothing_on_two<machine_type,state_type> do_nothing_op;

		// construction
#if defined(FSM_USE_NULL_STATE)
		/** Constructs a machine. Sets the current state to the state type's null state. */
		machine() : 
			curr_(get_null_state<state_type>()) 
#else
		/** Constructs a machine. */
		machine()
#endif
		{}

		// operations

		/** Add a state.
			@Remarks Client code has to take care that the machine is in a valid state when events are processed.
		*/
		void addState(const state_type&);
		//? void addEvent(const event_type);

		/** Add a transition.
			@Remarks Client code has to take care that the machine is in a valid state when events are processed.
		*/
		void addTransition(const state_type&, const event_type&, const state_type&);

		/** Explicitely set the current state.
			A call to this function can be useful if the initial state has to be set explicitely. By default (if null
			states are enabled) the initial state is the null state and a transition from a null state to a custom
			state added with addState() has to be specified by the client code.
		*/
		void setState(const state_type&);

		/** Clears the machine, i.e. removes all states and transitions.
		*/
		void clear();

		/** Returns the number of states that have been added with addState(). */
		size_t numStates() const;
		/** Returns the number of outgoing transitions for a specific state. */
		size_t numOffgoingTransitions(const state_type&) const; // off-going transitions for source state

		/** Returns a reference to the current state. */
		const state_type& current() const;

#ifdef _DEBUG
		/** DEBUG-ONLY function for dumping the machine contents to a std::ostream object. */
		void dump(std::ostream& out) const
		{
			out << "machine<>::dump()\n";
			out << "  current = '" << curr_ << "'\n";
			for (state_map::const_iterator it = states_.begin(); it != states_.end(); ++it)
			{
				out << "  state '" << it->first << "'\n";
				const event_state_map& evt2state = it->second.offgoing_transitions;
				for ( typename event_state_map::const_iterator itT = evt2state.begin(); itT != evt2state.end(); ++itT)
				{
					out << "    event '" << itT->first << "' -> state '" << itT->second << "'\n";
				}
			}
		}
#endif

		/** This method validates the machine by making sure that states and transitions are properly connected, i.e.
			it finds unconnected states and invalid transitions.
			@todo implement!
		*/
		std::pair<bool,std::string> validate();

		/** Processes the given event thereby triggering a transition from the current state to the transition's target state.
			If no transition can be found for the combination of currently active state and given event, the method returns without doing anything.
		*/
		void processEvent(const event_type& evt)
		{
			do_nothing_op do_nothing;
			processEventCb(evt, do_nothing, do_nothing);
		}

		/** Same as processEvent but this method allows to provide custom callbacks for 'enter state' and 'exit state' calls.
			@copydoc processEvent
		*/
		template<typename enter_state_op_t, typename exit_state_op_t>
		void processEventCb(const event_type& evt, enter_state_op_t& enterOp, exit_state_op_t& exitOp)
		{
			const typename state_map::const_iterator it = states_.find( curr_ );
			if (it == states_.end())
			{
				std::cerr << "error: machine<>::processEventCb():\n   invalid current state.\n\n";
				return;
			}
			const event_state_map& evt2state = it->second.offgoing_transitions;
			const typename event_state_map::const_iterator itFindTransition = evt2state.find(evt);
			if (itFindTransition == evt2state.end())
			{
				std::cerr << "error: machine<>::processEventCb():\n   transition not found! (evt=" << evt << ", src=" << it->first << ")\n\n";
				return;
			}
			exitOp(*this,curr_);
			curr_ = itFindTransition->second;
			enterOp(*this,curr_);
		}

	private:
		// @todo hash_map<> or AssocVector<>
		typedef std::map<event_type,state_type> event_state_map;
		struct state_e
		{
			event_state_map offgoing_transitions;
			//void* userdata;
		};
		typedef std::map<state_type,state_e> state_map;
		state_map		states_;
		state_type		curr_;
	};
	template<typename _state_type, typename _event_type>
	void machine<_state_type,_event_type>::setState(const state_type& dest)
	{
		const typename state_map::const_iterator it = states_.find( dest );
		if (it == states_.end())
		{
			std::cerr << "error: machine<>::setState():\n   unregistered state.\n\n";
			return;
		}
		curr_ = dest;
	}
	template<typename _state_type, typename _event_type>
	const _state_type& machine<_state_type,_event_type>::current() const
	{
		return curr_;
	}
	template<typename _state_type, typename _event_type>
	void machine<_state_type,_event_type>::clear()
	{
#if defined(FSM_USE_NULL_STATE)
		curr_ = get_null_state<state_type>();
#else
		curr_ = state_type();
#endif
		states_.clear();
	}
#if 0 // alternative implementation
	template<typename _state_type, typename _event_type>
	void machine<_state_type,_event_type>::processEvent(const event_type& evt)
	{
		const state_map::const_iterator it = states_.find( curr_ );
		if (it == states_.end())
		{
			std::cerr << "error: machine<>::processEvent():\n   invalid current state.\n\n";
			return;
		}
		const event_state_map& evt2state = it->second.offgoing_transitions;
		const event_state_map::const_iterator itFindTransition = evt2state.find(evt);
		if (itFindTransition == evt2state.end())
		{
			std::cerr << "error: machine<>::processEvent():\n   transition not found! (evt=" << evt << ", src=" << it->first << ")\n\n";
			return;
		}
		curr_ = itFindTransition->second;
	}
#endif
	template<typename _state_type, typename _event_type>
	void machine<_state_type,_event_type>::addState(const state_type& state)
	{
		typename state_map::iterator it = states_.find( state );
		if (it == states_.end())
		{
			states_.insert( typename state_map::value_type(state,state_e()) );
			return;
		}
#ifdef _DEBUG
		std::cerr << "error: machine<>::addState():\n   state already exists!\n\n";
#endif
	}
	template<typename _state_type, typename _event_type>
	void machine<_state_type,_event_type>::addTransition(const state_type& src, const event_type& evt, const state_type& dest)
	{
		if (src == dest) // @todo Allow this !? Optionally? Shouldn't do much harm...
		{
			std::cerr << "error: machine<>::addTransition():\n   source and destination are identical!\n\n";
			return;
		}
		typename state_map::iterator it = states_.find( src );
		if (it == states_.end())
		{
			std::cerr << "error: machine<>::addTransition():\n   unregistered source state!\n\n";
			return;
		}
		{
			const typename state_map::const_iterator itDest = states_.find( dest );
			if (itDest == states_.end())
			{
				std::cerr << "error: machine<>::addTransition():\n   unregistered destination state!\n\n";
				return;
			}
		}
		event_state_map& evt2state = it->second.offgoing_transitions;
#ifdef _DEBUG
		if (evt2state.find(evt) != evt2state.end())
		{
			std::cerr << "error: machine<>::addTransition():\n   transition for this combination of source and event already exists!\n\n";
			return;
		}
#endif
		evt2state.insert( typename event_state_map::value_type( evt, dest) );
#ifdef _DEBUG
		std::cout << "machine<>::addTransition(): from=" << src << " evt=" << evt << " dest=" << dest << "\n";
#endif
	}
	template<typename _state_type, typename _event_type>
	size_t machine<_state_type,_event_type>::numStates() const
	{
		return states_.size();
	}
	template<typename _state_type, typename _event_type>
	size_t machine<_state_type,_event_type>::numOffgoingTransitions(const state_type& src) const
	{
		const typename state_map::const_iterator it = states_.find( src );
		return ( (it == states_.end()) ? 0 : it->second.offgoing_transitions.size() );
	}
} // namespace fsm

#endif

