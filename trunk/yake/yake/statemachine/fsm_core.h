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
	template<typename state_type_>
	const state_type_& get_null_state();

	template<typename state_type_>
	inline bool is_state_null(const state_type_& state)
	{
		return (state == get_null_state<state_type_>());
	}

	template<>
	inline const int& get_null_state()
	{
		static int ms_null = 0xffffffff;
		return ms_null;
	}
	template<>
	inline const unsigned char& get_null_state()
	{
		static unsigned char ms_null = 0xff;
		return ms_null;
	}
	template<>
	inline const std::string& get_null_state()
	{
		static std::string ms_null = "null";
		return ms_null;
	}
#endif
	template<typename T1>
	struct do_nothing_on_one
	{
		void operator()(const T1&)
		{}
	};
	template<typename T1, typename T2>
	struct do_nothing_on_two
	{
		void operator()(const T1&,const T2&)
		{}
	};

	template<typename _state_type, typename _event_type>
	struct machine
	{
		// types
		typedef _state_type state_type;
		typedef _event_type event_type;
		typedef machine machine_type;

		typedef do_nothing_on_one<state_type> do_nothing_state_op;
		typedef do_nothing_on_two<machine_type,state_type> do_nothing_op;

		// construction
#if defined(FSM_USE_NULL_STATE)
		machine() : 
			curr_(get_null_state<state_type>()) 
#else
		machine()
#endif
		{}

		// operations
		void addState(const state_type&);
		//? void addEvent(const event_type);
		void addTransition(const state_type&, const event_type&, const state_type&);
		void setState(const state_type&);

		void clear();

		size_t numStates() const;
		size_t numOffgoingTransitions(const state_type&) const; // off-going transitions for source state
		const state_type& current() const;

#ifdef _DEBUG
		void dump(std::ostream& out) const
		{
			out << "machine<>::dump()\n";
			out << "  current = '" << curr_ << "'\n";
			for (state_map::const_iterator it = states_.begin(); it != states_.end(); ++it)
			{
				out << "  state '" << it->first << "'\n";
				const event_state_map& evt2state = it->second.offgoing_transitions;
				for (event_state_map::const_iterator itT = evt2state.begin(); itT != evt2state.end(); ++itT)
				{
					out << "    event '" << itT->first << "' -> state '" << itT->second << "'\n";
				}
			}
		}
#endif

		std::pair<bool,std::string> validate();

		void processEvent(const event_type& evt)
		{
			do_nothing_op do_nothing;
			processEventCb(evt, do_nothing, do_nothing);
		}

		template<typename enter_state_op_t, typename exit_state_op_t>
		void processEventCb(const event_type& evt, enter_state_op_t& enterOp, exit_state_op_t& exitOp)
		{
			const state_map::const_iterator it = states_.find( curr_ );
			if (it == states_.end())
			{
				std::cerr << "error: machine<>::processEventCb():\n   invalid current state.\n\n";
				return;
			}
			const event_state_map& evt2state = it->second.offgoing_transitions;
			const event_state_map::const_iterator itFindTransition = evt2state.find(evt);
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
		const state_map::const_iterator it = states_.find( dest );
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
		state_map::iterator it = states_.find( state );
		if (it == states_.end())
		{
			states_.insert( state_map::value_type(state,state_e()) );
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
		state_map::iterator it = states_.find( src );
		if (it == states_.end())
		{
			std::cerr << "error: machine<>::addTransition():\n   unregistered source state!\n\n";
			return;
		}
		{
			const state_map::const_iterator itDest = states_.find( dest );
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
		evt2state.insert( event_state_map::value_type( evt, dest) );
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
		const state_map::const_iterator it = states_.find( src );
		return ( (it == states_.end()) ? 0 : it->second.offgoing_transitions.size() );
	}
} // namespace fsm

#endif
