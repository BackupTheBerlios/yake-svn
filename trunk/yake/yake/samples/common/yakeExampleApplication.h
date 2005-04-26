#ifndef YAKE_EXAMPLE_APPLICATION_H
#define YAKE_EXAMPLE_APPLICATION_H

/**
*/
#include <yake/graphics/yakeGraphics.h>
#include <yake/physics/yakePhysics.h>
#include <yake/scripting/yakeScriptingSystem.h>
#include <yake/input/yakeInput.h>
#include <yake/input/yakeInputEventGenerator.h>
#include <yake/audio/yakeAudio.h>
#include <iostream>

namespace yake {
	using namespace base::templates;
namespace exapp {

// todo: kick the plugin approach or combine it with class registry
// cleanup this subsystem mess
	class ExampleApplication
	{
	private:
		typedef Vector< base::Plugin* >	PluginList;
		PluginList								mPlugins;
		typedef Vector< SharedPtr<base::Library> > LibList;
		LibList									mLibs;
		SharedPtr< graphics::IGraphicsSystem >	mGraphicsSystem;
		SharedPtr< physics::IPhysicsSystem >	mPhysicsSystem;
		SharedPtr< scripting::IScriptingSystem>	mScriptingSystem;
		SharedPtr< scripting::IBinder >			mScriptingBindings;
		SharedPtr< input::IInputSystem >		mInputSystem;
		SharedPtr< audio::IAudioSystem >		mAudioSystem;
		bool									mShutdownRequested;
	protected:
		input::KeyboardEventGenerator			mKeyboardEventGenerator;
		input::MouseEventGenerator				mMouseEventGenerator;

		// while yake can handle more than one keyboard or mouse we don't do that ...
		input::KeyboardDevice*					mKeyboard;
		input::MouseDevice*						mMouse;

		bool									mLoadGraphicsSystem;
		bool									mLoadPhysicsSystem;
		bool									mLoadScriptingSystem;
		bool									mLoadInputSystem;
		bool									mLoadScriptingBindings;
		bool									mLoadAudioSystem;
	public:
		ExampleApplication(bool loadGraphics, bool loadPhysics, bool loadScripting, bool loadInput, bool loadScriptingBindings, bool loadAudio ) :
				mShutdownRequested(false), 
				mLoadGraphicsSystem( loadGraphics ),
				mLoadPhysicsSystem( loadPhysics ), 
				mLoadScriptingSystem( loadScripting ), 
				mLoadInputSystem( loadInput ), 
				mLoadScriptingBindings( loadScriptingBindings ),
				mLoadAudioSystem( loadAudio )
		{
		}

		virtual ~ExampleApplication()
		{
			destroyAllSystems();
			unloadAllPlugins();
		}

		void requestShutdown()
		{
			mShutdownRequested = true;
		}

		bool shutdownRequested() const
		{
			return mShutdownRequested;
		}

		const input::KeyboardDevice* getKeyboard() const
		{
			return mKeyboard;
		}

		const input::MouseDevice* getMouse() const
		{
			return mMouse;
		}

		graphics::IGraphicsSystem& getGraphicsSystem() const
		{
			if (!mGraphicsSystem)
				YAKE_EXCEPT( "Don't have a graphics system!", "getGraphicsSystem()" );
			return *mGraphicsSystem;
		}

		scripting::IScriptingSystem& getScriptingSystem() const
		{
			if (!mScriptingSystem)
				YAKE_EXCEPT( "Don't have a scripting system!", "getScriptingSystem()" );
			return *mScriptingSystem;
		}

		physics::IPhysicsSystem& getPhysicsSystem() const
		{ return *mPhysicsSystem; }

		scripting::IBinder& getScriptingBindings() const
		{
			if (!mScriptingBindings)
				YAKE_EXCEPT( "Don't have scripting bindings!", "getScriptingBindings()" );
			return *mScriptingBindings;
		}

		input::IInputSystem& getInputSystem() const
		{ return *mInputSystem; }

		audio::IAudioSystem* getAudioSystem() const
		{
			YAKE_ASSERT( mAudioSystem.get() );
			return mAudioSystem.get();
		}

		virtual void initialise()
		{
			
			// scripting
			if ( mLoadScriptingSystem )
			{
				SharedPtr<base::Library> pLib = loadLib("scriptingBindingsLua" );
				YAKE_ASSERT( pLib ).debug("Cannot load scripting bindings plugin.");

				mScriptingSystem = create_default< scripting::IScriptingSystem >();
				YAKE_ASSERT( mScriptingSystem ).error("Cannot create scripting system.");
			}

			// scripting bindings
			if ( mLoadScriptingBindings )
			{
				SharedPtr<base::Library> pLib = loadLib("scriptingBindingsLua" );
				YAKE_ASSERT( pLib ).debug("Cannot load scripting bindings plugin.");

				mScriptingBindings = create_default< scripting::IBinder >();
				YAKE_ASSERT( mScriptingBindings ).error("Cannot create scripting bindings object.");
			}

			// graphics
			if (mLoadGraphicsSystem)
			{
				SharedPtr<base::Library> pLib = loadLib("graphicsOgre" );
				YAKE_ASSERT( pLib ).debug("Cannot load graphics plugin.");

				mGraphicsSystem = create_default< graphics::IGraphicsSystem >();
				// ... or alternatively we can create a graphics system by name:
				//mGraphicsSystem = create< graphics::IGraphicsSystem >("ogre3d");

				mGraphicsSystem->subscribeToShutdownSignal( boost::bind( &ExampleApplication::requestShutdown, this ) );
			}

			// physics
			if (mLoadPhysicsSystem)
			{
				SharedPtr<base::Library> pLib = loadLib("physicsNX" );
				YAKE_ASSERT( pLib ).debug("Cannot load graphics plugin.");

				mPhysicsSystem = create_default< physics::IPhysicsSystem >();
				YAKE_ASSERT( mPhysicsSystem ).error("Cannot create physics system.");
			}

			// input
			if (mLoadInputSystem)
			{
				SharedPtr<base::Library> pLib = loadLib("inputOgre" );
				YAKE_ASSERT( pLib ).debug("Cannot load input plugin.");

				mInputSystem = create_default< input::IInputSystem >();
				YAKE_ASSERT( mInputSystem ).error("Cannot create input system.");

				setupInput();
			}

			// audio
			if (mLoadAudioSystem)
			{
				SharedPtr<base::Library> pLib = loadLib( "audioOpenAL" );
				YAKE_ASSERT( pLib ).debug("Cannot load audio plugin.");

				mAudioSystem = create_default< audio::IAudioSystem >();
				//OR: mAudioSystem.reset( create< audio::IAudioSystem >("openalpp") );
			}
		}

		virtual void run() = 0;

	private:
		void setupInput()
		{
			YAKE_ASSERT( mInputSystem );
			input::IInputSystem::DeviceEntryList devs;
			devs = getInputSystem().getAvailableDevices();

			for (input::IInputSystem::DeviceEntryList::const_iterator it = devs.begin(); it != devs.end(); ++it)
			{
				std::cout << "input device: " << (*it).name << std::endl;
				if ((*it).type == input::IDT_KEYBOARD)
					mKeyboard = static_cast<input::KeyboardDevice*>(getInputSystem().activateDevice( (*it).name ));
				else if ((*it).type == input::IDT_MOUSE)
					mMouse = static_cast<input::MouseDevice*>(getInputSystem().activateDevice( (*it).name ));
			}
			YAKE_ASSERT( mKeyboard );
			YAKE_ASSERT( mMouse );

			mKeyboardEventGenerator.attachDevice( mKeyboard );
			mMouseEventGenerator.attachDevice( mMouse );
		}
	protected:
		/** @todo deprecated so remove:
		template< class PluginType >
		PluginType* loadPlugin( const String & file )
		{
			yake::base::Library* pDynLib = new yake::base::Library( file );
			YAKE_ASSERT( pDynLib ).debug("Out of memory.");

			std::cout << "Loading plugin " << file << std::endl;
			yake::base::YakeDynLibStartPluginFn pfnStartPlugin = (yake::base::YakeDynLibStartPluginFn)pDynLib->getSymbol("dynlibStartPlugin");
			YAKE_ASSERT( pfnStartPlugin ).debug("Cannot find export in dynamic library.");

			PluginType* pPlugin = static_cast<PluginType*>(pfnStartPlugin());
			YAKE_ASSERT( pPlugin ).debug("Plugin startup function failed.");

			//
			bool bRet = pPlugin->initialise();
			YAKE_ASSERT( bRet ).debug("Plugin initialization failed.");
			if (!bRet)
			{
				delete pPlugin;
				return 0;
			}

			mPlugins.push_back( pPlugin );

			return pPlugin;
		}
		*/

		void unloadAllPlugins()
		{
			// better safe than sorry.
			//YAKE_SAFE_DELETE( mScriptingBindings );
			destroyAllSystems();

			// destroy plugins in reverse order of initialitation
			for (PluginList::reverse_iterator it = mPlugins.rbegin(); it != mPlugins.rend(); ++it)
			{
				(*it)->shutdown();
				delete (*it);
			}
			mPlugins.clear();

			mLibs.clear();
		}

		SharedPtr<base::Library> loadLib( const String & file )
		{
			SharedPtr<base::Library> pDynLib( new base::Library( file ) );
			YAKE_ASSERT( pDynLib ).debug( "Out of memory." );
			mLibs.push_back( pDynLib );
			return pDynLib;
		}

		void destroyAllSystems()
		{
			mScriptingBindings.reset();
			mScriptingSystem.reset();
			mInputSystem.reset();
			mGraphicsSystem.reset();
			mPhysicsSystem.reset();
			mAudioSystem.reset();
		}
	};

}
}

#endif 
