#include <yakeInput/inc/pch.h>
#include <yakeInput/inc/yakeInputEventGenerator.h>

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
	MouseEventGenerator::MouseEventGenerator() : mMouse(0), mMousePosition( base::math::Vector3::kZero )
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
		base::math::Vector3 pos = mMouse->getPosition();
		if (pos != mMousePosition)
		{
			mMousePosition = pos;
			mMouseMovedSignal( mMousePosition );
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