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
//---------------------------------------------------------
// NOTE: Do not include this file directly!
//---------------------------------------------------------
template<typename StateIdType>
StateMachine<StateIdType>::StateMachine() : mCurrentStateId( kStateNone ), mTransitionInProgress(false)
{
}
template<typename StateIdType>
bool StateMachine<StateIdType>::_stateExists( const StateId & id ) const
{
	return (mStates.find( id ) != mStates.end());
}
template<typename StateIdType>
bool StateMachine<StateIdType>::_executeState( const StateId & id )
{
	StateFnMap::const_iterator it = mStates.find( id );
	if (it == mStates.end())
		return false;
	it->second();
	return true;
}
template<typename StateIdType>
StateIdType StateMachine<StateIdType>::getCurrentState() const
{
	return mCurrentStateId;
}
template<typename StateIdType>
void StateMachine<StateIdType>::regState( const StateId & id, const StateFn & fn )
{
	//if (_stateExists(id))
	//	return;
	mStates[ id ] = fn;
}
template<typename StateIdType>
void StateMachine<StateIdType>::unregState( const StateId & id )
{
	// @todo handle mCurrentStateId == id

	// remove state
	StateFnMap::iterator it = mStates.find( id );
	if (it != mStates.end())
		mStates.erase( it );

	// @todo remove all transitions!
}
template<typename StateIdType>
void StateMachine<StateIdType>::regTransition( const StateId & idFrom, const StateId & idTo, const TransitionFnList & fns )
{
	// special case: idFrom == kStateNone is valid even though kStateNone may not be a registered state!
	if (idFrom != kStateNone && !_stateExists(idFrom))
		return; // @todo report error
	if (!_stateExists(idTo))
		return; // @todo report error
	StateIdPair pair = std::make_pair(idFrom,idTo);
	TransitionMap::const_iterator itFind = mTransitions.find( pair );
	if (itFind != mTransitions.end())
		mTransitions[ pair ] = fns;
	else
		mTransitions.insert( std::make_pair(pair, fns) );
}
template<typename StateIdType>
void StateMachine<StateIdType>::regTransition( const StateId & idFrom, const StateId & idTo )
{
	TransitionFnList fns;
	regTransition( idFrom, idTo, fns );
}
template<typename StateIdType>
void StateMachine<StateIdType>::regTransition( const StateId & idFrom, const StateId & idTo, const TransitionFn & fn )
{
	TransitionFnList fns;
	fns.push_back( fn );
	regTransition( idFrom, idTo, fns );
}
template<typename StateIdType>
void StateMachine<StateIdType>::addTransitionFn( const StateId & idFrom, const StateId & idTo, const TransitionFn & fn )
{
	if (!_stateExists(idFrom) || !_stateExists(idTo))
		return; // @todo report error
	StateIdPair pair = std::make_pair(idFrom,idTo);
	TransitionMap::iterator itFind = mTransitions.find( pair );
	if (itFind == mTransitions.end())
		return; // @todo report error
	itFind->second.push_back( fn );
}
template<typename StateIdType>
bool StateMachine<StateIdType>::_getTransition( const StateIdPair & idpair, TransitionFnList & fns ) const
{
	TransitionMap::const_iterator itFind = mTransitions.find( idpair );
	if (itFind == mTransitions.end())
		return false; // @todo report error?
	fns = itFind->second;
	return true;
}
template<typename StateIdType>
ErrorCode StateMachine<StateIdType>::changeStateTo( const StateId & idTo )
{
	if (mTransitionInProgress)
		return eInvalidNesting;

	mTransitionInProgress = true;
	const StateId idFrom = mCurrentStateId;

	// transition between registered states
	if (!_stateExists(idTo))
	{
		mTransitionInProgress = false;
		return eInvalidParam; // @todo report error
	}
	if (idFrom != kStateNone && !_stateExists(idFrom))
	{
		mTransitionInProgress = false;
		return eInvalidParam; // @todo report error
	}
	StateIdPair pair = std::make_pair(idFrom,idTo);
	TransitionFnList fns;
	bool bTransitionRegistered = _getTransition( pair, fns );

	// execute transition if possible
	if (bTransitionRegistered)
	{
		ConstDequeIterator<TransitionFnList> it( fns.begin(), fns.end() );
		while (it.hasMoreElements())
			it.getNext()(); // call function
	}
	else
	{
		mTransitionInProgress = false;
		return eRejected;
	}

	// call state function
	mCurrentStateId = idTo;
	if (!_executeState( mCurrentStateId ))
	{
		mTransitionInProgress = false;
		return eError; // @todo report error
	}
	mTransitionInProgress = false;
	return eOk;
}
template<typename StateIdType>
void StateMachine<StateIdType>::dump(std::ostream& rhs) const
{
	rhs << "STATE MACHINE:" << std::endl;

	// states
	rhs << "states:" << std::endl;
	templates::ConstVectorIterator< StateFnMap > itState( mStates.begin(), mStates.end() );
	while (itState.hasMoreElements())
	{
		const std::pair<StateId, StateFn> & state = itState.getNext();
		rhs << "   " << state.first << std::endl;
	}

	// transitions
	rhs << "transitions:" << std::endl;
	templates::ConstVectorIterator< TransitionMap > itTrans( mTransitions.begin(), mTransitions.end() );
	while (itTrans.hasMoreElements())
	{
		const std::pair< StateIdPair, TransitionFnList > & trans = itTrans.getNext();
		rhs << "   from " << trans.first.first << " to " << trans.first.second << " (" << int(trans.second.size()) << " calls)" << std::endl;
	}

	rhs << std::endl;
}
