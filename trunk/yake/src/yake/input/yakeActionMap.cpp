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
#include <yake/input/yakePCH.h>
#include <yake/input/yakeInputSystem.h>
#include <yake/input/yakeActionMap.h>

namespace yake {
namespace input {

	const ActionId ACTIONID_FORWARD(1, "forward");
	const ActionId ACTIONID_REVERSE(2, "reverse");
	const ActionId ACTIONID_STRAFE_LEFT(3, "strafe_left");

	const ActionId ACTIONID_USER(10000, "user");

	//-----------------------------------------------------
	// KeyboardActionCondition
	//-----------------------------------------------------

	//-----------------------------------------------------
	KeyboardActionCondition::KeyboardActionCondition(
		KeyboardDevice* device, const KeyCode key, KeyboardActionMode mode) :
						mKeyb(device),
						mKey(key),
						mMode(mode),
						mLastState(false)
	{
	}

	//-----------------------------------------------------
	void KeyboardActionCondition::setKeyboard( KeyboardDevice* device )
	{
		mKeyb = device;
	}

	//-----------------------------------------------------
	void KeyboardActionCondition::setKey( const KeyCode key )
	{ 
		mKey = key;
	}

	//-----------------------------------------------------
	void KeyboardActionCondition::setMode( KeyboardActionMode mode )
	{ 
		mMode = mode;
	}

	//-----------------------------------------------------
	bool KeyboardActionCondition::operator()()
	{
		if (!mKeyb)
			return false;
		if (mMode == KAM_CONTINUOUS)
			return mKeyb->isKeyDown( mKey );
		else
		{
			bool state = mKeyb->isKeyDown( mKey );
			if (state != mLastState)
			{
				mLastState = state;
				if (mMode == KAM_PRESSED_AND_RELEASED)
					return true;
				else
				{
					// fire only if key is 'pressed'
					if (mLastState && mMode == KAM_PRESSED)
						return true;
					else if (!mLastState && mMode == KAM_RELEASED)
						return true;
					else
						return false;
				}
			}
			else
				return false;
		}
	}

	//-----------------------------------------------------
	// ActionMap
	//-----------------------------------------------------

	//-----------------------------------------------------
	ActionMap::ActionMap()
	{
	}

	//-----------------------------------------------------
	ActionMap::~ActionMap()
	{
		for (ActionList::const_iterator it = mEntries.begin(); it != mEntries.end(); ++it)
		{
			ActionMapEntry* pEntry = (*it);
			YAKE_ASSERT( pEntry );
			YAKE_SAFE_DELETE( pEntry->condition );
			delete pEntry;
		}
		mEntries.clear();
	}

	//-----------------------------------------------------
	void ActionMap::reg( const ActionId & actionId, ActionCondition* condition )
	{
		YAKE_ASSERT( condition );
		ActionMapEntry* entry = new ActionMapEntry;
		entry->condition = condition;
		entry->actionId  = actionId;
		mEntries.push_back( entry );
	}

	//-----------------------------------------------------
	void ActionMap::subscribeToActionId( const ActionId & actionId, const ActionSignal::slot_type & slot )
	{
		for (ActionList::const_iterator it = mEntries.begin(); it != mEntries.end(); ++it)
		{
			ActionMapEntry* pEntry = (*it);
			YAKE_ASSERT( pEntry );
			if (pEntry->actionId == actionId)
			{
				pEntry->signal.connect( slot );
				return;
			}
		}
	}

	//-----------------------------------------------------
	void ActionMap::update()
	{
		for (ActionList::const_iterator it = mEntries.begin(); it != mEntries.end(); ++it)
		{
			ActionMapEntry* pEntry = (*it);
			YAKE_ASSERT( pEntry );
			YAKE_ASSERT( pEntry->condition );

			if ( (*pEntry->condition)() )
				pEntry->signal();
		}
	}
}
}
