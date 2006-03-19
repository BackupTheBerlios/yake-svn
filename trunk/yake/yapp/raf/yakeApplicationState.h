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
#ifndef YAKE_RAF_APPLICATIONSTATE_H
#define YAKE_RAF_APPLICATIONSTATE_H

#include "yapp/raf/yakePrerequisites.h"
#include "yake/statemachine/fsm_core.h"

namespace yake {
namespace raf {
	class Application;

	/** Application states of the application state machine are represented by derived
		classes of this type.
	*/
	class YAKE_RAF_API ApplicationState
	{
		ApplicationState();
	public:
		ApplicationState(Application& owner);
		virtual ~ApplicationState();

		Application& getApp();
		const Application& getApp() const;

		virtual void onEnter() {}
		virtual void onStep() {}
		virtual void onExit() {}
	private:
		Application&	mOwner;
	};
	typedef ApplicationState* ApplicationStatePtr;

	class YAKE_RAF_API AppMachine
	{
	private:
		AppMachine(const AppMachine&);
		AppMachine& operator=(const AppMachine&);
	public:
		AppMachine();
		~AppMachine();

		typedef String event_type;
		void addState(const String&, ApplicationState*);
		void setState(const String&);
		void addTransition(const String&, const event_type&, const String&);
		void processEvent(const event_type&);
		//const String& currentName() const;

		void step();
	private:
		typedef fsm::oomachine<ApplicationState,String> fsm_type;
		fsm_type						machine_;

		typedef AssocVector<String,SharedPtr<ApplicationState> > state_map;
		state_map					states_;
	};

	/** Main state base class */
	class YAKE_RAF_API MainState : public ApplicationState
	{
		MainState();
	public:
		MainState(Application& owner);

		virtual void onEnter();
		virtual void onStep();
		virtual void onExit();
	};

} // namespace raf
} // namespace yake

namespace fsm {
	template <>
	inline const yake::raf::ApplicationStatePtr& get_null_state<yake::raf::ApplicationState*>()
	{
		static yake::raf::ApplicationStatePtr ms_null = 0;
		return ms_null;
	}
}


#endif
