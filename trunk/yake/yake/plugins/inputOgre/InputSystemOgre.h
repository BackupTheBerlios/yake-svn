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
#ifndef INC_YAKE_INPUTSYSTEMOGRE_H
#define INC_YAKE_INPUTSYSTEMOGRE_H

namespace Ogre {
	class InputReader;
};

namespace yake {
using namespace base::math;
namespace input {

	//-----------------------------------------------------------------------
	class MouseDeviceOgre : public MouseDevice
	{
	public:
		MouseDeviceOgre(Ogre::InputReader * inputReader);
		virtual ~MouseDeviceOgre();

		virtual int getNumButtons() const;
		virtual bool isButtonDown( int button ) const;
		virtual base::math::Vector3 getPosition() const;
		virtual void setClip( const base::math::Rectangle & rect )
		{ mClip = rect; }
		virtual void setScale( const base::math::Vector3 & scale )
		{ mScale = scale; }

		virtual void poll();

	protected:
		Ogre::InputReader	* mInputReader;
		bool				mButtons[3];
		Vector3				mPosition;
		Vector3				mScale;
		Rectangle			mClip;
	};

	//-----------------------------------------------------------------------
	class KeyboardDeviceOgre : public KeyboardDevice
	{
	public:
		KeyboardDeviceOgre(Ogre::InputReader * inputReader);
		virtual ~KeyboardDeviceOgre();

		virtual void poll();

		virtual bool isKeyDown( KeyCode key ) const;

	protected:
		Ogre::InputReader	* mInputReader;
	};

	//-----------------------------------------------------------------------
	class InputSystemOgre : public IInputSystem
	{
		YAKE_DECLARE_CONCRETE( InputSystemOgre, "ogre" )
	public:
		InputSystemOgre();
		virtual ~InputSystemOgre();

		virtual void update();
		virtual DeviceEntryList getAvailableDevices();
		virtual InputDevice* activateDevice( const base::String & deviceName );
		virtual InputDevice* getDevice( const base::String & deviceName ) const;
		virtual void getActiveDevices( std::vector< InputDevice* > & devices ) const;

	protected:
		// devices
		struct ActiveDevice {
			base::String	name;
			InputDevice*	device;
			InputDeviceType	type;
		};
		typedef base::templates::Vector< ActiveDevice >	ActiveDeviceList;
		ActiveDeviceList	mActiveDevices;

		// Ogre stuff
		Ogre::InputReader	* mInputReader;
	};

}
}
#endif
