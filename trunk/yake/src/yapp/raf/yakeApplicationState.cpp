/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) 2004 The YAKE Team
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
#include <yapp/raf/pch.h>
#include <yapp/raf/yakeApplicationState.h>

namespace yake {
namespace raf {

	//-----------------------------------------------------
	// Class: ApplicationState
	//-----------------------------------------------------
	ApplicationState::ApplicationState(Application& owner) :
		mOwner(owner)
	{
	}
	ApplicationState::~ApplicationState()
	{
	}
	Application& ApplicationState::getApp()
	{
		return mOwner;
	}
	const Application& ApplicationState::getApp() const
	{
		return mOwner;
	}

	//-----------------------------------------------------
	// Class: MainState
	//-----------------------------------------------------
	MainState::MainState(Application& owner) :
		ApplicationState(owner)
	{
	}
	void MainState::onEnter()
	{
		YAKE_LOG_INFORMATION("MainState::onEnter()");
	}
	void MainState::onExit()
	{
		YAKE_LOG_INFORMATION("MainState::onExit()");
	}
	void MainState::onStep()
	{
		YAKE_LOG_INFORMATION("MainState::onStep()");
	}

	//-----------------------------------------------------
	// Class: AppMachine
	//-----------------------------------------------------
	AppMachine::AppMachine()
	{}
	AppMachine::~AppMachine()
	{}
	void AppMachine::addState(const String& stateId, ApplicationState* state)
	{
		YAKE_ASSERT( !stateId.empty() );
		YAKE_ASSERT( state );
		YAKE_ASSERT( states_.end() == states_.find(stateId) );
		states_.insert( state_map::value_type(stateId,SharedPtr<ApplicationState>(state)) );
		machine_.addState( state );
	}
	void AppMachine::setState(const String& stateId)
	{
		state_map::const_iterator it = states_.find(stateId);
		YAKE_ASSERT( states_.end() != it );
		machine_.setState( it->second.get() );
	}
	void AppMachine::addTransition(const String& from, const event_type& evt, const String& to)
	{
		state_map::const_iterator itFrom = states_.find(from);
		YAKE_ASSERT( states_.end() != itFrom );
		state_map::const_iterator itTo = states_.find(to);
		YAKE_ASSERT( states_.end() != itTo );
		machine_.addTransition( itFrom->second.get(), evt, itTo->second.get() );
	}
	void AppMachine::processEvent(const event_type& evt)
	{
		machine_.processEvent( evt );
	}
	void AppMachine::step()
	{
		YAKE_ASSERT( machine_.current() );
		machine_.current()->onStep();
	}

} // namespace raf
} // namespace yake
