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
		AssocVector<String,SharedPtr<SystemType> >::iterator itFind = systems.find( name );
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
	bool Application::initialise()
	{
		ApplicationConfiguration& cfg = getConfiguration();

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
		loadAndInitSystems<graphics::IGraphicsSystem>( cfg.getGraphicsSystems(), mGraphicsSystems );
		loadAndInitSystems<physics::IPhysicsSystem>( cfg.getPhysicsSystems(), mPhysicsSystems );
		loadAndInitSystems<input::IInputSystem>( cfg.getInputSystems(), mInputSystems );
		if (!mInputSystems.empty())
			initInput();
		if (!mPhysicsSystems.empty())
			initPhysics();

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
