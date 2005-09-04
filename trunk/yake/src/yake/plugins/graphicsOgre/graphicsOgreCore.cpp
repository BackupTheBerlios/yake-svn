/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#include <yake/plugins/graphicsOgre/yakePCH.h>

#if YAKE_PLATFORM == PLATFORM_WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

//#define YAKE_OGREPLUGIN_NO_CONFIG_DIALOG

#if defined( YAKE_OGREPLUGIN_NO_CONFIG_DIALOG )
#include "../../rendersystems/direct3d9/include/OgreD3D9RenderSystem.h"
#endif
#ifdef OGRE_MEMORY_MACROS
#pragma message("disabling OGRE's memory macros")
#	include <OgreNoMemoryMacros.h>
#endif
#include <OgreMemoryMacros.h>

#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>

using namespace Ogre;

namespace yake {
namespace graphics {
namespace ogre3d {

	class SystemFrameListener : public Ogre::FrameListener
	{
	public:
		SystemFrameListener(Ogre::RenderWindow* window) : FrameListener(), mRWin(window)
		{
		}

		virtual bool frameStarted(const FrameEvent & evt)
		{
			// nothin' to do... getting a beer.
			return true;
		}

		virtual bool frameEnded(const FrameEvent & evt)
		{
			return true;
		}
	protected:
		Ogre::RenderWindow*		mRWin;
	};


	OgreCore::OgreCore(const bool bWindowAlreadyCreated /*= false*/,
				const bool bShutdownOgre /*= true*/,
				const bool bParseDefaultResourceFile /*= true*/,
				Ogre::RenderWindow* pRenderWindow /*= 0*/) :
		mShutdownOgre( bShutdownOgre ),
		mSysFL( 0 ),
		mRoot( 0 )
	{
		mReady = false;
		try {

			if (!bWindowAlreadyCreated)
			{
				getConfig();

				mRoot = new Root(
					Ogre::String("yake.graphics.ogre_plugins.cfg"),
					Ogre::String("yake.graphics.ogre.cfg"),
					Ogre::String("yake.graphics.ogre.log") );

				if (bParseDefaultResourceFile)
					setupResourcesFromConfigFile();

// todo: we don't want to choose this at compilation time
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
					if ( str.find("GL") )
						found = true;
					++it;
				}
				if (!found)
				{
					mReady = false;
					return;
				}

				Root::getSingleton().setRenderSystem(mRSys);
				D3D9RenderSystem *pRSys9 = (D3D9RenderSystem *)Root::getSingleton().getRenderSystem();
				pRSys9->initConfigOptions();
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
				mRWin = mRoot->initialise(true, "YAKE Demo");

				_chooseSceneManager();

				TextureManager::getSingleton().setDefaultNumMipmaps( 0 );
				Animation::setDefaultInterpolationMode(Animation::IM_SPLINE);

				// Initialise, parse scripts etc.

				mRoot->getRenderSystem()->_initRenderTargets();

				ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
			}
			else
			{	// bWindowAlreadyCreated == true
				mRoot = Root::getSingletonPtr();
				mRSys = Root::getSingleton().getRenderSystem();

				YAKE_ASSERT( pRenderWindow );
				mRWin = pRenderWindow;
				ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
				_chooseSceneManager();
				if (bParseDefaultResourceFile)
					setupResourcesFromConfigFile();
			}

			mSysFL = new SystemFrameListener( mRWin );
			mRoot->addFrameListener( mSysFL );

			mReady = true;
		}
		catch (Ogre::Exception& e)
		{
			//YAKE_LOGPRINTF("[yake.graphics.ogre] OGRE EXCEPTION\n%s\n", e.getFullDescription().c_str() );
			mReady = false;
			YAKE_GRAPHICS_EXCEPT2(e.getFullDescription().c_str(), "[yake.graphics.ogre] OGRE EXCEPTION");
		}
	}

	void OgreCore::_chooseSceneManager()
	{
		StringMap::const_iterator itFind = mConfig.find("scenemanager");
		if (itFind == mConfig.end())
			mSceneMgr = mRoot->getSceneManager( ST_GENERIC );
		else
		{
			String val = itFind->second;
			if (val == "exterior_close")
				mSceneType = ST_EXTERIOR_CLOSE;
			else if (val == "exterior_far")
				mSceneType = ST_EXTERIOR_FAR;
			else if (val == "exterior_real_far")
				mSceneType = ST_EXTERIOR_REAL_FAR;
			else
			{
				YAKE_ASSERT( 1==0 ).warning("unknown scene manager config option. Using fallback: generic.");
				mSceneType = ST_GENERIC;
			}
			mSceneMgr = mRoot->getSceneManager( mSceneType );
		}
		YAKE_ASSERT( mSceneMgr );
	}

	OgreCore::~OgreCore()
	{
		try {
			if (mSysFL)
			{
				mRoot->removeFrameListener( mSysFL );
				YAKE_SAFE_DELETE(mSysFL);
			}
			if (mShutdownOgre)
			{
				YAKE_SAFE_DELETE(mRoot);
			}
			else
				mRoot = 0;
		}
		catch (Ogre::Exception& e)
		{
			YAKE_LOG( "[yake.graphics.ogre] OGRE EXCEPTION\n" + e.getFullDescription() + "\n");
			mReady = false;
		}
	}

	void OgreCore::setupResourcesFromConfigFile()
	{
		// Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load("yake.graphics.ogre_resources.cfg");

        // Go through all sections & settings in the file
        ConfigFile::SectionIterator seci = cf.getSectionIterator();

		Ogre::String secName, typeName, archName;
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			}
		}
	}

	void OgreCore::getConfig()
	{
		// Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load("yake.graphics.ogre_config.cfg");

		// Go through all settings in the file
		Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator();

		String key, value;
		while (i.hasMoreElements())
		{
			key = i.peekNextKey();
			value = i.getNext();
			mConfig.insert( std::make_pair( key, value ) );
		}
	}

	void OgreCore::update( real timeElapsed )
	{
		YAKE_DECLARE_FUNCTION( update );
		//HACK/FIXME
		static bool bFirstTime = true;
		if (bFirstTime)
		{
			bFirstTime = false;
			if (mSceneType == ST_EXTERIOR_CLOSE)
			{
				// Fog
				// NB it's VERY important to set this before calling setWorldGeometry
				// because the vertex program picked will be different
				ColourValue fadeColour(0.93, 0.86, 0.76);
				mSceneMgr->setFog( FOG_LINEAR, fadeColour, .001, 500, 1000);
				mRWin->getViewport(0)->setBackgroundColour(fadeColour);

				StringMap::const_iterator itFind = mConfig.find("world");
				if (itFind != mConfig.end())
				{
					mSceneMgr->setWorldGeometry( itFind->second );
				}
			}
			mSceneMgr->setAmbientLight( ColourValue(0.2,0.2,0.2) );
		}

		//if (mReady && mRWin && mRSys)
		{
			try {
#if YAKE_PLATFORM == PLATFORM_WIN32
				// Pump events on Win32
    			MSG  msg;
				while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				//if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
#endif

				if (!mRoot->renderOneFrame())
					mShutdownSignal();
				if ( mRWin->isClosed() )
					mShutdownSignal();
			}
			catch (Ogre::Exception& e)
			{
				//YAKE_LOGPRINTF("[yake.graphics.ogre] OGRE EXCEPTION\n%s\n", e.getFullDescription() );
				String rMsg = "[yake.graphics.ogre] OGRE EXCEPTION\n" + e.getFullDescription() + "\n";
				YAKE_LOG( rMsg );
				YAKE_EXCEPT( rMsg );
				mReady = false;
			}
		} // if (mReady && mRWin && mRSys){
	}

	void OgreCore::subscribeToShutdownSignal( const IGraphicsSystem::ShutdownSignal::slot_type & rSlot )
	{
		mShutdownSignal.connect( rSlot );
	}

}
}
}
