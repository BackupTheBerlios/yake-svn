/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
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
#ifndef YAPP_STATEMACHINE_FSM_H
#define YAPP_STATEMACHINE_FSM_H

#include <yapp/base/yappPrerequisites.h>

//State --transition--> State
// A quote from some website ( I forgot which one ):
//"(...) an event is the stimulus that causes a state machine to transition between states."
// Event => State --transition--> State
//

namespace yake {
namespace app {

	enum ErrorCode {
		eInvalidParam,
		eOk,
		eError,
		eRejected,
		eInvalidNesting
	};

namespace state {

	inline std::ostream& operator << (std::ostream& lhs, const ErrorCode & rhs)
	{
		lhs << "ErrorCode (" << int(rhs) << "): ";
		switch (rhs)
		{
		case eOk:
			lhs << "OK";
			break;
		case eRejected:
			lhs << "Rejected";
			break;
		case eInvalidParam:
			lhs << "Invalid Parameter";
			break;
		case eInvalidNesting:
			lhs << "Invalid Nesting";
			break;
		default:
			lhs << "UNKNOWN";
		};
		return lhs;
	}


	/** A simple state machine managing states and transitions between registered states.
	*/
	template<typename StateIdType>
	class StateMachine
	{
	public:
		typedef StateIdType StateId; // can be made templated.

		typedef boost::function<void(void)> StateFn;
		typedef std::map< StateId, StateFn > StateFnMap;

		typedef boost::function<void(void)> TransitionFn;
		typedef std::deque< TransitionFn > TransitionFnList;
		typedef std::pair<StateId,StateId> StateIdPair;
		typedef std::map< StateIdPair, TransitionFnList > TransitionMap;

		const static StateId kStateNone;
	public:
		StateMachine();

		//@{ name control
		/** Return the id of the currently active state.
			@return the id of the currently active state or kStateNone.
		*/
		StateId getCurrentState() const;
		/** Initiate a state transition from the currently active state to a new one.
			The transition table will be searched for a valid state transition. If
			a transition is found it will be executed and the new state will become active.
			The new state's state function will be called.
			@Remarks Rossible return values:
				- eRejected: no valid transition from the currently active to state to the specified one
				- eInvalidParam: no state with the given id (idTo) was registered.
				- eInvalidNesting: A transition was already in progress, nested transitions are not allowed.
			@param idTo the id of the target state
		*/
		ErrorCode changeStateTo( const StateId & idTo );
		//@}

		//@{ name state registration
		/** Register a state.
			@Remarks If a state with the given id exists in the machine the operation will replcae
				the existing registration information!
			@param id state id
			@param fn the function associated with the state
		*/
		void regState( const StateId & id, const StateFn & fn );
		/** Unregister a state.
			A state and all state transitions originating from it are unregistered.
			Transitions that target the unregistered state will fail.
			@todo unregister transitions!
			@param id state id
		*/
		void unregState( const StateId & id );
		//@}

		//@{ name transition registration
		/** Register a transition from one state to another.
			No transition function will be registered.
			@param idFrom source state
			@param idTo target state
		*/
		void regTransition( const StateId & idFrom, const StateId & idTo );
		/** Register a transition from one state to another and a function to be called when the
			transition is executed.
			@param idFrom source state
			@param idTo target state
			@param fn transition function
		*/
		void regTransition( const StateId & idFrom, const StateId & idTo, const TransitionFn & fn );
		/** Register a transition from one state to another and a list of functions to be called when the
			transition is executed.
			@param idFrom source state
			@param idTo target state
			@param fns transition functions
		*/
		void regTransition( const StateId & idFrom, const StateId & idTo, const TransitionFnList & fns );
		//@}
		/** Adds a single transition function to a registered transition.
			@see regTransition
		*/
		void addTransitionFn( const StateId & idFrom, const StateId & idTo, const TransitionFn & fn );

		/// Dumps state machine information to a stream.
		void dump(std::ostream& rhs) const;
	private:
		bool _stateExists( const StateId & id ) const;
		/// Returns true if a transition was found, false if not. If found, then fns contains the list of the transition's functions.
		bool _getTransition( const StateIdPair & idpair, TransitionFnList & fns ) const;
		/** Execute the specified state's function.
			@Return true if successful, otherwise false.
		*/
		bool _executeState( const StateId & id );
	private:
		StateId			mCurrentStateId;
		StateFnMap		mStates;
		TransitionMap	mTransitions;
		bool				mTransitionInProgress;
	};

	/// A specialization for state machines using String for identifying states.
	template<> const String StateMachine<String>::kStateNone = "NONE";

	#include "yakeFiniteStateMachine_inc.h"

} // state
} // app
} // yake

#endif