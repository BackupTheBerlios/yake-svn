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
#ifndef YAKE_RAF_RTAPPLICATIONSTATE_H
#define YAKE_RAF_RTAPPLICATIONSTATE_H

#include "yake/input/yakeInputSystem.h"

namespace yake {
namespace raf {
	class Application;
	class MainState;

	/** Typical realtime application main state */
	class YAKE_RAF_API RtMainState : public MainState
	{
	public:
		RtMainState(Application& owner);
		void requestQuit();
		bool quitRequested() const;
		void enableInstantQuitByKey( input::KeyCode kc );
	protected:
		// main message handlers to override

		virtual void onEnter();
		virtual void onStep();
		virtual void onExit();
	private:
		// setup functions

		void createScene();
		void destroyScene();
		void createCameraViewportPair();

	protected:
		// message handlers to override by derived classes
		virtual void onCreateScene() {}
		virtual graphics::ICamera* onGetDefaultCamera() { return 0; }
		virtual graphics::ICamera* onCreateDefaultCamera();
		virtual graphics::IViewport* onGetDefaultViewport() { return 0; }
		virtual graphics::IViewport* onCreateDefaultViewport();
		virtual SharedPtr<graphics::IWorld> onGetDefaultWorld() { return SharedPtr<graphics::IWorld>(); }
		virtual void onDestroyScene() {}
		virtual void onFrame(const real timeElapsed) {}
		virtual bool onQuitRequested() { return true; }

		// other ...
		graphics::IWorld* getGraphicalWorld();
		audio::IWorld* getAudioWorld();
		physics::IWorld* getPhysicalWorld();

		graphics::ICamera* getDefaultCamera();
	private:
		bool						mQuitRequested;
		SharedPtr<graphics::IWorld>	mGWorld;
		SharedPtr<audio::IWorld>	mAWorld;
		SharedPtr<physics::IWorld>	mPWorld;

		typedef std::vector<SharedPtr<graphics::IViewport> > ViewportList;
		ViewportList				mViewports;
		graphics::ICamera*			mDefaultCamera;

		input::KeyCode				mQuitKC;
		bool						mQuitByKeyEnabled;
	};

} // namespace raf
} // namespace yake

#endif
