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
#ifndef INC_YAKE_INPUTSYSTEM_H
#define INC_YAKE_INPUTSYSTEM_H

#include <inc/input/yakeInput.h>
#include <inc/base/math/yakeGeometry.h>

namespace yake {
namespace input {

	// Keyboard scan codes - copied from DirectInput for now for speed.
	// (SK: actually I copied them from Ogre again...)
	enum KeyCode
	{
		KC_ESCAPE          =0x01,
		KC_1               =0x02,
		KC_2               =0x03,
		KC_3               =0x04,
		KC_4               =0x05,
		KC_5               =0x06,
		KC_6               =0x07,
		KC_7               =0x08,
		KC_8               =0x09,
		KC_9               =0x0A,
		KC_0               =0x0B,
		KC_MINUS           =0x0C,    /* - on main keyboard */
		KC_EQUALS          =0x0D,
		KC_BACK            =0x0E,    /* backspace */
		KC_TAB             =0x0F,
		KC_Q               =0x10,
		KC_W               =0x11,
		KC_E               =0x12,
		KC_R               =0x13,
		KC_T               =0x14,
		KC_Y               =0x15,
		KC_U               =0x16,
		KC_I               =0x17,
		KC_O               =0x18,
		KC_P               =0x19,
		KC_LBRACKET        =0x1A,
		KC_RBRACKET        =0x1B,
		KC_RETURN          =0x1C,    /* Enter on main keyboard */
		KC_LCONTROL        =0x1D,
		KC_A               =0x1E,
		KC_S               =0x1F,
		KC_D               =0x20,
		KC_F               =0x21,
		KC_G               =0x22,
		KC_H               =0x23,
		KC_J               =0x24,
		KC_K               =0x25,
		KC_L               =0x26,
		KC_SEMICOLON       =0x27,
		KC_APOSTROPHE      =0x28,
		KC_GRAVE           =0x29,    /* accent grave */
		KC_LSHIFT          =0x2A,
		KC_BACKSLASH       =0x2B,
		KC_Z               =0x2C,
		KC_X               =0x2D,
		KC_C               =0x2E,
		KC_V               =0x2F,
		KC_B               =0x30,
		KC_N               =0x31,
		KC_M               =0x32,
		KC_COMMA           =0x33,
		KC_PERIOD          =0x34,    /* . on main keyboard */
		KC_SLASH           =0x35,    /* '/' on main keyboard */
		KC_RSHIFT          =0x36,
		KC_MULTIPLY        =0x37,    /* * on numeric keypad */
		KC_LMENU           =0x38,    /* left Alt */
		KC_SPACE           =0x39,
		KC_CAPITAL         =0x3A,
		KC_F1              =0x3B,
		KC_F2              =0x3C,
		KC_F3              =0x3D,
		KC_F4              =0x3E,
		KC_F5              =0x3F,
		KC_F6              =0x40,
		KC_F7              =0x41,
		KC_F8              =0x42,
		KC_F9              =0x43,
		KC_F10             =0x44,
		KC_NUMLOCK         =0x45,
		KC_SCROLL          =0x46,    /* Scroll Lock */
		KC_NUMPAD7         =0x47,
		KC_NUMPAD8         =0x48,
		KC_NUMPAD9         =0x49,
		KC_SUBTRACT        =0x4A,    /* - on numeric keypad */
		KC_NUMPAD4         =0x4B,
		KC_NUMPAD5         =0x4C,
		KC_NUMPAD6         =0x4D,
		KC_ADD             =0x4E,    /* + on numeric keypad */
		KC_NUMPAD1         =0x4F,
		KC_NUMPAD2         =0x50,
		KC_NUMPAD3         =0x51,
		KC_NUMPAD0         =0x52,
		KC_DECIMAL         =0x53,    /* . on numeric keypad */
		KC_OEM_102         =0x56,    /* < > | on UK/Germany keyboards */
		KC_F11             =0x57,
		KC_F12             =0x58,
		KC_F13             =0x64,    /*                     (NEC PC98) */
		KC_F14             =0x65,    /*                     (NEC PC98) */
		KC_F15             =0x66,    /*                     (NEC PC98) */
		KC_KANA            =0x70,    /* (Japanese keyboard)            */
		KC_ABNT_C1         =0x73,    /* / ? on Portugese (Brazilian) keyboards */
		KC_CONVERT         =0x79,    /* (Japanese keyboard)            */
		KC_NOCONVERT       =0x7B,    /* (Japanese keyboard)            */
		KC_YEN             =0x7D,    /* (Japanese keyboard)            */
		KC_ABNT_C2         =0x7E,    /* Numpad . on Portugese (Brazilian) keyboards */
		KC_NUMPADEQUALS    =0x8D,    /* = on numeric keypad (NEC PC98) */
		KC_PREVTRACK       =0x90,    /* Previous Track (KC_CIRCUMFLEX on Japanese keyboard) */
		KC_AT              =0x91,    /*                     (NEC PC98) */
		KC_COLON           =0x92,    /*                     (NEC PC98) */
		KC_UNDERLINE       =0x93,    /*                     (NEC PC98) */
		KC_KANJI           =0x94,    /* (Japanese keyboard)            */
		KC_STOP            =0x95,    /*                     (NEC PC98) */
		KC_AX              =0x96,    /*                     (Japan AX) */
		KC_UNLABELED       =0x97,    /*                        (J3100) */
		KC_NEXTTRACK       =0x99,    /* Next Track */
		KC_NUMPADENTER     =0x9C,    /* Enter on numeric keypad */
		KC_RCONTROL        =0x9D,
		KC_MUTE            =0xA0,    /* Mute */
		KC_CALCULATOR      =0xA1,    /* Calculator */
		KC_PLAYPAUSE       =0xA2,    /* Play / Pause */
		KC_MEDIASTOP       =0xA4,    /* Media Stop */
		KC_VOLUMEDOWN      =0xAE,    /* Volume - */
		KC_VOLUMEUP        =0xB0,    /* Volume + */
		KC_WEBHOME         =0xB2,    /* Web home */
		KC_NUMPADCOMMA     =0xB3,    /* , on numeric keypad (NEC PC98) */
		KC_DIVIDE          =0xB5,    /* / on numeric keypad */
		KC_SYSRQ           =0xB7,
		KC_RMENU           =0xB8,    /* right Alt */
		KC_PAUSE           =0xC5,    /* Pause */
		KC_HOME            =0xC7,    /* Home on arrow keypad */
		KC_UP              =0xC8,    /* UpArrow on arrow keypad */
		KC_PGUP            =0xC9,    /* PgUp on arrow keypad */
		KC_LEFT            =0xCB,    /* LeftArrow on arrow keypad */
		KC_RIGHT           =0xCD,    /* RightArrow on arrow keypad */
		KC_END             =0xCF,    /* End on arrow keypad */
		KC_DOWN            =0xD0,    /* DownArrow on arrow keypad */
		KC_PGDOWN          =0xD1,    /* PgDn on arrow keypad */
		KC_INSERT          =0xD2,    /* Insert on arrow keypad */
		KC_DELETE          =0xD3,    /* Delete on arrow keypad */
		KC_LWIN            =0xDB,    /* Left Windows key */
		KC_RWIN            =0xDC,    /* Right Windows key */
		KC_APPS            =0xDD,    /* AppMenu key */
		KC_POWER           =0xDE,    /* System Power */
		KC_SLEEP           =0xDF,    /* System Sleep */
		KC_WAKE            =0xE3,    /* System Wake */
		KC_WEBSEARCH       =0xE5,    /* Web Search */
		KC_WEBFAVORITES    =0xE6,    /* Web Favorites */
		KC_WEBREFRESH      =0xE7,    /* Web Refresh */
		KC_WEBSTOP         =0xE8,    /* Web Stop */
		KC_WEBFORWARD      =0xE9,    /* Web Forward */
		KC_WEBBACK         =0xEA,    /* Web Back */
		KC_MYCOMPUTER      =0xEB,    /* My Computer */
		KC_MAIL            =0xEC,    /* Mail */
		KC_MEDIASELECT     =0xED     /* Media Select */
	};

	enum InputDeviceType {
		IDT_KEYBOARD,
		IDT_JOYSTICK,
		IDT_MOUSE,
		IDT_OTHER
	};

	class YAKE_INPUT_API InputDevice
	{
	protected:
		InputDevice() {}
	public:
		virtual ~InputDevice() {}

		virtual InputDeviceType getType() const = 0;

		/** This is usually called in regular intervals
			to get the input status of each device.
			To trigger signals/events (e.g. onKeyDown) use a DeviceEventGenerator like
			KeyboardEventGenerator or MouseEventGenerator, for example.
			\see KeyboardEventGenerator
			\see MouseEventGenerator
			\see JoystickEventGenerator
		*/
		virtual void poll() = 0;
	};

	class YAKE_INPUT_API KeyboardDevice : public InputDevice
	{
	public:			
		virtual InputDeviceType getType() const
		{ return IDT_KEYBOARD; }

		virtual bool isKeyDown( KeyCode key ) = 0;
	};

	class YAKE_INPUT_API MouseDevice : public InputDevice
	{
	public:			
		virtual InputDeviceType getType() const
		{ return IDT_MOUSE; }

		virtual int getNumButtons() const = 0;
		virtual bool isButtonDown( int button ) const = 0;
		virtual base::math::Vector3 getPosition() const = 0;
		virtual void setClip( const base::math::Rectangle & rect ) = 0;
		virtual void setScale( const base::math::Vector3 & scale ) = 0;
	};

	class YAKE_INPUT_API JoystickDevice : public InputDevice
	{
	public:			
		static const uint16 HAT_CENTERED	= 0x0010;
		static const uint16 HAT_LEFT		= 0x0001;
		static const uint16 HAT_RIGHT		= 0x0002;
		static const uint16 HAT_UP			= 0x0004;
		static const uint16 HAT_DOWN		= 0x0008;
		static const uint16 HAT_LEFTUP		= HAT_LEFT | HAT_UP;
		static const uint16 HAT_LEFTDOWN	= HAT_LEFT | HAT_DOWN;
		static const uint16 HAT_RIGHTUP	= HAT_RIGHT | HAT_UP;
		static const uint16 HAT_RIGHTDOWN	= HAT_RIGHT | HAT_DOWN;

	public:
		virtual InputDeviceType getType() const
		{ return IDT_JOYSTICK; }

		virtual bool isButtonDown( int button ) const = 0;

		virtual uint16 getHatPosition() const = 0;

		/// Return the number of available axis
		virtual int getNumAxes() const = 0;

		/// Return the position of the axis. Values in [-1..1].
		virtual real getAxisPosition( int axis ) const = 0;

		/// Set the dead zone for an axis. Values in [-1..1].
		virtual void setAxisDeadZone( int axis, real deadzoneMin = -0.07, real deadzoneMax = 0.07 ) = 0;

		virtual std::pair<real,real> getBallPosition() const = 0;
	};

	struct KeyboardEvent
	{
		InputDevice		* device;
		KeyCode keyCode;
	};
	struct MouseWheelEvent;
	struct JoystickEvent;
	struct JoystickAxisEvent;

	struct InputEvent
	{
		InputDevice		* device;
	};

	class InputSystem;
	class YAKE_INPUT_API InputPlugin : public base::Plugin
	{
	public:
		virtual InputSystem* createSystem() = 0;
	};

	class InputEventGenerator;
	/** Input system interface.
	*/
	class YAKE_INPUT_API InputSystem// : public System
	{
	protected:
		/*InputSystem() : System( ST_INPUT )
		{
		}*/
	public:
		~InputSystem() {}
		/** Used for enumerating devices. */
		struct DeviceListEntry
		{
			base::String		name;
			InputDeviceType		type;
		};
		typedef std::vector< DeviceListEntry > DeviceList;

	public:
		virtual void update() = 0;
		virtual DeviceList getAvailableDevices() = 0;

		virtual InputDevice* activateDevice( const base::String & deviceName ) = 0;

		virtual InputDevice* getDevice( const base::String & deviceName ) const = 0;

		virtual void getActiveDevices( std::vector< InputDevice* > & devices ) const = 0;
	};

}
}

#endif

