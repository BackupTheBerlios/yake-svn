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

namespace yake {
namespace app {

	enum ErrorCode {
		eInvalidParam,
		eOk,
		eError,
		eRejected,
		eInvalidNesting,
		eAlreadyRegistered,
		eNotFound,
		eStateNotFound,
		eTransitionNotFound,
		eNestingTooDeep
	};

namespace state {

	typedef Signal0<void(void)> StepSignal;

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
		case eAlreadyRegistered:
			lhs << "Already registered";
			break;
		case eNotFound:
			lhs << "Not found";
			break;
		case eStateNotFound:
			lhs << "State not found";
			break;
		case eTransitionNotFound:
			lhs << "Transition not found";
			break;
		default:
			lhs << "UNKNOWN";
		};
		return lhs;
	}

	typedef boost::function<void(void)> TransitionFn;
	typedef std::deque< TransitionFn > TransitionFnList;

// todo: a type specific template? this seems to be senseless, please add a creator policy and kick the pure virtual function

	/** Template base class for very, very simple clonable objects.
		@todo move out into yake::base or yapp::base.
	*/
	template<typename T>
	struct clonableT
	{
		virtual ~clonableT() {}
		virtual T* clone() const = 0;
	};

#define YAKE_DECLARE_CLONABLE( BASECLASS, CLASS ) \
	public: \
		BASECLASS* clone() const\
		{ \
			return new CLASS(*this); \
		} \
	private:

	/** A state for a state machine.
		A state is clonable. Derived classes should therefore overload the copy constructor.
		A state has three main functions: enter, step, exit. Derived classes should implement
		onEnter(), onStep() and onExit() if they need to modify the default behaviour. Furthermore
		a call to either enter(), step() or exit() will trigger the corresponding event (event_enter,
		event_step, event_exit). Thereby it's possible to bind various kind of handlers to these
		events, for example, LUA scripting hooks.
	*/
	class YAPP_BASE_API State : public clonableT<State>
	{
		YAKE_DECLARE_CLONABLE( State, State );
	protected:
		State(const State& rkOther)
		{
		}
	public:
		State() {}
		virtual ~State() {};

		void enter();
		void step();
		void exit();

		// events
		typedef Signal0<void(void)> event_enter_sig;
		typedef Signal0<void(void)> event_step_sig;
		typedef Signal0<void(void)> event_exit_sig;
		event_enter_sig	event_enter;
		event_step_sig	event_step;
		event_exit_sig	event_exit;
		/*
		event<void> event_enter;
		event<void> event_step;
		event<void> event_exit;

		clients do this:
			coolstate = {
				enter = function {
					print("lala");
				}
				exit = function {
					print("lala");
				}
			}
			coolstate thestate;
			thestate:event_enter:connect( coolstate:f ); // or ( thestate:f ) ?
		*/
	protected:
		virtual void onEnter() {}
		virtual void onStep() {}
		virtual void onExit() {}
	};
	typedef State* StatePtr;

	/** A state transition in a state machine.
		A transition is used to directionally connect states. If transition
		functions were added they are called whenever the operator () of the
		transition object is called.
		It is possible to derive from this class in order to customize its behaviour
		or provide different or additional functionality.
		One event is provided, the event_execute event. It can be used to attach
		various kinds of handlers, e.g. handlers scripted with Lua.
	*/
	class YAPP_BASE_API Transition : public clonableT<Transition>
	{
		YAKE_DECLARE_CLONABLE( Transition, Transition );
	protected:
		Transition(const Transition& rkOther) : mTransitionFns(rkOther.mTransitionFns)
		{}
	public:
		Transition() {}
		Transition( const TransitionFn & fn0 )
			{
				mTransitionFns.push_back( fn0 ); 
			}
		Transition( const TransitionFn & fn0,
					const TransitionFn & fn1 )
			{ 
				mTransitionFns.push_back( fn0 );
				mTransitionFns.push_back( fn1 );
			}
		Transition( const TransitionFn & fn0,
					const TransitionFn & fn1,
					const TransitionFn & fn2)
			{ 
				mTransitionFns.push_back( fn0 );
				mTransitionFns.push_back( fn1 );
				mTransitionFns.push_back( fn2 );
			}
		void operator()();
		void addTransitionFn( const TransitionFn& fn );

		TransitionFnList	mTransitionFns;

		//events
		//event_execute();
	};
	typedef Transition* TransitionPtr;
	typedef boost::shared_ptr<Transition> SharedTransitionPtr;

	//---------------------------------------------------------
	/** A stacked state machine.
		States and transitions must be added to a machine before they can be used.
		The type of state ids can be chosen as a template parameter.
		If a transition between states shall be successfull the corresponding transition
		object has to exist, i.e. one has to add it to the machine before use.
		Possible entry states can be defined by provding transitions from kStateNone to
		a specific state.
		When changing a state with changeTo() one can choose whether to exit the previously
		active state and enter the new one or whether to pop the new state on the stack.
		The top-most state in the stack (i.e. the one last changed to) is the active one.
		If the machine's executeState() function is called the active's state step() method
		will be called.
		When a machine is destroyed, the stack will be cleared by removing the states one
		by one and calling their exit() methods (beginning at the top of the stack, i.e. the
		active state).
	*/
	template< typename StateIdType >
	class Machine : public clonableT<Machine>
	{
		YAKE_DECLARE_CLONABLE( Machine, Machine );
	protected:
		Machine(const Machine& rkOther) :
				mStates(rkOther.mStates),
				mStack(rkOther.mStack), //@todo copy stack, too?
				mTransitions(rkOther.mTransitions)
		{}
	public:
		Machine() {}
		virtual ~Machine()
		{ exitAll(); }

		typedef StateIdType StateId;

		typedef boost::shared_ptr<State> SharedStatePtr;
		typedef std::map< StateId, SharedStatePtr > StateMap;
		typedef std::pair<StateId, SharedStatePtr> IdStatePair;
		typedef std::deque< IdStatePair > StateStack;
		typedef std::pair<StateId,StateId> StateIdPair;
		typedef std::map< StateIdPair, SharedTransitionPtr > TransitionMap;

		StepSignal event_step;
		const static StateId kStateNone;

		ErrorCode addState( const StateIdType & rkId, SharedStatePtr & rState );
		ErrorCode addState( const StateIdType & rkId, StatePtr rState );
		ErrorCode addTransition( const StateId & rkFrom, const StateId & rkTo );
		ErrorCode addTransition( const StateId & rkFrom, const StateId & rkTo, SharedTransitionPtr & rTransition );
		ErrorCode addTransition( const StateId & rkFrom, const StateId & rkTo, TransitionPtr rTransition );
		ErrorCode changeTo( const StateIdType & rkId, bool bStacked = false );
		ErrorCode exitAll();
		ErrorCode exitTopOnStack();
		ErrorCode step();
		void dump(std::ostream & s) const;

	private:
		StateMap		mStates;
		StateStack		mStack;
		TransitionMap	mTransitions;

		SharedStatePtr _getStateById( const StateId & rkId )
		{
			StateMap::const_iterator itFind = mStates.find( rkId );
			if (itFind == mStates.end())
				return SharedStatePtr();
			return itFind->second;
		}
		bool _getStateById( const StateId & rkId, IdStatePair & statePair )
		{
			StateMap::const_iterator itFind = mStates.find( rkId );
			if (itFind == mStates.end())
				return false;
			statePair = *itFind;
			return true;
		}
		bool _getTransition( const StateIdPair & idPair, SharedTransitionPtr & rTransition )
		{
			TransitionMap::const_iterator itFind = mTransitions.find( idPair );
			if (itFind == mTransitions.end())
				return SharedTransitionPtr();
			return itFind->second;
		}
	};

	template<typename StateIdType>
		std::ostream& operator << (std::ostream& lhs, const state::Machine<StateIdType> & m)
	{
		m.dump( lhs );
		return lhs;
	}

#include "yakeFiniteStateMachine_inc.h"
	//---------------------------------------------------------

} // state
} // app
} // yake

#endif