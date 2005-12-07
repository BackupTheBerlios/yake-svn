#ifndef YAKE_CEGUI_HELPERS_H
#define YAKE_CEGUI_HELPERS_H

namespace yake {
	/** 
	 * Convert Yake button id to corresponding CEGUI button id
	 * @param buttonID Yake button id
	 * @return CEGUI button id
	 */
	CEGUI::MouseButton convertYakeButtonToCegui( int buttonID )
	{
		switch (buttonID)
		{
		case 0:
			return CEGUI::LeftButton;
		case 1:
			return CEGUI::RightButton;
		case 2:
			return CEGUI::MiddleButton;
		case 3:
			return CEGUI::X1Button;
		default:
			return CEGUI::LeftButton;
		}
	}

	/**
	 *   Get character corresponding to passed keycode.
	 * @param keyCode 
	 * @param modifiers Shows if shift button was pressed
	 * @return Character
	 * @todo move this into yake::input
	 * @remarks Taken from the mighty OGRE3D :)
	 */
	char getKeyChar(int keyCode, long modifiers)
	{
		//HACK ripped from OgreInput
		//TODO Ogre is rendering engine... should Yake input system contain this method?
		using namespace yake::input;

		if (modifiers == 0)
		{
			switch (keyCode)
			{
			case KC_1: return '1';
			case KC_2: return '2';
			case KC_3: return '3';
			case KC_4: return '4';
			case KC_5: return '5';
			case KC_6: return '6';
			case KC_7: return '7';
			case KC_8: return '8';
			case KC_9: return '9';
			case KC_0: return '0';
			case KC_MINUS: return '-';			/* - on main keyboard */
			case KC_EQUALS: return '=';
			case KC_Q: return 'q';
			case KC_W: return 'w';
			case KC_E: return 'e';
			case KC_R: return 'r';
			case KC_T: return 't';
			case KC_Y: return 'y';
			case KC_U: return 'u';
			case KC_I: return 'i';
			case KC_O: return 'o';
			case KC_P: return 'p';
			case KC_LBRACKET: return '[';
			case KC_RBRACKET: return ']';
			case KC_A: return 'a';
			case KC_S: return 's';
			case KC_D: return 'd';
			case KC_F: return 'f';
			case KC_G: return 'g';
			case KC_H: return 'h';
			case KC_J: return 'j';
			case KC_K: return 'k';
			case KC_L: return 'l';
			case KC_SEMICOLON: return ';';
			case KC_APOSTROPHE: return '\'';
			case KC_GRAVE: return '`';			/* accent grave */
			case KC_BACKSLASH: return '\\';
			case KC_Z: return 'z';
			case KC_X: return 'x';
			case KC_C: return 'c';
			case KC_V: return 'v';
			case KC_B: return 'b';
			case KC_N: return 'n';
			case KC_M: return 'm';
			case KC_COMMA: return ',';
			case KC_PERIOD: return '.';			/* . on main keyboard */
			case KC_SLASH: return '/';			/* '/' on main keyboard */
			case KC_MULTIPLY: return '*';		/* * on numeric keypad */
			case KC_SPACE: return ' ';
			case KC_NUMPAD7: return '7';
			case KC_NUMPAD8: return '8';
			case KC_NUMPAD9: return '9';
			case KC_SUBTRACT: return '-';		/* - on numeric keypad */
			case KC_NUMPAD4: return '4';
			case KC_NUMPAD5: return '5';
			case KC_NUMPAD6: return '6';
			case KC_ADD: return '+';			/* + on numeric keypad */
			case KC_NUMPAD1: return '1';
			case KC_NUMPAD2: return '2';
			case KC_NUMPAD3: return '3';
			case KC_NUMPAD0: return '0';
			case KC_DECIMAL: return '.';		/* . on numeric keypad */
			case KC_NUMPADEQUALS: return '=';	/* = on numeric keypad (NEC PC98) */
			case KC_AT: return '@';				/*                     (NEC PC98) */
			case KC_COLON: return ':';			/*                     (NEC PC98) */
			case KC_UNDERLINE: return '_';		/*                     (NEC PC98) */
			case KC_NUMPADCOMMA: return ',';	/* , on numeric keypad (NEC PC98) */
			case KC_DIVIDE: return '/';			/* / on numeric keypad */
			}
		}
		else //HACK if ( modifiers == InputEvent::SHIFT_MASK)
		{
			switch (keyCode)
			{
			case KC_1: return '!';
			case KC_2: return '@';
			case KC_3: return '#';
			case KC_4: return '$';
			case KC_5: return '%';
			case KC_6: return '^';
			case KC_7: return '&';
			case KC_8: return '*';
			case KC_9: return '(';
			case KC_0: return ')';
			case KC_MINUS: return '_';
			case KC_EQUALS: return '+';
			case KC_Q: return 'Q';
			case KC_W: return 'W';
			case KC_E: return 'E';
			case KC_R: return 'R';
			case KC_T: return 'T';
			case KC_Y: return 'Y';
			case KC_U: return 'U';
			case KC_I: return 'I';
			case KC_O: return 'O';
			case KC_P: return 'P';
			case KC_LBRACKET: return '{';
			case KC_RBRACKET: return '}';
			case KC_A: return 'A';
			case KC_S: return 'S';
			case KC_D: return 'D';
			case KC_F: return 'F';
			case KC_G: return 'G';
			case KC_H: return 'H';
			case KC_J: return 'J';
			case KC_K: return 'K';
			case KC_L: return 'L';
			case KC_SEMICOLON: return ':';
			case KC_APOSTROPHE: return '"';
			case KC_GRAVE: return '~';			/* accent grave */
			case KC_BACKSLASH: return '|';
			case KC_Z: return 'Z';
			case KC_X: return 'X';
			case KC_C: return 'C';
			case KC_V: return 'V';
			case KC_B: return 'B';
			case KC_N: return 'N';
			case KC_M: return 'M';
			case KC_COMMA: return '<';
			case KC_PERIOD: return '>';			/* . on main keyboard */
			case KC_SLASH: return '?';			/* '/' on main keyboard */
			case KC_MULTIPLY: return '*';		/* * on numeric keypad */
			case KC_SPACE: return ' ';
			}
		}
		return 0;
	}

} // namespace yake

#endif
