

namespace yake {
namespace exapp {

	class ExampleApplication
	{
	private:
		typedef std::vector< base::Plugin* >	PluginList;
		PluginList								mPlugins;
		graphics::GraphicsSystem				* mGraphicsSystem;
		physics::PhysicsSystem					* mPhysicsSystem;
		scripting::ScriptingSystem				* mScriptingSystem;
		//scripting::ScriptingBindings			* mScriptingBindings;
		input::InputSystem						* mInputSystem;
	protected:
		input::KeyboardEventGenerator			mKeyboardEventGenerator;
		input::MouseEventGenerator				mMouseEventGenerator;

		// while yake can handle more than one keyboard or mouse we don't do that ...
		input::KeyboardDevice*					mKeyboard;
		input::MouseDevice*						mMouse;

		bool									mLoadGraphicsSystem;
		bool									mLoadPhysicsSystem;
		bool									mLoadScriptingSystem;
	public:
		ExampleApplication(bool loadGraphics, bool loadPhysics, bool loadScripting) :
				mLoadGraphicsSystem( loadGraphics ), mLoadPhysicsSystem( loadPhysics ), mLoadScriptingSystem( loadScripting ),
				mGraphicsSystem(0), mScriptingSystem(0), mPhysicsSystem(0), mInputSystem(0)//, mScriptingBindings(0)
		{
		}

		virtual ~ExampleApplication()
		{
			//YAKE_SAFE_DELETE( mScriptingBindings );
			destroyAllSystems();
			unloadAllPlugins();
		}

		graphics::GraphicsSystem& getGraphicsSystem() const
		{
			//if (!mGraphicsSystem)
			//	YAKE_EXCEPT( "Don't have a graphics system!", "getGraphicsSystem()" );
			return *mGraphicsSystem;
		}

		scripting::ScriptingSystem& getScriptingSystem() const
		{
			//if (!mGraphicsSystem)
			//	YAKE_EXCEPT( "Don't have a scripting system!", "getScriptingSystem()" );
			return *mScriptingSystem;
		}

		physics::PhysicsSystem& getPhysicsSystem() const
		{ return *mPhysicsSystem; }

		/*scripting::ScriptingBindings& getScriptingBindings() const
		{
			if (!mScriptingBindings)
				YAKE_EXCEPT( "Don't have scripting bindings!", "getScriptingBindings()" );
			return *mScriptingBindings;
		}*/

		input::InputSystem& getInputSystem() const
		{ return *mInputSystem; }

		virtual void initialise()
		{
			// scripting
			if (mLoadScriptingSystem)
			{
				scripting::ScriptingPlugin* pSP = loadPlugin<scripting::ScriptingPlugin>( "yakeScriptingLua.dll" );
				YAKE_ASSERT( pSP ).debug("Cannot load scripting plugin.");

				mScriptingSystem = pSP->createSystem();
				YAKE_ASSERT( mScriptingSystem ).error("Cannot create scripting system.");
			}

			// scripting bindings
			/*scripting::ScriptingBindingsPlugin* pSBP = loadPlugin<scripting::ScriptingBindingsPlugin>( "yakeScriptingLuaBindings.dll" );
			YAKE_ASSERT( pSBP ).debug("Cannot load scripting bindings plugin.");

			mScriptingBindings = pSBP->createSystem();
			YAKE_ASSERT( mScriptingBindings ).error("Cannot create scripting bindings object.");*/

			// graphics
			if (mLoadGraphicsSystem)
			{
				graphics::GraphicsPlugin* pGP = loadPlugin<graphics::GraphicsPlugin>( "yakeGraphicsOGRE.dll" );
				YAKE_ASSERT( pGP ).debug("Cannot load graphics plugin.");

				mGraphicsSystem = pGP->createSystem();
				YAKE_ASSERT( mGraphicsSystem ).error("Cannot create graphics system.");
			}

			// physics
			if (mLoadPhysicsSystem)
			{
				physics::PhysicsPlugin* pPP = loadPlugin<physics::PhysicsPlugin>( "yakePhysicsODE.dll" );
				YAKE_ASSERT( pPP ).debug("Cannot load physics plugin.");

				mPhysicsSystem = pPP->createSystem();
				YAKE_ASSERT( mPhysicsSystem ).error("Cannot create graphics system.");
			}

			// input
			input::InputPlugin* pIP = loadPlugin<input::InputPlugin>( "yakeInputOGRE.dll" );
			YAKE_ASSERT( pIP ).debug("Cannot load input plugin.");

			mInputSystem = pIP->createSystem();
			YAKE_ASSERT( mInputSystem ).error("Cannot create input system.");

			setupInput();
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
		}

		void destroyAllSystems()
		{
			YAKE_SAFE_DELETE( mInputSystem );
			//YAKE_SAFE_DELETE( mScriptingBindings );
			YAKE_SAFE_DELETE( mGraphicsSystem );
			YAKE_SAFE_DELETE( mPhysicsSystem );
			YAKE_SAFE_DELETE( mScriptingSystem );
		}
	};

}
}