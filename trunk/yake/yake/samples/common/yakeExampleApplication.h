/**
	\todo add loading of script bindings
	\todo add subscription to shutdown request by graphics system (due to closed window)
*/
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yake/physics/yakePhysicsSystem.h>
#include <yake/scripting/yakeScriptingSystem.h>
#include <yake/input/yakeInputSystem.h>
#include <yake/input/yakeInputEventGenerator.h>
#include <yake/audio/yakeAudio.h>
#include <iostream>

namespace yake {
	using namespace base::templates;
namespace exapp {

	class ExampleApplication
	{
	private:
		typedef Vector< base::Plugin* >	PluginList;
		PluginList								mPlugins;
		typedef Vector< Pointer<base::Library> > LibList;
		LibList									mLibs;
		Pointer< graphics::IGraphicsSystem >	mGraphicsSystem;
		physics::PhysicsSystem					* mPhysicsSystem;
		scripting::ScriptingSystem				* mScriptingSystem;
		SharedPtr<scripting::IBinder>			mScriptingBindings;
		input::InputSystem						* mInputSystem;
		Pointer<audio::IAudioSystem>			mAudioSystem;
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
				mLoadAudioSystem( loadAudio ),
				mScriptingSystem(0),
				mPhysicsSystem(0),
				mInputSystem(0)//, mScriptingBindings(0)
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

		scripting::ScriptingSystem& getScriptingSystem() const
		{
			if (!mScriptingSystem)
				YAKE_EXCEPT( "Don't have a scripting system!", "getScriptingSystem()" );
			return *mScriptingSystem;
		}

		physics::PhysicsSystem& getPhysicsSystem() const
		{ return *mPhysicsSystem; }

		scripting::IBinder& getScriptingBindings() const
		{
			if (!mScriptingBindings)
				YAKE_EXCEPT( "Don't have scripting bindings!", "getScriptingBindings()" );
			return *mScriptingBindings;
		}

		input::InputSystem& getInputSystem() const
		{ return *mInputSystem; }

		audio::IAudioSystem* getAudioSystem() const
		{
			YAKE_ASSERT( mAudioSystem.get() );
			return mAudioSystem.get();
		}

		virtual void initialise()
		{
			
			// scripting bindings
			if ( mLoadScriptingBindings )
			{
				scripting::ScriptingBindingsPlugin* pSBP = 
				  loadPlugin<scripting::ScriptingBindingsPlugin>( 
				  				"scriptingBindingsLua" );
				YAKE_ASSERT( pSBP ).debug("Cannot load scripting bindings plugin.");
 
				mScriptingBindings = pSBP->createBinder();
				YAKE_ASSERT( mScriptingBindings ).error(
				                              "Cannot create scripting bindings object." );
			}
			
			// scripting
			if ( mLoadScriptingSystem )
			{
				scripting::ScriptingPlugin* pSP = 
					loadPlugin<scripting::ScriptingPlugin>( 
									"scriptingLua" );
				YAKE_ASSERT( pSP ).debug( "Cannot load scripting plugin." );

				mScriptingSystem = pSP->createSystem();
				YAKE_ASSERT( mScriptingSystem ).error( "Cannot create scripting system." );
			}

			// graphics
			if (mLoadGraphicsSystem)
			{
				Pointer<base::Library> pLib = loadLib( 
								"graphicsOgre" );
				YAKE_ASSERT( pLib ).debug("Cannot load graphics plugin.");

				mGraphicsSystem = create< graphics::IGraphicsSystem >();
				// ... or alternatively we can create a graphics system by name:
				//mGraphicsSystem = create< graphics::IGraphicsSystem >("ogre3d");

				mGraphicsSystem->subscribeToShutdownSignal( boost::bind( &ExampleApplication::requestShutdown, this ) );
			}

			// physics
			if (mLoadPhysicsSystem)
			{
				physics::PhysicsPlugin* pPP = loadPlugin<physics::PhysicsPlugin>( "physicsODE" );
				YAKE_ASSERT( pPP ).debug("Cannot load physics plugin.");

				mPhysicsSystem = pPP->createSystem();
				YAKE_ASSERT( mPhysicsSystem ).error("Cannot create graphics system.");
			}

			// input
			if (mLoadInputSystem)
			{
				input::InputPlugin* pIP = loadPlugin<input::InputPlugin>( 
									"inputOgre" );
				YAKE_ASSERT( pIP ).debug("Cannot load input plugin.");

				mInputSystem = pIP->createSystem();
				YAKE_ASSERT( mInputSystem ).error("Cannot create input system.");

				setupInput();
			}

			// audio
			if (mLoadAudioSystem)
			{
				Pointer<base::Library> pLib = loadLib( "audioOpenAL" );
				YAKE_ASSERT( pLib ).debug("Cannot load audio plugin.");

				mAudioSystem = create< audio::IAudioSystem >();
				//OR: mAudioSystem.reset( create< audio::IAudioSystem >("openalpp") );
			}
		}

		virtual void run() = 0;

	private:
		void setupInput()
		{
			YAKE_ASSERT( mInputSystem );
			input::InputSystem::DeviceList devs;
			devs = getInputSystem().getAvailableDevices();

			for (input::InputSystem::DeviceList::const_iterator it = devs.begin(); it != devs.end(); ++it)
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
		template< class PluginType >
		PluginType* loadPlugin( const base::String & file )
		{
			yake::base::Library* pDynLib = new yake::base::Library( file );
			YAKE_ASSERT( pDynLib ).debug("Out of memory.");

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

		Pointer<base::Library> loadLib( const base::String & file )
		{
            Pointer<base::Library> pDynLib( new base::Library( file ) );
			YAKE_ASSERT( pDynLib ).debug( "Out of memory." );
			mLibs.push_back( pDynLib );
			return pDynLib;
		}

		void destroyAllSystems()
		{
			YAKE_SAFE_DELETE( mInputSystem );
			//YAKE_SAFE_DELETE( mScriptingBindings );
			mGraphicsSystem.reset();
			mAudioSystem.reset();
			YAKE_SAFE_DELETE( mPhysicsSystem );
			YAKE_SAFE_DELETE( mScriptingSystem );
		}
	};

}
}
