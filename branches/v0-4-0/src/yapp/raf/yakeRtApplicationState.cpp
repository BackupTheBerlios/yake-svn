/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) 2004 The YAKE Team
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
#include <yapp/raf/pch.h>
#include <yapp/raf/yakeApplicationState.h>
#include <yapp/raf/yakeRtApplicationState.h>
#include <yapp/raf/yakeApplication.h>

namespace yake {
namespace raf {

	//-----------------------------------------------------
	// Class: RtMainState
	//-----------------------------------------------------
	RtMainState::RtMainState(Application& owner) :
		MainState(owner), 
		mQuitRequested(false),
		mQuitKC(input::KC_ESCAPE),
		mQuitByKeyEnabled(false)
	{}
	graphics::ICamera* RtMainState::onCreateDefaultCamera()
	{
		return getGraphicalWorld()->createCamera();
	}
	graphics::IViewport* RtMainState::onCreateDefaultViewport()
	{
		YAKE_ASSERT( mDefaultCamera );
		if (!mDefaultCamera)
			return 0;
		graphics::IViewport* pVP = getGraphicalWorld()->createViewport( mDefaultCamera );
		YAKE_ASSERT( pVP );
		if (!pVP)
		{
			YAKE_LOG_WARNING("Could not create the default viewport!");
			return 0;
		}
		// default is a fullscreen viewport
		pVP->setDimensions( real(0), real(0), real(1), real(1) );
		pVP->setZ( 100 );
		return pVP;
	}
	void RtMainState::requestQuit()
	{
		mQuitRequested = onQuitRequested();
	}
	bool RtMainState::quitRequested() const
	{
		return mQuitRequested;
	}
	void RtMainState::enableInstantQuitByKey( input::KeyCode kc )
	{
		mQuitByKeyEnabled = true;
		mQuitKC = kc;
	}
	void RtMainState::onEnter()
	{
		createScene();
	}
	void RtMainState::onStep()
	{
		real lastTime = native::getTime();
		while (!quitRequested())
		{
			real now = native::getTime();
			real elapsed = now - lastTime;
			if (elapsed < real(0.0001))
				elapsed = real(0.01);

			if (getApp().getInputSystem())
			{
				getApp().getInputSystem()->update();
				if (getApp().getKeyboardEventGenerator())
					getApp().getKeyboardEventGenerator()->update();
				if (getApp().getMouseEventGenerator())
					getApp().getMouseEventGenerator()->update();
				if (mQuitByKeyEnabled && getApp().getKeyboard()->isKeyDown( mQuitKC ))
					requestQuit();
			}

			onFrame(elapsed);

			if (mPWorld.get())
				mPWorld->step( elapsed );

			if (mGWorld.get())
				mGWorld->render( elapsed );

			lastTime = now;
		}
	}
	void RtMainState::onExit()
	{
		destroyScene();
	}
	void RtMainState::createScene()
	{
		// set up default camera and viewport
		mDefaultCamera = onCreateDefaultCamera();
		YAKE_ASSERT( mDefaultCamera );
		if (!mDefaultCamera)
		{
			YAKE_LOG_WARNING("No default camera created. => No default viewport will be created.");
		}
		else
		{
			graphics::IViewport* pVP = onCreateDefaultViewport();
			YAKE_ASSERT( pVP );
			if (pVP)
			{
				mViewports.push_back( SharedPtr<graphics::IViewport>(pVP) );
			}
			else
				YAKE_LOG_WARNING("Could not create default viewport!");
		}

		// set up user defined scene
		onCreateScene();
	}
	void RtMainState::destroyScene()
	{
		onDestroyScene();

		mPWorld.reset();
		mAWorld.reset();
		mGWorld.reset();
		mViewports.clear();
		YAKE_SAFE_DELETE( mDefaultCamera );
	}
	graphics::IWorld* RtMainState::getGraphicalWorld()
	{
		if (!mGWorld.get())
		{
			graphics::IGraphicsSystem* pSys = getApp().getGraphicsSystem();
			YAKE_ASSERT( pSys );
			mGWorld = pSys->createWorld();
		}
		if (!mGWorld.get())
			YAKE_EXCEPT("We really need a valid graphical world here! Bailing out!");
		getApp().getGraphicsSystem()->subscribeToShutdownSignal( boost::bind( &RtMainState::requestQuit, this ) );
		return mGWorld.get();
	}
	physics::IWorld* RtMainState::getPhysicalWorld()
	{
		if (!mPWorld.get())
		{
			physics::IPhysicsSystem* pSys = getApp().getPhysicsSystem();
			YAKE_ASSERT( pSys );
			mPWorld = pSys->createWorld();
		}
		if (!mPWorld.get())
			YAKE_EXCEPT("We really need a valid physical world here! Bailing out!");
		return mPWorld.get();
	}
	audio::IWorld* RtMainState::getAudioWorld()
	{
		if (!mAWorld.get())
		{
			audio::IAudioSystem* pSys = getApp().getAudioSystem();
			YAKE_ASSERT( pSys );
			mAWorld.reset( pSys->createWorld() );
		}
		if (!mAWorld.get())
			YAKE_EXCEPT("We really need a valid audio world here! Bailing out!");
		return mAWorld.get();
	}
	graphics::ICamera* RtMainState::getDefaultCamera()
	{
		return mDefaultCamera;
	}

} // namespace raf
} // namespace yake
