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
#include <yapp/raf/yakeApplication.h>
#if YAKE_RAF_USES_CEGUI == 1
#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUISystem.h>
#include <yapp/common/yakeCEGUIRendererAdapter.h>
#include <yapp/common/yakeCEGUIHelpers.h>
#ifdef YAKE_DEBUG
#	pragma comment(lib,"CEGUIBase_d.lib")
#else
#	pragma comment(lib,"CEGUIBase.lib")
#endif
#endif

namespace yake {
namespace raf {

	//
	template<class SystemType>
		SystemType* findSystem(AssocVector<String,SharedPtr<SystemType> >& systems, const String& name)
	{
		if (systems.empty())
			return 0;
		if (name.empty())
		{
			return systems.begin()->second.get();
		}
		typename AssocVector<String,SharedPtr<SystemType> >::iterator itFind = systems.find( name );
		if (itFind == systems.end())
			return 0;
		return itFind->second.get();
	}
	template<class SystemType>
		void loadAndInitSystems( const StringVector& sysNames, AssocVector<String,SharedPtr<SystemType> >& sysList )
	{
		ConstVectorIterator<StringVector> itSys( sysNames );
		while (itSys.hasMoreElements())
		{
			const String& sysName = itSys.getNext();
			YAKE_ASSERT( !sysName.empty() );
			SharedPtr<SystemType> pSys;
			try {
				if (sysName.empty())
					pSys = create_default<SystemType>();
				else
					pSys = create<SystemType>( sysName );
			}
			catch (yake::Exception& e)
			{
				YAKE_EXCEPT("raf: Cannot create system \"" + sysName + "\".\n   Exception message: " + e.getMessage() + "\n   Possible reason: The necessary plugin was not loaded.");
			}
			YAKE_ASSERT( pSys.get() );
			if (pSys.get())
				sysList.insert( std::make_pair(sysName,pSys) );
			else
				YAKE_EXCEPT("raf: Cannot use a null system: " + sysName);
		}
	}


	//
	bool runApplication(Application& theApp)
	{
		try 
		{
			if (!theApp.initialise())
				return false;
			theApp.run();
			return theApp.shutdown();
		} 
		catch (yake::Exception& e)
		{
			YAKE_LOG_ERROR("RAF catched a Yake exception!\nException Message: " + e.getMessage());
		}
		catch (...)
		{
			YAKE_LOG_ERROR("RAF catched an unhandled exception!");
		}
		return false;
	}

	Application::Application() : 
		mMachine( 0 ),
		mMainState( 0 ),
#if YAKE_RAF_USES_CEGUI == 1
		mCeguiRendererAdapter( 0 ),
		mCeguiMouseInputEnabled( false ),
		mCeguiKeyboardInputEnabled( false ),
#endif
		mKeyboard( 0 ),
		mKeyboardEventGenerator( 0 ),
		mMouse( 0 ),
		mMouseEventGenerator( 0 )
	{
	}
	Application::~Application()
	{
		YAKE_SAFE_DELETE( mMachine );
	}
	void Application::initInput()
	{
		YAKE_ASSERT( !mInputSystems.empty() );
		if (mInputSystems.empty())
			return;
		input::IInputSystem* pInput = (*mInputSystems.begin()).second.get();
		YAKE_ASSERT( pInput );
		if (!pInput)
			return;

		input::IInputSystem::DeviceEntryList devices;
		devices = pInput->getAvailableDevices();

		ConstVectorIterator<input::IInputSystem::DeviceEntryList> itDevice(devices);
		while (itDevice.hasMoreElements())
		{
			const input::IInputSystem::DeviceListEntry& device = itDevice.getNext();
			std::cout << "input device: " << device.name << std::endl;
			if (!mKeyboard && device.type == input::IDT_KEYBOARD)
			{
				mKeyboard = static_cast<input::KeyboardDevice*>(pInput->activateDevice( device.name ));
				YAKE_ASSERT( mKeyboard );
				if (mKeyboard)
				{
					mKeyboardEventGenerator = new input::KeyboardEventGenerator();
					YAKE_ASSERT( mKeyboardEventGenerator );
					mKeyboardEventGenerator->attachDevice( mKeyboard );
				}
			}
			else if (!mMouse && device.type == input::IDT_MOUSE)
			{
				mMouse = static_cast<input::MouseDevice*>(pInput->activateDevice( device.name ));
				YAKE_ASSERT( mMouse );
				if (mMouse)
				{
					mMouseEventGenerator = new input::MouseEventGenerator();
					YAKE_ASSERT( mMouseEventGenerator );
					mMouseEventGenerator->attachDevice( mMouse );
				}
			}
		}
	}
	void Application::initPhysics()
	{
	}
#if YAKE_RAF_USES_CEGUI == 1
	void Application::initCEGUI()
	{
		const String file = "ceguiOgreRendererAdapter";
		yake::base::Library* pDynLib = new yake::base::Library( file ); //@FIXME. MEMORY LEAK.
		YAKE_ASSERT( pDynLib ).debug( "Out of memory" );

		YAKE_LOG( "Loading plugin " + file );

		yake::base::YakeDynLibStartPluginFn pfnStartPlugin = (yake::base::YakeDynLibStartPluginFn)pDynLib->getSymbol( "dynlibStartPlugin" );
		YAKE_ASSERT( pfnStartPlugin ).debug( "Cannot find export in dynamic library" );

		mCeguiRendererAdapter = static_cast<ceguiadapter::RendererAdapterPlugin*>(pfnStartPlugin());
		YAKE_ASSERT( mCeguiRendererAdapter ).debug( "Plugin startup function failed" );

		//
		bool bRet = mCeguiRendererAdapter->initialise();
		YAKE_ASSERT( bRet ).debug( "Plugin initialization failed" );

		if (!bRet)
		{
			delete mCeguiRendererAdapter;
			YAKE_EXCEPT("Could not initialize GUI (CEGUI)!");
		}

		//---- Stating CEGUI

		YAKE_LOG( "Initialising CEGUI..." );
		CEGUI::System* ceguiSys = new CEGUI::System( mCeguiRendererAdapter->getRenderer() );

		using namespace CEGUI;

		YAKE_LOG( "Setting CEGUI logging level..." );
		Logger::getSingleton().setLoggingLevel( Informative );

		try
		{
			YAKE_LOG( "Loading scheme..." );
			// load scheme and set up defaults
			SchemeManager::getSingleton().loadScheme((utf8*)"TaharezLook.scheme");
			System::getSingleton().setDefaultMouseCursor((utf8*)"TaharezLook", (utf8*)"MouseArrow");
			CEGUI::MouseCursor::getSingleton().setImage(CEGUI::System::getSingleton().getDefaultMouseCursor());
			System::getSingleton().setDefaultFont((utf8*)"Tahoma-12");

			WindowManager& wmgr = WindowManager::getSingleton();

			//Window* sheet = wmgr.loadWindowLayout( "console.layout" );
			//System::getSingleton().setGUISheet(sheet);

			//CEGUI::Window* pEditBoxWnd = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"/Console/Wnd/Edit");
			//mEditBox = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().getWindow("/Console/Wnd/Edit"));
			//mListBox = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().getWindow("/Console/Wnd/List"));
		}
		// catch to prevent exit (errors will be logged).
		catch( CEGUI::Exception& e)
		{
			YAKE_LOG( yake::String( "CEGUI Exception: " ) + e.getMessage().c_str() );
			YAKE_EXCEPT(yake::String( "Caught CEGUI Exception:\n") + e.getMessage().c_str() );
		}
	}
	void Application::enableKeyboardInputForCEGUI(const bool yes)
	{
		if (!mKeyboardEventGenerator)
			return;
		if (mCeguiKeyboardInputEnabled && !yes)
		{
			// disable
			ConstDequeIterator< SigConnList > it( mCeguiKeyboardSigConn );
			while (it.hasMoreElements())
				it.getNext().disconnect();
			mCeguiKeyboardSigConn.clear();
		}
		else if (!mCeguiKeyboardInputEnabled && yes)
		{
			// enable
			mCeguiKeyboardInputEnabled = true;
			mCeguiKeyboardSigConn.push_back(
				getKeyboardEventGenerator()->subscribeToKeyDown( Bind1( &Application::onKeyDown, this ) ) );
			mCeguiKeyboardSigConn.push_back(
				getKeyboardEventGenerator()->subscribeToKeyUp( Bind1( &Application::onKeyUp, this ) ) );
		}
	}
	void Application::enableMouseInputForCEGUI(const bool yes)
	{
		if (!mMouseEventGenerator)
			return;
		if (mCeguiMouseInputEnabled && !yes)
		{
			// disable
			ConstDequeIterator< SigConnList > it( mCeguiMouseSigConn );
			while (it.hasMoreElements())
				it.getNext().disconnect();
			mCeguiMouseSigConn.clear();
		}
		else if (!mCeguiMouseInputEnabled && yes)
		{
			// enable
			mCeguiMouseInputEnabled = true;
			mCeguiMouseSigConn.push_back(
				getMouseEventGenerator()->subscribeToMouseButtonDown( Bind1( &Application::onMBDown, this ) ) );
			mCeguiMouseSigConn.push_back(
				getMouseEventGenerator()->subscribeToMouseButtonUp( Bind1( &Application::onMBUp, this ) ) );
			mCeguiMouseSigConn.push_back(
				getMouseEventGenerator()->subscribeToMouseMoved( Bind1( &Application::onMouseMoved, this ) ) );
		}
	}
	void Application::enableInputForCEGUI(const bool keyb /*= true*/, const bool mouse /*= true*/)
	{
		enableKeyboardInputForCEGUI( keyb );
		enableMouseInputForCEGUI( mouse );
	}
	void Application::onKeyDown( const yake::input::KeyboardEvent& rEvent )
	{
		YAKE_ASSERT( mCeguiKeyboardInputEnabled ).debug("Callback should not be enabled!");
		if (!mCeguiKeyboardInputEnabled)
			return;
		if (CEGUI::System::getSingletonPtr())
		{
			CEGUI::System::getSingleton().injectKeyDown( rEvent.keyCode );
			long modifiers = getKeyboard()->isKeyDown(input::KC_LSHIFT) || getKeyboard()->isKeyDown(input::KC_RSHIFT) ? 1 : 0;
			CEGUI::System::getSingleton().injectChar( getKeyChar( rEvent.keyCode, modifiers ) );
		}
	}
	void Application::onKeyUp( const yake::input::KeyboardEvent& rEvent )
	{
		YAKE_ASSERT( mCeguiKeyboardInputEnabled ).debug("Callback should not be enabled!");
		if (!mCeguiKeyboardInputEnabled)
			return;
		if (CEGUI::System::getSingletonPtr())
			CEGUI::System::getSingleton().injectKeyUp( rEvent.keyCode );
	}
	void Application::onMBDown( uint8 btn )
	{
		YAKE_ASSERT( mCeguiMouseInputEnabled ).debug("Callback should not be enabled!");
		if (!mCeguiMouseInputEnabled)
			return;
		if (CEGUI::System::getSingletonPtr())
			CEGUI::System::getSingleton().injectMouseButtonDown( convertYakeButtonToCegui( btn ) );
	}
	void Application::onMBUp( uint8 btn )
	{
		YAKE_ASSERT( mCeguiMouseInputEnabled ).debug("Callback should not be enabled!");
		if (!mCeguiMouseInputEnabled)
			return;
		if (CEGUI::System::getSingletonPtr())
			CEGUI::System::getSingleton().injectMouseButtonUp( convertYakeButtonToCegui( btn ) );
	}
	void Application::onMouseMoved( const Vector3& rDelta )
	{
		YAKE_ASSERT( mCeguiMouseInputEnabled ).debug("Callback should not be enabled!");
		if (!mCeguiMouseInputEnabled)
			return;
		const real scale = 1;
		if (CEGUI::System::getSingletonPtr())
			CEGUI::System::getSingleton().injectMouseMove( rDelta.x*scale, rDelta.y*scale );
	}
#endif
	bool Application::initialise()
	{
		ApplicationConfiguration& cfg = getConfiguration();

		mUseExtGraphicsSystem = cfg.useExternalGraphicsSystem().get() != 0;

		// load libs
		StringVector libs = cfg.getLibraries();
		ConstVectorIterator<StringVector> itLib( libs );
		while (itLib.hasMoreElements())
		{
			loadLibrary(itLib.getNext());
		}

		// load & init systems
		loadAndInitSystems<scripting::IScriptingSystem>( cfg.getScriptingSystems(), mScriptingSystems );
		loadAndInitSystems<audio::IAudioSystem>( cfg.getAudioSystems(), mAudioSystems );
		if (mUseExtGraphicsSystem)
			mGraphicsSystems["default"] = cfg.useExternalGraphicsSystem();
		else
			loadAndInitSystems<graphics::IGraphicsSystem>( cfg.getGraphicsSystems(), mGraphicsSystems );
		loadAndInitSystems<physics::IPhysicsSystem>( cfg.getPhysicsSystems(), mPhysicsSystems );
		loadAndInitSystems<input::IInputSystem>( cfg.getInputSystems(), mInputSystems );
		if (!mInputSystems.empty())
			initInput();
		if (!mPhysicsSystems.empty())
			initPhysics();

#if YAKE_RAF_USES_CEGUI == 1
		// CEGUI
		if (cfg.loadCEGUI())
		{
			initCEGUI();
		}
#endif

		// application state machine
		mMachine = new AppMachine( state::TH_RELAXED );
		YAKE_ASSERT( mMachine );
		if (!mMachine)
			return false;
		mMainState = createMainState();
		YAKE_ASSERT( mMainState );
		if (!mMainState)
			return false;
		mMachine->addState( "main", mMainState );

		// user application init
		if (!onInitialise())
		{
			// clean up
			return false;
		}
		return true;
	}
	bool Application::shutdown()
	{
		bool ret = onShutdown();

		YAKE_ASSERT( mMachine );
		if (mMachine)
		{
			mMachine->exitAll();
			YAKE_SAFE_DELETE( mMachine );
		}
#if YAKE_RAF_USES_CEGUI == 1
		if (mCeguiRendererAdapter)
		{
			if ( CEGUI::System::getSingletonPtr() )
				delete CEGUI::System::getSingletonPtr();
			mCeguiRendererAdapter->shutdown();
			YAKE_SAFE_DELETE( mCeguiRendererAdapter );
		}
#endif
		YAKE_SAFE_DELETE( mKeyboardEventGenerator );
		YAKE_SAFE_DELETE( mMouseEventGenerator );
		mInputSystems.clear();
		mGraphicsSystems.clear();
		mPhysicsSystems.clear();
		mAudioSystems.clear();
		mScriptingSystems.clear();
		mLibs.clear();

		return ret;
	}
	bool Application::run()
	{
		return onRun();
	}
	bool Application::onRun()
	{
		YAKE_ASSERT( mMachine );
		if (!mMachine)
			return false;
		mMachine->changeTo("main");
		mMachine->step();
		return true;
	}
	AppMachine* Application::getMachine()
	{
		return mMachine;
	}
	ApplicationState* Application::getMainState()
	{
		return mMainState;
	}
	base::Library* Application::loadLibrary( const String& file )
	{
		YAKE_ASSERT( !file.empty() );
		if (file.empty())
			return 0;
		
		SharedPtr<base::Library> pDynLib( new base::Library( file ) );
		YAKE_ASSERT( pDynLib ).debug( "Out of memory." );
		if (!pDynLib)
			return 0;

		mLibs.push_back( pDynLib );
		return pDynLib.get();
	}
	scripting::IScriptingSystem* Application::getScriptingSystem(const String& name)
	{
		return findSystem<scripting::IScriptingSystem>( mScriptingSystems, name );
	}
	input::IInputSystem* Application::getInputSystem(const String& name)
	{
		return findSystem<input::IInputSystem>( mInputSystems, name );
	}
	graphics::IGraphicsSystem* Application::getGraphicsSystem(const String& name)
	{
		if (name.empty() && mUseExtGraphicsSystem)
			return findSystem<graphics::IGraphicsSystem>( mGraphicsSystems, "default" );
		return findSystem<graphics::IGraphicsSystem>( mGraphicsSystems, name );
	}
	physics::IPhysicsSystem* Application::getPhysicsSystem(const String& name)
	{
		return findSystem<physics::IPhysicsSystem>( mPhysicsSystems, name );
	}
	audio::IAudioSystem* Application::getAudioSystem(const String& name)
	{
		return findSystem<audio::IAudioSystem>( mAudioSystems, name );
	}
	input::KeyboardDevice* Application::getKeyboard()
	{
		return mKeyboard;
	}
	input::KeyboardEventGenerator* Application::getKeyboardEventGenerator()
	{
		return mKeyboardEventGenerator;
	}
	input::MouseDevice* Application::getMouse()
	{
		return mMouse;
	}
	input::MouseEventGenerator* Application::getMouseEventGenerator()
	{
		return mMouseEventGenerator;
	}

} // namespace raf
} // namespace yake
