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
#ifndef YAKE_BASE_STATE_H
#define YAKE_BASE_STATE_H

//todo: kick manager

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
// Yake
#include <yake/base/yakeString.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

//------------------------------------------------------
// Class declarations for:
//
//    State
//    StateManager
//
//------------------------------------------------------

//------------------------------------------------------
/** Represents a state in a state hierarchy.
	Derived classes have to implement code for entering and leaving
	the state.
	A state may have substates. Only one of the substates may be active
	at any time. A state owns its substates!
	\see StateManager
	\see UpdatedState
	\see UpdatedStateManager
*/


class YAKE_BASE_API State
{
	State();
private:
	State			* mParentState;
	State			* mActiveState;
protected:
	String		mName;
	typedef HashMap< String, State*, _StringHash > StateList;
	StateList	mSubStates;

	State( const String & name );

	void setParentState( State* parent );
	State* getParentState() const;
public:
	virtual ~State();

	/** Get the state's name.
		\return name of the sate
	*/
	String getName() const;

	/** The enter() and leave() methods have to be overridden by concrete
		State implementations.
		\see leave()
	*/
	virtual void enter() = 0;

	/** The enter() and leave() methods have to be overridden by concrete
		State implementations.
		\see enter()
	*/
	virtual void leave() = 0;

	/** Add a substate to this state. Ownership is transferred to this state!
		That means that the sub-state will be destroyed when this state is destroyed.
	*/
	void addSubState( State* pState );

	/** Return a sub-state by its name.
		\param name Name of the sub-state to return a pointer to.
		\return a pointer to a state or 0 if it couldn't be found.
	*/
	State* getSubState( const String & name ) const;

	/** Switches to a different sub-state. If there's a sub-state active its
		leave() method is called. The sub-state referenced by name is made active
		by calling its enter() method.
		\remark If the sub-state referenced by the parameter name is already the active one
		its leave() and enter() methods are called nevertheless!
		\param name name of the state to switch to
		\return true if sub-state switching was successfull. false if there was an error
			(typical error: the substate referenced by the parameter name couldn't be found).
	*/
	bool switchToSubState( const String & name );

	/** Returns a pointer to the active sub-state. Ownership stays with this state.
		\return a pointer to the active state or 0 if there isn't one.
	*/
	State* getActiveSubState() const;
};

//------------------------------------------------------
/** A state manager.
	\see State
	\see UpdatedState
*/
class YAKE_BASE_API StateManager
{
private:
	State		* mRootState;
public:
	StateManager();
	virtual ~StateManager();

	/** Set the root state of the state manager. This state is always active!
		If you set the root node to 0 or to a new state object, the old root
		will be destroyed (thereby calling its leave() method).
		If you set the root state to the same state again, i.e. you pass in a pointer
		to the same object again, nothing will happen!
	*/
	void setRootState( State* pState );

	/** Return a pointer to the root state. Ownership is not transferred.
	*/
	State* getRootState() const;
};

//------------------------------------------------------
/** A state that can be updated via an update() method which
	takes a templated type as the parameter type.
	\see State
	\see UpdatedStateManager
*/
template < typename T >
class UpdatedState : public State
{
public:
	virtual void update( const T & upd ) = 0;
};

//------------------------------------------------------
/** Manages updatable states.
	\see StateManager
	\see State
*/
template < typename T >
class UpdatedStateManager : public StateManager
{
public:
	virtual void update( const T & upd )
	{
		for( StateList::iterator it = mSubStates.begin(); it != mSubStates.end(); ++it )
		{
			YAKE_ASSERT( it->second ).error( "Encountered 0 pointer in list!" );
			it->second->update( upd );
		}
	}
};

//------------------------------------------------------
//typedef UpdatedState< FrameUpdate > YAKE_BASE_API ApplicationState; // todo
//typedef UpdatedStateManager< FrameUpdate > YAKE_BASE_API ApplicationStateManager;

} // base
} // yake

#endif // YAKE_BASE_STATE_H
