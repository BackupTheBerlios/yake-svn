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
#include <yapp/base/yappPCH.h>
#include <yapp/statemachine/yakeFiniteStateMachine.h>

namespace yake {
namespace app {
namespace state {

#if YAKE_COMPILER == COMPILER_GNUC
	template<> const String Machine<String>::kStateNone = "NONE";
#endif

	//---------------------------------------------------------
	// State
	//---------------------------------------------------------
	void State::enter()
	{
		event_enter();
		onEnter();
	}
	void State::step()
	{
		event_step();
		onStep();
	}
	void State::exit()
	{
		event_exit();
		onExit();
	}

	//---------------------------------------------------------
	// Transition
	//---------------------------------------------------------
	void Transition::operator()()
	{
		ConstVectorIterator<TransitionFnList> it(mTransitionFns.begin(), mTransitionFns.end());
		while (it.hasMoreElements())
			it.getNext()();
	}
	void Transition::addTransitionFn( const TransitionFn& fn )
	{
		mTransitionFns.push_back( fn );
	}

}
}
}