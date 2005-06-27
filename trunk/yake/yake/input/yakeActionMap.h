/*
   ------------------------------------------------------------------------------------
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
#ifndef YAKE_INPUT_ACTIONMAP_H
#define YAKE_INPUT_ACTIONMAP_H

#include <yake/input/yakeInput.h>

namespace yake {

using namespace base;

namespace input {


	//typedef uint32 ActionId;
	class YAKE_INPUT_API ActionId
	{
	public:
		ActionId( uint32 id, const String & name ) : mId(id), mName(name)
		{
		}
		bool operator == (const ActionId & rhs)
		{
			return mId == rhs.mId;
		}
		operator uint32()
		{
			return mId;
		}
		uint32 id() const
		{ return mId; }
	private:
		uint32			mId;
		String	mName;
	};
	inline uint32 operator + (const ActionId & lhs, const uint32 rhs)
	{
		return lhs.id() + rhs;
	}

	// default action ids ( 0..9999 )
	extern YAKE_INPUT_API const ActionId ACTIONID_FORWARD;
	extern YAKE_INPUT_API const ActionId ACTIONID_REVERSE;
	extern YAKE_INPUT_API const ActionId ACTIONID_STRAFE_LEFT;
	extern YAKE_INPUT_API const ActionId ACTIONID_USER;
	// example for a user defined action id:
	//const ActionId ACTIONID_USER_CheerWildly( ACTIONID_USER+1, "cheerwildly" );


	/** Base class for action conditions.
		Action conditions decide when to trigger action signals by returning
		true in operator().
	*/
	class YAKE_INPUT_API ActionCondition
	{
	public:
		virtual ~ActionCondition() {}

		virtual bool operator()() = 0;
	};

	enum KeyboardActionMode {
		KAM_CONTINUOUS,
		KAM_PRESSED,
		KAM_RELEASED,
		KAM_PRESSED_AND_RELEASED
	};
	/** KeyboardActionCondition checks key presses/releases in order to decide when
		to trigger action signals.
	*/
	class YAKE_INPUT_API KeyboardActionCondition : public ActionCondition
	{
	public:
		KeyboardActionCondition( KeyboardDevice* device, const KeyCode key, KeyboardActionMode mode = KAM_CONTINUOUS );	

		void setKeyboard( KeyboardDevice* device );
		void setKey( const KeyCode key );

		/** set whether a "key down" event will trigger a single signal
			or a continuous stream of signals until a corresponding "key up"
			is encountered.
		*/
		void setMode( KeyboardActionMode mode );

		virtual bool operator()();
	private:
		KeyboardDevice*		mKeyb;
		KeyCode				mKey;
		bool				mContinuous;
		bool				mLastState;
		KeyboardActionMode	mMode;
	};

	class YAKE_INPUT_API MouseActionCondition : public ActionCondition
	{
	public:
		MouseActionCondition( MouseDevice* device );
		void setMouse( MouseDevice* device );
	protected:
		MouseDevice*	mMouse;
	};

	class YAKE_INPUT_API MouseButtonActionCondition : public MouseActionCondition
	{
	public:
		MouseButtonActionCondition( MouseDevice* device, uint8 button = 0, bool continous = false );
		void setButton( uint8 button );
		void setContinuous( bool continous = false );
		virtual bool operator()()
		{
			return mMouse->isButtonDown( mButton );
		}
	private:
		uint8	mButton;
	};

	/** An ActionMap object can handly any number of ActionCondition/ActionId pairs.
		The update() method has to be regularly called.
		If an action should be run an ActionSignal is fired which will inform all
		subscribers of the action.
	*/
	class YAKE_INPUT_API ActionMap
	{
	public:
		ActionMap();
		~ActionMap();

		typedef Signal1< void(void) > ActionSignal;
		void reg( const ActionId & actionId, ActionCondition* condition );
		void subscribeToActionId( const ActionId & actionId, const ActionSignal::slot_type & slot );
		void update();
	private:
		typedef Vector< ActionId > ActionIdList;
		struct ActionMapEntry {
			ActionMapEntry() : condition(0), actionId(ACTIONID_USER)
			{
			}
			ActionCondition*	condition;
			ActionId			actionId;
			ActionSignal		signal;
		};
		typedef Vector< ActionMapEntry* > ActionList;
		ActionList	mEntries;
	};

	// utility, often needed for configuration files.
	//class ActionIdNameMap

	/** The ActionConditionFactory is utility factory class.
		While not essential to the core functionality of action maps it can be useful for
		easy creation of conditions.
	*/
	class YAKE_INPUT_API ActionConditionFactory
	{
	public:
		void setDefaultKeyboard( KeyboardDevice* device );
		void setDefaultMouse( KeyboardDevice* device );

		ActionCondition* createKeyUpCondition( const KeyCode key, bool continuous, KeyboardDevice* device = 0 );
		ActionCondition* createKeyDownCondition( const KeyCode key, bool continuous, KeyboardDevice* device = 0 );

		ActionCondition* createMouseButtonUpCondition( const KeyCode key, bool continuous, MouseDevice* device = 0 );
		ActionCondition* createMouseButtonDownCondition( const KeyCode key, bool continuous, MouseDevice* device = 0 );
	};

}
}

#endif
