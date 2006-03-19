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
#ifndef YAKE_RAF_APPLICATION_H
#define YAKE_RAF_APPLICATION_H

namespace yake {
	namespace audio {
		class IAudioSystem;
	}
	namespace input {
		class IInputSystem;
		class KeyboardDevice;
		class KeyboardEventGenerator;
		class MouseDevice;
		class MouseEventGenerator;
	}
#if YAKE_RAF_USES_CEGUI == 1
	namespace ceguiadapter {
		class RendererAdapterPlugin;
	}
#endif
namespace raf {

	class Application;

	/** This is a default executor for an application.
		It initialises, runs and shuts the application down and
		handles any unhandled exceptions in the process.
		If one of the three major stages fails, then false is returned.
		On success, true is returned.
		@see Application
	*/
	YAKE_RAF_API bool runApplication(Application& theApp);

	struct ApplicationConfiguration
	{
		virtual ~ApplicationConfiguration() {}

		/** List the libraries to load.
			For example, "graphicsOgre" for OGRE3D graphics or
				"physicsNx" for the Novodex based physics plugin.
		*/
		virtual StringVector getLibraries() = 0;

		/** An empty list will not create any systems.
			If the list contains an empty string, then the default system for the specific type will
			be created! (using create_default<>())
		*/
		virtual StringVector getGraphicsSystems()
			{ return MakeStringVector(); }
		virtual StringVector getScriptingSystems()
			{ return MakeStringVector(); }
		virtual StringVector getAudioSystems()
			{ return MakeStringVector(); }
		virtual StringVector getPhysicsSystems()
			{ return MakeStringVector(); }
		virtual StringVector getInputSystems()
			{ return MakeStringVector(); }

		virtual SharedPtr<graphics::IGraphicsSystem> useExternalGraphicsSystem() { return SharedPtr<graphics::IGraphicsSystem>(); }
#if YAKE_RAF_USES_CEGUI == 1
		/** I could need some Dihydrogendinatriumethylendiamintetraacetat. */
		virtual bool loadCEGUI() { return false; }
#endif
	};

	class YAKE_RAF_API Application
	{
		// construction/destruction
	protected:
		Application();
		Application(const Application&);
	public:
		virtual ~Application();

		/** Call to initialise the application.
			Derived applications can react by overriding onInitialise().
			Returns true on success, otherwise false.
			@see onInitialise()
		*/
		bool initialise();
		/** Call to run the application after successful initialisation.
			Derived applications have to implement the onRun() method which will be invoked by run().
			Returns true on success, otherwise false.
			@see onRun()
		*/
		bool run();
		/** Call to shutdown the application. Either initialise() and/or run() have to have succeeded previously
			to calling this method!
			Derived applications can react by overriding onShutdown() which will be invoked by shutdown().
			Returns true on success, otherwise false.
			@see onShutdown()
		*/
		bool shutdown();
	protected:
		AppMachine* getMachine();

		// util

		base::Library* loadLibrary( const String& file );
		ApplicationState* getMainState();

		// events

		virtual ApplicationState* createMainState()
		{ return new MainState(*this); }

		/** Can be overridden by user defined application code. */
		virtual bool onInitialise() { return true; }

		/** Can be overridden by user defined application code. */
		virtual bool onRun();

		/** Can be overridden by user defined application code. */
		virtual bool onShutdown() { return true; }

		// ...

		virtual ApplicationConfiguration& getConfiguration() = 0;

	public:
		scripting::IScriptingSystem* getScriptingSystem(const String& name = "");
		input::IInputSystem* getInputSystem(const String& name = "");
		graphics::IGraphicsSystem* getGraphicsSystem(const String& name = "");
		physics::IPhysicsSystem* getPhysicsSystem(const String& name = "");
		audio::IAudioSystem* getAudioSystem(const String& name = "");
		input::KeyboardDevice* getKeyboard();
		input::KeyboardEventGenerator* getKeyboardEventGenerator();
		input::MouseDevice* getMouse();
		input::MouseEventGenerator* getMouseEventGenerator();

#if YAKE_RAF_USES_CEGUI == 1
		void enableKeyboardInputForCEGUI(const bool yes);
		void enableMouseInputForCEGUI(const bool yes);
		void enableInputForCEGUI(const bool keyb = true, const bool mouse = true);
#endif
	private:
		void initInput();
		void initPhysics();
#if YAKE_RAF_USES_CEGUI == 1
		void initCEGUI();

		void onKeyDown( const yake::input::KeyboardEvent& rEvent );
		void onKeyUp( const yake::input::KeyboardEvent& rEvent );
		void onMBDown( uint8 btn );
		void onMBUp( uint8 btn );
		void onMouseMoved( const math::Vector3& rDelta );
#endif
	private:
		typedef Deque<SharedPtr<base::Library> > LibList;
		typedef AssocVector<String,SharedPtr<scripting::IScriptingSystem> > ScriptingSystemList;
		typedef AssocVector<String,SharedPtr<audio::IAudioSystem> > AudioSystemList;
		typedef AssocVector<String,SharedPtr<graphics::IGraphicsSystem> > GraphicsSystemList;
		typedef AssocVector<String,SharedPtr<physics::IPhysicsSystem> > PhysicsSystemList;
		typedef AssocVector<String,SharedPtr<input::IInputSystem> > InputSystemList;
		AppMachine*						mMachine;
		ApplicationState*				mMainState;
		LibList							mLibs;
		ScriptingSystemList				mScriptingSystems;
		AudioSystemList					mAudioSystems;
		GraphicsSystemList				mGraphicsSystems;
		PhysicsSystemList				mPhysicsSystems;
		InputSystemList					mInputSystems;
		input::KeyboardDevice*			mKeyboard;
		input::KeyboardEventGenerator*	mKeyboardEventGenerator;
		input::MouseDevice*				mMouse;
		input::MouseEventGenerator*		mMouseEventGenerator;
		bool							mUseExtGraphicsSystem;
#if YAKE_RAF_USES_CEGUI == 1
		::yake::ceguiadapter::RendererAdapterPlugin*	mCeguiRendererAdapter;
		typedef std::deque<yake::SignalConnection> SigConnList;
		SigConnList						mCeguiMouseSigConn;
		SigConnList						mCeguiKeyboardSigConn;
		bool							mCeguiMouseInputEnabled;
		bool							mCeguiKeyboardInputEnabled;
#endif
	};

	/** An example base application class provided for convenience.
		It wraps and manages a configuration object.
		One thing less to worry about.
		Usage: class MyApp : public ExampleApplication<MyConfig> {}
	*/
	template<class ConfigType>
	class ExampleApplication : public raf::Application
	{
	public:
		ExampleApplication() : raf::Application()
		{
		}
	protected:
		virtual raf::ApplicationConfiguration& getConfiguration()
		{
			return mCfg;
		}
	private:
		ConfigType	mCfg;
	};

} // namespace raf
} // namespace yake

#endif

