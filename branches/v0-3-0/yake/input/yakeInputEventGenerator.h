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
#ifndef INC_YAKE_INPUTEVENTGENERATOR_H
#define INC_YAKE_INPUTEVENTGENERATOR_H

#ifndef YAKE_INPUT_PREREQUISITES_H
#include <yake/input/yakePrerequisites.h>
#endif

namespace yake {
namespace input {

	class YAKE_INPUT_API DeviceEventGenerator
	{
	public:
		virtual ~DeviceEventGenerator() {}

		virtual void update() = 0;
		virtual void attachDevice( InputDevice* device ) = 0;
		virtual InputDevice* detachDevice() = 0;
	};

	class YAKE_INPUT_API KeyboardEventGenerator : public DeviceEventGenerator
	{
	public:
		KeyboardEventGenerator();
		virtual ~KeyboardEventGenerator();

		virtual void update();
		virtual void attachDevice( InputDevice* device );
		virtual InputDevice* detachDevice();

		// keyboard signal
		typedef Signal1< void( const KeyboardEvent & )> KeyboardSignal;

		// keyboard signals
		virtual void subscribeToKeyDown( const KeyboardSignal::slot_type & slot )
		{ mKeyDownSignal.connect( slot ); }
		virtual void subscribeToKeyUp( const KeyboardSignal::slot_type & slot )
		{ mKeyUpSignal.connect( slot ); }

	protected:
		// keyboard signal
		KeyboardSignal		mKeyDownSignal;
		KeyboardSignal		mKeyUpSignal;

	protected:
		bool				mKeyData[256];
		KeyboardDevice*		mKeyb;
	};

	class YAKE_INPUT_API MouseEventGenerator : public DeviceEventGenerator
	{
	public:
		MouseEventGenerator();
		virtual ~MouseEventGenerator();

		virtual void update();
		virtual void attachDevice( InputDevice* device );
		virtual InputDevice* detachDevice();

		// mouse signals
		typedef Signal1< void ( const math::Vector3 &)> MouseMovedSignal;
		typedef Signal1< void ( uint8 )> MouseButtonSignal;
		typedef Signal1< void ( const MouseWheelEvent & )> MouseWheelSignal;

		// mouse signals
		virtual void subscribeToMouseWheelDown( const MouseWheelSignal::slot_type & slot )
		{ mMouseWheelDownSignal.connect( slot ); }
		virtual void subscribeToMouseWheelUp( const MouseWheelSignal::slot_type & slot )
		{ mMouseWheelUpSignal.connect( slot ); }
		virtual void subscribeToMouseButtonDown( const MouseButtonSignal::slot_type & slot )
		{ mMouseButtonDownSignal.connect( slot ); }
		virtual void subscribeToMouseButtonUp( const MouseButtonSignal::slot_type & slot )
		{ mMouseButtonUpSignal.connect( slot ); }
		virtual void subscribeToMouseMoved( const MouseMovedSignal::slot_type & slot )
		{ mMouseMovedSignal.connect( slot ); }
	protected:
		// mouse signals
		MouseButtonSignal	mMouseButtonDownSignal,
							mMouseButtonUpSignal;
		MouseMovedSignal	mMouseMovedSignal;
		MouseWheelSignal	mMouseWheelDownSignal;
		MouseWheelSignal	mMouseWheelUpSignal;

	protected:
		math::Vector3	mMousePosition;
		bool				mMouseButtons[3];
		MouseDevice*		mMouse;
	};

	/** Once attached to an input system this generator will fire input events
		for every device it can handle. Subscribing to signals may be more useful
		than checking for certain input of each device manually.
	*/
	class YAKE_INPUT_API JoystickEventGenerator : public DeviceEventGenerator
	{
	public:
		JoystickEventGenerator();
		~JoystickEventGenerator();

		virtual void update();
		virtual void attachDevice( InputDevice* device );
		virtual InputDevice* detachDevice();

		void update( real timeElapsed );

		// joystick signals
		typedef Signal1< void( const JoystickEvent& ) > JoystickSignal;
		typedef Signal1< void( const JoystickAxisEvent& ) > JoystickAxisSignal;

	public:
		// signals

		// joystick signals
		virtual void subscribeToJoystickButtonDown( const JoystickSignal::slot_type & slot )
		{ mJoystickButtonDownSignal.connect( slot ); }
		virtual void subscribeToJoystickButtonUp( const JoystickSignal::slot_type & slot )
		{ mJoystickButtonUpSignal.connect( slot ); }
		virtual void subscribeToJoystickAxisChange( const JoystickAxisSignal::slot_type & slot )
		{ mJoystickAxisSignal.connect( slot ); }

	protected:

		// joystick signals
		JoystickSignal		mJoystickButtonDownSignal,
							mJoystickButtonUpSignal;
		JoystickAxisSignal	mJoystickAxisSignal;

	protected:
	};

}
}

#endif
