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
#include <plugins/yakeGraphicsOgre/inc/pch.h>

#if YAKE_PLATFORM == PLATFORM_WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

//#define YAKE_OGREPLUGIN_NO_CONFIG_DIALOG

#if defined( YAKE_OGREPLUGIN_NO_CONFIG_DIALOG )
#include "../../rendersystems/direct3d9/include/OgreD3D9RenderSystem.h"
#endif

#include <plugins/yakeGraphicsOgre/inc/graphicsOgreCore.h>

using namespace Ogre;

namespace yake {
	namespace graphics {

	void updateStats(Ogre::RenderWindow* window)
    {
        static String currFps = "Current FPS: ";
        static String avgFps = "Average FPS: ";
        static String bestFps = "Best FPS: ";
        static String worstFps = "Worst FPS: ";
        static String tris = "Triangle Count: ";

        // update stats when necessary
        GuiElement* guiAvg = GuiManager::getSingleton().getGuiElement("Core/AverageFps");
        GuiElement* guiCurr = GuiManager::getSingleton().getGuiElement("Core/CurrFps");
        GuiElement* guiBest = GuiManager::getSingleton().getGuiElement("Core/BestFps");
        GuiElement* guiWorst = GuiManager::getSingleton().getGuiElement("Core/WorstFps");

		guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(window->getAverageFPS()));
        guiCurr->setCaption(currFps + Ogre::StringConverter::toString(window->getLastFPS()));
        guiBest->setCaption(bestFps + Ogre::StringConverter::toString(window->getBestFPS())
            +" "+Ogre::StringConverter::toString(window->getBestFrameTime())+" ms");
        guiWorst->setCaption(worstFps + Ogre::StringConverter::toString(window->getWorstFPS())
            +" "+Ogre::StringConverter::toString(window->getWorstFrameTime())+" ms");

        GuiElement* guiTris = GuiManager::getSingleton().getGuiElement("Core/NumTris");
        guiTris->setCaption(tris + Ogre::StringConverter::toString(window->getTriangleCount()));

        GuiElement* guiDbg = GuiManager::getSingleton().getGuiElement("Core/DebugText");
        guiDbg->setCaption(window->getDebugText());
    }

	class SystemFrameListener : public Ogre::FrameListener
	{
	public:
		SystemFrameListener(Ogre::RenderWindow* window) : FrameListener(), mRWin(window)
		{
		}

		virtual bool frameStarted(const FrameEvent & evt)
		{
			updateStats( mRWin );
			return true;
		}

		virtual bool frameEnded(const FrameEvent & evt)
		{
			return true;
		}
	protected:
		Ogre::RenderWindow*		mRWin;
	};


	OgreCore::OgreCore()
	{
		mReady = false;
		try {
			mRoot = new Root("yake.graphics.ogre_plugins.cfg", "yake.graphics.ogre.cfg", "yake.graphics.ogre.log");

			setupResourcesFromConfigFile();

#if defined( YAKE_OGREPLUGIN_NO_CONFIG_DIALOG )
			RenderSystemList *rl = Root::getSingleton().getAvailableRenderers();
			RenderSystemList::const_iterator it = rl->begin();
			if (rl->empty())
			{
				mReady = false;
				return;
			}

			bool found = false;
			while (!found && it != rl->end())
			{
				mRSys = (*it);
				String str = mRSys->getName().c_str();
				//if (str.find("3D9"))
				if (str.find("GL"))
					found = true;
				++it;
			}
			if (!found)
			{
				mReady = false;
				return;
			}

			Root::getSingleton().setRenderSystem(mRSys);
			D3D9RenderSystem *mRSys9 = (D3D9RenderSystem *)Root::getSingleton().getRenderSystem();
			mRSys9->initConfigOptions();
			mRSys->setConfigOption("Anti aliasing", "None");
			mRSys->setConfigOption("Full Screen", "No");
			mRSys->setConfigOption("VSync", "No");
			mRSys->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
#else
			if (!mRoot->showConfigDialog())
			{
				return;
			}
#endif
			mRWin = mRoot->initialise(true);

			_chooseSceneManager();

			TextureManager::getSingleton().setDefaultNumMipMaps( 0 );
			Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);

			Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");
			if (o)
				o->show();
	
			//...

			mSysFL = new SystemFrameListener( mRWin );
			mRoot->addFrameListener( mSysFL );

			mRoot->getRenderSystem()->_initRenderTargets();

			mReady = true;
		}
		catch (Ogre::Exception& e)
		{
			YAKE_LOGPRINTF("[yake.graphics.ogre] OGRE EXCEPTION\n%s\n", e.getFullDescription() );
			mReady = false;
		}
	}

	void OgreCore::_chooseSceneManager()
	{
		mSceneMgr = mRoot->getSceneManager( ST_GENERIC );
		mSceneMgr->setAmbientLight( ColourValue(0.5,0.5,0.7) );
	}

	OgreCore::~OgreCore()
	{
		try {
			if (mSysFL)
			{
				mRoot->removeFrameListener( mSysFL );
				YAKE_SAFE_DELETE(mSysFL);
			}
			YAKE_SAFE_DELETE(mRoot);
		}
		catch (Ogre::Exception& e)
		{
			YAKE_LOGPRINTF("[yake.graphics.ogre] OGRE EXCEPTION\n%s\n", e.getFullDescription() );
			mReady = false;
		}
	}

	void OgreCore::setupResourcesFromConfigFile()
	{
		// Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load("yake.graphics.ogre_resources.cfg");

		// Go through all settings in the file
		Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator();

		String typeName, archName;
		while (i.hasMoreElements())
		{
			typeName = i.peekNextKey();
			archName = i.getNext();
			Ogre::ResourceManager::addCommonArchiveEx(archName, typeName);
		}
	}

	void OgreCore::update( real timeElapsed )
	{
		//if (mReady && mRWin && mRSys)
		{
			Ogre::FrameEvent evt;
			evt.timeSinceLastFrame = timeElapsed;

			try {

#if YAKE_PLATFORM == PLATFORM_WIN32
				// Pump events on Win32
    			MSG  msg;
				while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
#endif

				mRoot->_fireFrameStarted( evt );
				mRWin->update();
				mRoot->getRenderSystem()->_updateAllRenderTargets();
				mRoot->_fireFrameEnded();
			}
			catch (Ogre::Exception& e)
			{
				YAKE_LOGPRINTF("[yake.graphics.ogre] OGRE EXCEPTION\n%s\n", e.getFullDescription() );
				mReady = false;
			}
		} // if (mReady && mRWin && mRSys){
	}

	}
}
