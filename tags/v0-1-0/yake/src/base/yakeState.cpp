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
*/
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <inc/base/yakePCH.h>
#include <inc/base/yakeState.h>
#include <inc/base/templates/yakeSmartAssert.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

	//------------------------------------------------------
	// Class definitons in this file:
	//
	//    State
	//    StateManager
	//
	//------------------------------------------------------

	//------------------------------------------------------
	// State
	//------------------------------------------------------

	//------------------------------------------------------
	State::State( const String & name ) : mName( name ), mParentState( 0 ), mActiveState( 0 )
	{
	}

	//------------------------------------------------------
	State::~State()
	{
		for (StateList::iterator it = mSubStates.begin(); it != mSubStates.end(); ++it)
			if (it->second)
				delete it->second;
		mSubStates.clear();

		//leave();
	}

	//------------------------------------------------------
	void State::setParentState( State * parent )
	{
		mParentState = parent;
	}

	//------------------------------------------------------
	State* State::getParentState() const
	{
		return mParentState;
	}

	//------------------------------------------------------
	String State::getName() const
	{
		return mName;
	}

	//------------------------------------------------------
	void State::addSubState( State * pState )
	{
		YAKE_ASSERT( pState ).debug("Trying to add a 0-state.");
		if (!pState)
			return;

		YAKE_ASSERT( pState->getName().length() > 0 ).debug("State needs a name!");
		if (pState->getName().length() == 0)
			return;

		// avoid duplicate entries
		for (StateList::const_iterator it = mSubStates.begin(); it != mSubStates.end(); ++it)
		{
			if (it->second->getName() == pState->getName())
			{
				YAKE_ASSERT( 1==0 )( pState->getName() ).debug("There is already a substate with the same name!");
				return;
			}
		}

		pState->setParentState( this );

		mSubStates[ pState->getName() ] = pState;
	}

	//------------------------------------------------------
	State* State::getSubState( const String & name ) const
	{
		StateList::const_iterator itFind = mSubStates.find( name );
		if (itFind == mSubStates.end())
		{
			YAKE_ASSERT( 1==0 )( name ).warning("State not in substate list!");
			return 0;
		}
		return itFind->second;
	}

	//------------------------------------------------------
	State* State::getActiveSubState() const
	{
		return mActiveState;
	}

	//------------------------------------------------------
	bool State::switchToSubState( const String & name )
	{
		if (mActiveState)
			mActiveState->leave();

		State* oldState = mActiveState;
		mActiveState = getSubState( name );
		YAKE_ASSERT( mActiveState )( name ).debug("Cannot switch to state because it isn't in the substate list!");

		if (!mActiveState)
			return false;
			
		mActiveState->enter();
		return true;
	}

	//------------------------------------------------------
	// StateManager
	//------------------------------------------------------

	//------------------------------------------------------
	StateManager::StateManager() : mRootState( 0 )
	{
	}

	//------------------------------------------------------
	StateManager::~StateManager()
	{
		YAKE_SAFE_DELETE( mRootState ); // NB this will trigger the root state's leave() method (in its d'tor).
	}

	//------------------------------------------------------
	void StateManager::setRootState( State* pRoot )
	{
		if (mRootState == pRoot)
			return;
		YAKE_SAFE_DELETE( mRootState ); //NB The state's d'tor will trigger a call to its leave() method.
		YAKE_ASSERT( pRoot ).warning("Setting root state to 0. Is this intentional ?");
		mRootState = pRoot;
		if (mRootState)
			mRootState->enter();
	}

	//------------------------------------------------------
	State* StateManager::getRootState() const
	{
		return mRootState;
	}

} // base
} // yake