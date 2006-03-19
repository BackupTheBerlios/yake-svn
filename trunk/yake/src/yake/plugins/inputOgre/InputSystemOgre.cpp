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
#include <yake/plugins/inputOgre/yakePCH.h>
#include <yake/input/yakeInputSystem.h>
#include <yake/plugins/inputOgre/InputSystemOgre.h>

#include <OgreMemoryMacros.h>
#include <Ogre.h>
#ifdef OGRE_MEMORY_MACROS
#pragma message("disabling OGRE's memory macros")
#	include <OgreNoMemoryMacros.h>
#endif

namespace yake {
namespace input {

	YAKE_REGISTER_CONCRETE( InputSystemOgre );

	//-----------------------------------------------------------------------
	MouseDeviceOgre::MouseDeviceOgre(Ogre::InputReader * inputReader) :
				mInputReader(inputReader),
				mPosition(math::Vector3::kZero)
	{
		mButtons[0] = mButtons[1] = mButtons[2] = false;
	}

	//-----------------------------------------------------------------------
	MouseDeviceOgre::~MouseDeviceOgre()
	{
	}

	//-----------------------------------------------------------------------
	int MouseDeviceOgre::getNumButtons() const
	{
		return 3;
	}

	//-----------------------------------------------------------------------
	bool MouseDeviceOgre::isButtonDown( int button ) const
	{
		if (button > 2) return false;
		if (button < 0) return false;
		return mButtons[ button ];
	}
	
	//-----------------------------------------------------------------------
	math::Vector3 MouseDeviceOgre::getPosition() const
	{
		return mPosition;
	}

	//-----------------------------------------------------------------------
	void MouseDeviceOgre::poll()
	{
		for (int btn = 0; btn < 3; ++btn)
		{
			mButtons[btn] = mInputReader->getMouseButton( btn );
		}
		mPosition.x += mInputReader->getMouseRelativeX();
		mPosition.y += mInputReader->getMouseRelativeY();
		mPosition.z += mInputReader->getMouseRelativeZ();
		/*mPosition.x = mInputReader->getMouseAbsX();
		mPosition.y = mInputReader->getMouseAbsY();
		mPosition.z = mInputReader->getMouseAbsZ();*/
	}

	//-----------------------------------------------------------------------
	KeyboardDeviceOgre::KeyboardDeviceOgre(Ogre::InputReader * inputReader) : mInputReader(inputReader)
	{
	}

	//-----------------------------------------------------------------------
	KeyboardDeviceOgre::~KeyboardDeviceOgre()
	{
	}

	//-----------------------------------------------------------------------
	void KeyboardDeviceOgre::poll()
	{
	}

	//-----------------------------------------------------------------------
	bool KeyboardDeviceOgre::isKeyDown( KeyCode key ) const
	{
		if (!mInputReader)
			return false;
		return mInputReader->isKeyDown( static_cast<Ogre::KeyCode>(key) );
	}

	//-----------------------------------------------------------------------
	InputSystemOgre::InputSystemOgre()
	{
		mInputReader = Ogre::PlatformManager::getSingleton().createInputReader();
		YAKE_ASSERT( mInputReader ).debug("Could not create input reader.");
		if (mInputReader)
			mInputReader->initialise(Ogre::Root::getSingleton().getAutoCreatedWindow(), true, true, false);
	}

	//-----------------------------------------------------------------------
	InputSystemOgre::~InputSystemOgre()
	{
		for (ActiveDeviceList::const_iterator it = mActiveDevices.begin(); it != mActiveDevices.end(); ++it)
			delete (*it).device;
		Ogre::PlatformManager::getSingleton().destroyInputReader( mInputReader );
	}

	//-----------------------------------------------------------------------
	void InputSystemOgre::update()
	{
		if (mInputReader)
			mInputReader->capture();
		for (ActiveDeviceList::const_iterator it = mActiveDevices.begin(); it != mActiveDevices.end(); ++it)
			(*it).device->poll();
	}

	//-----------------------------------------------------------------------
	void InputSystemOgre::getActiveDevices( std::vector< InputDevice* > & devices ) const
	{
		devices.clear();
		for (ActiveDeviceList::const_iterator it = mActiveDevices.begin(); it != mActiveDevices.end(); ++it)
			devices.push_back( (*it).device );
	}

	//-----------------------------------------------------------------------
	InputDevice* InputSystemOgre::activateDevice( const String & deviceName )
	{
		if (getDevice(deviceName))
			return 0;

		if (deviceName == "Keyboard")
		{
			input::KeyboardDevice* pKeyboard = new KeyboardDeviceOgre( mInputReader );
			YAKE_ASSERT( pKeyboard ).debug("Out of memory ?");
			if (!pKeyboard)
				return 0;

			ActiveDevice dev;
			dev.name = "Keyboard";
			dev.type = IDT_KEYBOARD;
			dev.device = pKeyboard;
			mActiveDevices.push_back( dev );
			return pKeyboard;
		}
		else if (deviceName == "Mouse")
		{
			input::MouseDevice* pMouse = new MouseDeviceOgre( mInputReader );
			YAKE_ASSERT( pMouse ).debug("Out of memory ?");
			if (!pMouse)
				return 0;

			ActiveDevice dev;
			dev.name = "Mouse";
			dev.type = IDT_MOUSE;
			dev.device = pMouse;
			mActiveDevices.push_back( dev );
			return pMouse;
		}
		return 0;
	}

	//-----------------------------------------------------------------------
	InputDevice* InputSystemOgre::getDevice( const String & deviceName ) const
	{
		for (ActiveDeviceList::const_iterator it = mActiveDevices.begin(); it != mActiveDevices.end(); ++it)
			if ((*it).name == deviceName)
				return (*it).device;
		return 0;
	}

	//-----------------------------------------------------------------------
	InputSystemOgre::DeviceEntryList InputSystemOgre::getAvailableDevices()
	{
		DeviceEntryList list;
		DeviceListEntry entry;

		entry.name = "Mouse";
		entry.type = IDT_MOUSE;
		list.push_back( entry );

		entry.name = "Keyboard";
		entry.type = IDT_KEYBOARD;
		list.push_back( entry );

		return list;
	}

}
}
