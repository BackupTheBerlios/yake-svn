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
#include <yake/input/yakeInputEventGenerator.h>

namespace yake {
namespace input {

	//-----------------------------------------------------
	// KeyboardEventGenerator
	//-----------------------------------------------------

	//-----------------------------------------------------
	KeyboardEventGenerator::KeyboardEventGenerator() : mKeyb(0)
	{
	}

	//-----------------------------------------------------
	KeyboardEventGenerator::~KeyboardEventGenerator()
	{
	}

	//-----------------------------------------------------
	void KeyboardEventGenerator::update()
	{
		if (!mKeyb)
			return;

		for (int key=0; key<=255; ++key)
		{
			bool state = mKeyb->isKeyDown( static_cast<KeyCode>(key) );
			if (state != mKeyData[key])
			{
				KeyboardEvent evt;
				evt.keyCode = static_cast<KeyCode>(key);
				if (state)
					mKeyDownSignal( evt );
				else
					mKeyUpSignal( evt );
				mKeyData[key] = state;
			}
		}
	}

	//-----------------------------------------------------
	void KeyboardEventGenerator::attachDevice( InputDevice* device )
	{
		mKeyb = static_cast<KeyboardDevice*>(device);
	}

	//-----------------------------------------------------
	InputDevice* KeyboardEventGenerator::detachDevice()
	{
		InputDevice* inpDev = mKeyb;
		mKeyb = 0;
		return inpDev;
	}

	//-----------------------------------------------------
	// MouseEventGenerator
	//-----------------------------------------------------

	//-----------------------------------------------------
	MouseEventGenerator::MouseEventGenerator() : mMouse(0), mMousePosition( math::Vector3::kZero )
	{
	}

	//-----------------------------------------------------
	MouseEventGenerator::~MouseEventGenerator()
	{
	}

	//-----------------------------------------------------
	void MouseEventGenerator::update()
	{
		if (!mMouse)
			return;

		// wheel

		// movement
		math::Vector3 pos = mMouse->getPosition();
		if (pos != mMousePosition)
		{
			mMouseMovedSignal( pos - mMousePosition ); // send delta
			mMousePosition = pos;
		}

		// buttons
		uint8 buttonCount = mMouse->getNumButtons();
		if (buttonCount > 3)
			buttonCount = 3;

		for (uint8 btn=0; btn<buttonCount; ++btn)
		{
			bool state = mMouse->isButtonDown( btn );
			if (state != mMouseButtons[btn])
			{
				if (state)
					mMouseButtonDownSignal( btn );
				else
					mMouseButtonUpSignal( btn );
				mMouseButtons[btn] = state;
			}
		}
	}

	//-----------------------------------------------------
	void MouseEventGenerator::attachDevice( InputDevice* device )
	{
		mMouse = static_cast<MouseDevice*>(device);
	}

	//-----------------------------------------------------
	InputDevice* MouseEventGenerator::detachDevice()
	{
		InputDevice* inpDev = mMouse;
		mMouse = 0;
		return inpDev;
	}

}
}
