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
#ifndef YAKE_GRAPHICSSYSTEMOGRE_CORE_H
#define YAKE_GRAPHICSSYSTEMOGRE_CORE_H

namespace Ogre {
	class SceneManager;
	class Root;
	class RenderWindow;
	class RenderSystem;
	class Camera;
	class Viewport;
	class OgreWindowWin32;
}

namespace yake {
namespace graphics {
namespace ogre3d {

	class SystemFrameListener;
	class OgreCore
	{
	public:
		OgreCore(const bool bWindowAlreadyCreated = false,
				const bool bShutdownOgre = true,
				const bool bParseDefaultResourceFile = true,
				Ogre::RenderWindow* pRenderWindow = 0);
		~OgreCore();

		void update( real timeElapsed );

		void _start();

		inline Ogre::SceneManager* getSceneMgr() const { return mSceneMgr; }
		inline Ogre::Root* getRoot() const { return mRoot; }
		inline Ogre::RenderWindow* getRenderWindow() const { return mRWin; }
		inline Ogre::ParticleSystemManager* getParticleSysMgr() const 
		{ return Ogre::ParticleSystemManager::getSingletonPtr(); }

		void subscribeToShutdownSignal( const IGraphicsSystem::ShutdownSignal::slot_type & rSlot );

	protected:
		void setupResourcesFromConfigFile();

		void _chooseSceneManager();

		void getConfig();

	protected:
		bool					mReady;
		bool					mShutdownOgre;

		Ogre::Root*				mRoot;
		Ogre::RenderSystem*		mRSys;
		Ogre::RenderWindow*		mRWin;
		Ogre::OgreWindowWin32*	mOWin;

		Ogre::SceneManager*		mSceneMgr;

		SystemFrameListener*	mSysFL;

		typedef AssocVector<base::String,base::String> StringMap;
		StringMap				mConfig;

		Ogre::SceneType			mSceneType;

		IGraphicsSystem::ShutdownSignal mShutdownSignal;
	};

}
}
}

#endif
