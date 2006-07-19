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
#include <yake/input/yakePCH.h>
#include <yake/input/yakeInput.h>
#include <fstream>

namespace yake {
namespace input {

	const ActionId ACTIONID_NONE(0, "none");
	const ActionId ACTIONID_FORWARD(1, "forward");
	const ActionId ACTIONID_REVERSE(2, "reverse");
	const ActionId ACTIONID_STRAFE_LEFT(3, "strafe_left");
	const ActionId ACTIONID_STRAFE_RIGHT(4, "strafe_right");
	const ActionId ACTIONID_UP(5, "up");
	const ActionId ACTIONID_DOWN(6, "down");
	const ActionId ACTIONID_LEFT(7, "left");
	const ActionId ACTIONID_RIGHT(8, "right");
	const ActionId ACTIONID_ROTATE_LEFT(9, "rotate_left");
	const ActionId ACTIONID_ROTATE_RIGHT(10, "rotate_right");
	const ActionId ACTIONID_USE(11, "use");
	const ActionId ACTIONID_FIRE1(12, "fire1");
	const ActionId ACTIONID_FIRE2(13, "fire2");

	const ActionId ACTIONID_USER(10000, "user");

	//-----------------------------------------------------
	// Static helper functions
	//-----------------------------------------------------

	namespace detail {
		typedef std::map<String,ActionId::IdType> StringToIdMap;
		typedef std::map<ActionId::IdType,String> IdToStringMap;
		static StringToIdMap& getS2IdMap()
		{
			static StringToIdMap	g_s2id;
			return g_s2id;
		}
		static IdToStringMap& getId2SMap()
		{
			static IdToStringMap	g_id2s;
			return g_id2s;
		}
	}

	void ActionIdRegistry::reg(const ActionId& id)
	{
		const String n = id.getName();
		bool exists = getByName( n ) != ACTIONID_NONE;
		YAKE_ASSERT( !exists )( n ).debug("Action ID name already registered! Skipping!");
		if (exists)
			return;
		const ActionId::IdType i = id.id();
		exists = getById( i ) != ACTIONID_NONE;
		YAKE_ASSERT( !exists )( i ).debug("Action ID name already registered! Skipping!");
		if (exists)
			return;
		detail::getS2IdMap()[ n ] = i;
		detail::getId2SMap()[ i ] = n;
	}
	ActionId ActionIdRegistry::getByName(const String& name)
	{
		if (detail::getS2IdMap().empty())
			return ACTIONID_NONE;
		const detail::StringToIdMap::const_iterator itFind = detail::getS2IdMap().find( name );
		return (itFind != detail::getS2IdMap().end()) ? itFind->second : ACTIONID_NONE;
	}
	ActionId ActionIdRegistry::getById(const ActionId::IdType& id)
	{
		if (detail::getId2SMap().empty())
			return ACTIONID_NONE;
		const detail::IdToStringMap::const_iterator itFind = detail::getId2SMap().find( id );
		return (itFind != detail::getId2SMap().end()) ? getByName(itFind->second) : ACTIONID_NONE;
	}

	namespace detail {
		KeyCode toKeyCode( const String& s );
		String toString( const KeyCode& kc );

		typedef AssocVector<String,KeyCode> StringToKcMap;

		StringToKcMap g_s2kc;

#define REG_KEYBOARD_ACTIONCOND_CREATOR( NAME, KEYCODE ) \
	::yake::input::detail::g_s2kc[ NAME ] = KEYCODE;

		ActionCondition* createKeyboardActionCondition(const String& key, KeyboardActionMode kam, KeyboardDevice* pKeyboard)
		{
			YAKE_ASSERT( pKeyboard );
			if (!pKeyboard)
				return 0;
			if (g_s2kc.empty())
			{
				YAKE_ASSERT( 0 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_escape", KC_ESCAPE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_1", KC_1 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_2", KC_2 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_3", KC_3 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_4", KC_4 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_5", KC_5 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_6", KC_6 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_7", KC_7 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_8", KC_8 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_9", KC_9 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_0", KC_0 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_minus", KC_MINUS );    /* - on main keyboard */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_equals", KC_EQUALS );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_back", KC_BACK );    /* backspace */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_tab", KC_TAB );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_q", KC_Q );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_w", KC_W );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_e", KC_E );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_r", KC_R );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_t", KC_T );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_y", KC_Y );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_u", KC_U );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_i", KC_I );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_o", KC_O );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_p", KC_P );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_lbracket", KC_LBRACKET );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_rbracket", KC_RBRACKET );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_return", KC_RETURN );    /* Enter on main keyboard */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_lcontrol", KC_LCONTROL );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_a", KC_A );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_s", KC_S );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_d", KC_D );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f", KC_F );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_g", KC_G );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_h", KC_H );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_j", KC_J );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_k", KC_K );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_l", KC_L );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_semicolon", KC_SEMICOLON );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_apostrophe", KC_APOSTROPHE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_grave", KC_GRAVE );    /* accent grave */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_lshift", KC_LSHIFT );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_backslash", KC_BACKSLASH );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_z", KC_Z );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_x", KC_X );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_c", KC_C );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_v", KC_V );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_b", KC_B );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_n", KC_N );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_m", KC_M );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_comma", KC_COMMA );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_period", KC_PERIOD );   /* . on main keyboard */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_slash", KC_SLASH );    /* '/' on main keyboard */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_rshift", KC_RSHIFT );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_multiply", KC_MULTIPLY ); /* * on numeric keypad */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_lmenu", KC_LMENU );    /* left Alt */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_space", KC_SPACE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_capital", KC_CAPITAL );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f1", KC_F1 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f2", KC_F2 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f3", KC_F3 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f4", KC_F4 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f5", KC_F5 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f6", KC_F6 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f7", KC_F7 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f8", KC_F8 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f9", KC_F9 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f10", KC_F10 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numlock", KC_NUMLOCK );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_scrolllock", KC_SCROLL );    /* Scroll Lock */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_7", KC_NUMPAD7 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_8", KC_NUMPAD8 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_9", KC_NUMPAD9 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_subtract", KC_SUBTRACT );    /* - on numeric keypad */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_4", KC_NUMPAD4 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_5", KC_NUMPAD5 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_6", KC_NUMPAD6 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_add", KC_ADD );    /* + on numeric keypad */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_1", KC_NUMPAD1 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_2", KC_NUMPAD2 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_3", KC_NUMPAD3 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_0", KC_NUMPAD0 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_decimal", KC_DECIMAL );    /* . on numeric keypad */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_vertical", KC_OEM_102 );    /* < > | on UK/Germany keyboards */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f11", KC_F11 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f12", KC_F12 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f13", KC_F13 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f14", KC_F14 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_f15", KC_F15 );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_japan_kana", KC_KANA );    /* (Japanese keyboard)            */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_slash_portugese", KC_ABNT_C1 );    /* / ? on Portugese (Brazilian) keyboards */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_japan_convert", KC_CONVERT );    /* (Japanese keyboard)            */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_japan_noconvert", KC_NOCONVERT );    /* (Japanese keyboard)            */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_japan_yen", KC_YEN );    /* (Japanese keyboard)            */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_colon_portugese", KC_ABNT_C2 );    /* Numpad . on Portugese (Brazilian) keyboards */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_equals", KC_NUMPADEQUALS );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_prev_track", KC_PREVTRACK );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_at", KC_AT );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_colon", KC_COLON );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_underline", KC_UNDERLINE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_japan_kanji", KC_KANJI );    /* (Japanese keyboard)            */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_stop", KC_STOP );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_japan_ax", KC_AX );    /*                     (Japan AX) */
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_unlabeled", KC_UNLABELED );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_next_track", KC_NEXTTRACK );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_enter", KC_NUMPADENTER );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_rcontrol", KC_RCONTROL );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_mute", KC_MUTE );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_CALCULATOR );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_playpause", KC_PLAYPAUSE );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_MEDIASTOP );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_volume_down", KC_VOLUMEDOWN );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_volume_up", KC_VOLUMEUP );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBHOME );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_numpad_comma", KC_NUMPADCOMMA );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_divide", KC_DIVIDE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_sysrq", KC_SYSRQ );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_rmenu", KC_RMENU );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_pause", KC_PAUSE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_home", KC_HOME );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_up", KC_UP );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_pgup", KC_PGUP );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_left", KC_LEFT );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_right", KC_RIGHT );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_end", KC_END );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_down", KC_DOWN );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_pgdown", KC_PGDOWN );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_insert", KC_INSERT );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_delete", KC_DELETE );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_lwin", KC_LWIN );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_rwin", KC_RWIN );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_apps", KC_APPS );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_power", KC_POWER );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_sleep", KC_SLEEP );
				REG_KEYBOARD_ACTIONCOND_CREATOR( "key_wake", KC_WAKE );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBSEARCH );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBFAVORITES );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBREFRESH );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBSTOP );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBFORWARD );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_WEBBACK );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_MYCOMPUTER );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_MAIL );
				//REG_KEYBOARD_ACTIONCOND_CREATOR( "key_", KC_MEDIASELECT );
			}
			// KEY
			const StringToKcMap::const_iterator itFind = g_s2kc.find( key );
			if (itFind == g_s2kc.end())
				return 0;
			return new KeyboardActionCondition( pKeyboard, itFind->second, kam );
		}

	}

	bool ActionMap::loadFromFile(ActionMap& amap, const String& fname, KeyboardDevice* pKeyboard)
	{
		YAKE_ASSERT( pKeyboard ).warning("No keyboard device.");

		std::ifstream in( fname.c_str() );
		if (!in.is_open())
			return false;

		do {
			String line;
			std::getline(in,line);

			// parse line
			StringVector splitVector;
			splitVector = yake::split<String>(line, "=");

			YAKE_ASSERT( splitVector.size() >= 2 )( line ).warning("Invalid syntax! Not enough parameters!");
			if (splitVector.size() < 2)
				continue;

			const String actionName = splitVector.front();
			splitVector = yake::split<String>(splitVector.back(), ",");
			YAKE_ASSERT( splitVector.size() >= 1 )( actionName ).warning("Invalid syntax! Need an input specifier!");
			if (splitVector.size() < 1)
				continue;

			const String inputSpec = StringUtil::toLowerCase( splitVector.front() ); // e.g. "LMB" or "KEY_A"
			const String mode = StringUtil::toLowerCase( (splitVector.size() <= 1) ? "pressed" : splitVector.at(1) );
#ifdef YAKE_DEBUG
			if (splitVector.size() > 2)
			{
				YAKE_ASSERT( splitVector.size() <= 2 )(line).warning("Too many parameters! Ignoring superfluous ones!");
			}
#endif
			YAKE_ASSERT( mode == "pressed" || mode == "continuous" || mode == "released" || mode == "pressed_released" )(mode).warning("Invalid mode! Skipping!");
			if (mode != "pressed" && mode != "continuous" && mode != "released" && mode != "pressed_released")
				continue;

			ActionCondition* pCond = 0;
			if (StringUtil::startsWith(inputSpec, "key_", true ))
			{
				const KeyboardActionMode kam = (mode == "pressed") ? KAM_PRESSED :
				((mode == "continuous") ? KAM_CONTINUOUS :
				((mode == "released") ? KAM_RELEASED :
				((mode == "pressed_released") ? KAM_PRESSED_AND_RELEASED :
					KAM_PRESSED
				)
				)
				);

				pCond = detail::createKeyboardActionCondition( inputSpec, kam, pKeyboard );
			}

			//@todo create action
			YAKE_ASSERT( 0 ).debug("Batman, help!");
			YAKE_ASSERT( pCond );
			if (!pCond)
				continue;

			//@todo amap.reg( ..., pCond );

		} while (!in.eof());

		return true;
	}

	bool ActionMap::saveToFile(ActionMap& amap, const String& fname)
	{
		return false;
	}

	//-----------------------------------------------------
	// ActionId
	//-----------------------------------------------------

	bool ActionId::operator == (const ActionId& rhs) const
	{
#ifdef YAKE_DEBUG
		return (mId == rhs.mId) && (mName == rhs.mName);
#else
		return mId == rhs.mId;
#endif
	}
	bool ActionId::operator != (const ActionId& rhs) const
	{
#ifdef YAKE_DEBUG
		return (mId != rhs.mId) && (mName != rhs.mName);
#else
		return mId != rhs.mId;
#endif
	}

	ActionId::ActionId( const ActionId& other ) : mId(other.mId), mName(other.mName)
	{
	}

	ActionId::ActionId( IdType id ) : mId(id)
	{
		mName = ActionIdRegistry::getById( id );
	}

	ActionId::ActionId( IdType id, const String & name ) : mId(id), mName(name)
	{
		ActionIdRegistry::reg( *this );
	}

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
		mEntries.clear();
	}

	//-----------------------------------------------------
	ConditionConnection ActionMap::reg( const ActionId & actionId, ActionCondition* condition )
	{
		YAKE_ASSERT( condition );

		ActionList::iterator it = mEntries.find( actionId );
		if (it == mEntries.end())
		{
			std::pair<ActionList::iterator,bool> vt = mEntries.insert( std::make_pair( actionId, new ActionMapEntry() ) );
			YAKE_ASSERT( vt.second );
			vt.first->second->conditions.push_back( SharedPtr<ActionCondition>(condition));
		}
		else
			it->second->conditions.push_back( SharedPtr<ActionCondition>(condition));

		return ConditionConnection(this,actionId,condition);
	}

	//-----------------------------------------------------
	bool operator == (const SharedPtr<ActionCondition>& lhs, const ActionCondition* rhs)
	{
		return lhs.get() == rhs;
	}
	//-----------------------------------------------------
	void ActionMap::unreg( const ActionId& actionId, void* p )
	{
		ActionList::const_iterator it = mEntries.find( actionId );
		YAKE_ASSERT( it != mEntries.end() );
		if (it == mEntries.end())
			return;

		ActionCondition* cond = reinterpret_cast<ActionCondition*>(p);
		YAKE_ASSERT( cond );

		ConditionList& condList = it->second->conditions;
		ConditionList::iterator itC = std::find(condList.begin(), condList.end(), cond);
		YAKE_ASSERT( itC != condList.end() );
		condList.erase( itC );
	}

	//-----------------------------------------------------
	SignalConnection ActionMap::subscribeToActionId( const ActionId & actionId, const ActionSignal::slot_type & slot )
	{
		ActionList::const_iterator it = mEntries.find( actionId );
		YAKE_ASSERT( it != mEntries.end() );
		if (it == mEntries.end())
			return SignalConnection();
		ActionMapEntry* pEntry = it->second.get();
		YAKE_ASSERT( pEntry );
		return pEntry->signal.connect( slot );
	}

	//-----------------------------------------------------
	void ActionMap::update()
	{
		// the update loop
		ActionList dead;
		std::deque<ActionMapEntry*> triggered;
		for (ActionList::const_iterator it = mEntries.begin(); it != mEntries.end(); ++it)
		{
			ActionMapEntry* pEntry = it->second.get();
			YAKE_ASSERT( pEntry );
			if (!pEntry->alive)
			{
				dead.insert( std::make_pair(it->first, it->second) );
				continue;
			}
			const ConditionList& condList = pEntry->conditions;
			for (ConditionList::const_iterator itC = condList.begin(); itC != condList.end(); ++itC)
			{
				if ( (**itC)() )
				{
					triggered.push_back( pEntry );
					break; // ! important. @todo make it possible to subscribe to specific conditions.
				}
			}
		}


		// remove dead ones
		ConstVectorIterator<ActionList> itDead( dead );
		while (itDead.hasMoreElements())
		{
			ActionList::iterator itFind = std::find( mEntries.begin(), mEntries.end(), itDead.getNext() );
			if (itFind != mEntries.end())
			{
				mEntries.erase( itFind );
			}
		}

		// trigger signals
		ConstVectorIterator<std::deque<ActionMapEntry*> > itTriggered( triggered );
		while (itTriggered.hasMoreElements())
		{
			itTriggered.getNext()->signal();
		}
		triggered.clear();
	}

	//-----------------------------------------------------
	// class: ConditionConnection
	//-----------------------------------------------------
	ConditionConnection::ConditionConnection(ActionMap* m,const ActionId& id,void* p) : map_(m), data_(p), id_(id)
	{}
	ConditionConnection::ConditionConnection(const ConditionConnection& other) : map_(other.map_), data_(other.data_), id_(other.id_)
	{
	}
	ConditionConnection::~ConditionConnection()
	{
	}
	ConditionConnection& ConditionConnection::operator=(const ConditionConnection& rhs)
	{
		if (this == &rhs)
			return *this;
		map_ = rhs.map_;
		data_ = rhs.data_;
		id_ = rhs.id_;
		return *this;
	}
	void ConditionConnection::disconnect()
	{
		if (map_ && data_)
			map_->unreg( id_, data_ );
		map_ = 0;
		data_ = 0;
	}
}
}
