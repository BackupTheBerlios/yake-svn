/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
//---------------------------------------------------------
// NOTE: Do not include this file directly!
//---------------------------------------------------------
	//---------------------------------------------------------
	// State
	//---------------------------------------------------------

#if YAKE_COMPILER == COMPILER_MSVC
	template<> const String Machine<String>::kStateNone = "NONE";
#else
	template<> const String Machine<String>::kStateNone;
#endif

	//---------------------------------------------------------
	template< typename StateIdType >
		Machine<StateIdType>::Machine( const TransitionHandling th ) :
			mTransitionHandling(th)
	{
	}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::addTransition( const StateId& rkFrom, const StateId& rkTo )
		{
			const StateIdPair idPair( rkFrom, rkTo );
			SharedTransitionPtr pTmpTrans;
			if (_getTransition( idPair, pTmpTrans ))
				return eAlreadyRegistered; // already registered.
			mTransitions[ idPair ] = SharedTransitionPtr();
			return eOk;
		}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::addTransition( const StateId & rkFrom, const StateId & rkTo, SharedTransitionPtr & rTransition )
		{
			const StateIdPair idPair( rkFrom, rkTo );
			if (_hasTransition( idPair ))
				return eAlreadyRegistered; // already registered.
			mTransitions[ idPair ] = rTransition;
			return eOk;
		}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::addTransition( const StateId & rkFrom, const StateId & rkTo, TransitionPtr rTransition )
		{
			const StateIdPair idPair( rkFrom, rkTo );
			if (_hasTransition( idPair ))
				return eAlreadyRegistered; // already registered.
			mTransitions[ idPair ] = SharedTransitionPtr(rTransition);
			return eOk;
		}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::exitAll()
		{
			while (mStack.size() > 0)
			{
				mStack.front().second->exit();
				mStack.pop_front();
			}
			return eOk;
		}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::exitTopOnStack()
		{
			if (mStack.size() > 0)
			{
				mStack.front().second->exit();
				mStack.pop_front();
			}
			return eOk;
		}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::addState( const StateIdType & rkId, SharedStatePtr & rState )
	{
		if (_getStateById( rkId ).get())
			return eAlreadyRegistered;
		mStates[ rkId ] = rState;
		return eOk;
	}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::addState( const StateIdType & rkId, StatePtr rState )
	{
		if (_getStateById( rkId ).get())
			return eAlreadyRegistered;
		mStates[ rkId ] = SharedStatePtr(rState);
		return eOk;
	}
	template< typename StateIdType >
		ErrorCode Machine<StateIdType>::changeTo( const StateIdType & rkId, bool bStacked )
	{
		StateId currentStateId = (mStack.size() > 0) ? (mStack.front().first) : kStateNone;

		// find transition
		SharedTransitionPtr pTrans;
		if (!_getTransition( StateIdPair( currentStateId, rkId ), pTrans ) ) // transition not found
		{
			if (mTransitionHandling == yake::state::TH_SCRICT)
				return eTransitionNotFound;
		}

		// find state
		IdStatePair state;
		if (false == _getStateById(rkId, state))
			return eStateNotFound; // state not found

		// change state
		if (!bStacked)
			exitTopOnStack(); // this is safe even if no state is on the stack yet.

		if (pTrans.get()) // transitions without a transition object is fine!
			(*pTrans)();
		mStack.push_front( state );
		mStack.front().second->enter();

		return eOk;
	}
	template< typename StateIdType >
	ErrorCode Machine<StateIdType>::step()
	{
		event_step();
		if (mStack.size() > 0)
		{
			SharedStatePtr& pState = mStack.front().second;
			if (pState.get())
			{
				pState->step();
			}
		}
		return eOk;
	}
	template< typename StateIdType >
	void Machine<StateIdType>::dump(std::ostream & s) const
	{
		s << "FSM:" << std::endl;

		s << "\t" << "id of 'none' state: " << kStateNone << std::endl;

		s << "\t" << ( unsigned int )mStates.size() << " registered states" << std::endl;
		::yake::ConstVectorIterator<StateMap> itState( mStates.begin(), mStates.end() );
		
		while (itState.hasMoreElements())
		{
			s << "\t\t" << itState.getNext().first << std::endl;
		}

		s << "\t" << ( unsigned int )mTransitions.size() << " registered transitions" << std::endl;
		::yake::ConstVectorIterator<TransitionMap> itTrans( mTransitions.begin(), mTransitions.end() );
		
		while (itTrans.hasMoreElements())
		{
			const StateIdPair & idPair = itTrans.getNext().first;
			s << "\t\t" << idPair.first << " -> " << idPair.second << std::endl;
		}

		s << "\t" << ( unsigned int )mStack.size() << " states on the stack" << std::endl;
		if (mStack.size() > 0)
		{
			s << "\t\t(from top to bottom:)" << std::endl;
			::yake::ConstVectorIterator<StateStack> itStack( mStack.begin(), mStack.end() );
			while (itStack.hasMoreElements())
			{
				s << "\t\t" << itStack.getNext().first << std::endl;
			}
		}
	}
