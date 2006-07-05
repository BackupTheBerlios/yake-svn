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

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUICombobox.h>
#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include <CEGUI/elements/CEGUIStaticImage.h>

// Yake
#include <yake/base/yake.h>
#include <yake/base/yakeDebugOutputStream.h>
#include <yake/base/yakeLog.h>
#include <yake/base/yakeLibrary.h>
#include <yake/scripting/yakeScriptingSystem.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <yake/graphics/yakeGraphicsSystem.h>

#include <yake/samples/common/yakeExampleApplication.h>
#include <yake/data/yakeData.h>
#include <yapp/common/yakeCEGUIRendererAdapter.h>
#include <yapp/common/yakeCEGUIHelpers.h>
#include <yake/model/yakeGraphical.h>

// console 
#include <yapp/gui_addons/console/yakeGenericConsole.h>
#include <yapp/gui_addons/console/yakeDefaultHistory.h>
#include <yapp/gui_addons/console/yakeConsoleLuaProcessor.h>
#include <yapp/gui_addons/console/cegui/yakeCEGUIInputArea.h>
#include <yapp/gui_addons/console/cegui/yakeCEGUIOutputArea.h>

// boost
#include <boost/lexical_cast.hpp>

// luabind
#include <luabind/luabind.hpp>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::math;
using namespace yake::graphics;
using namespace yake::data;


// Let's try something out!
/** Message type
*/
struct Msg
{
	typedef char char_t;
	typedef yake::String string_t;
	typedef yake::Vector<Msg> ListType;

	string_t text_;	///<Message text
};

/** Application class.
 * Based on ExampleApplication which is often used in demos here.
 */
class TheApp : public yake::exapp::ExampleApplication
{
	YAKE_DECLARE_CLASS( TheApp );
private:

	/// Finally declare our console type
	typedef yake::gui::Console<	Msg,
											yake::gui::CEGUIInputArea,
											yake::gui::LuaInterpreter,
											yake::gui::History,
											yake::gui::CEGUIOutputArea> LuaConsole;

	/**	Class is a wrapper around LuaConsole for singleton to script binding.
	 */
	class ConsoleHolder
	{
	public:
		/**
		 *       Get contained console
		 * @return Reference to console object
		 */
		LuaConsole& getConsole() { return console_; }

	private:
		/// Contained console
		LuaConsole	console_;

		/// Declare that this class is a singleton
		YAKE_BUILD_PHOENIX_SINGLETON( ConsoleHolder );
	};

	/// Viewport/Camera pairs
	Vector< std::pair<IViewport*,ICamera*> >	mVPs;
	/// Graphical world
	SharedPtr< IWorld >								mGWorld;

	/// CEGUI renderer adapter handle
	ceguiadapter::RendererAdapterPlugin*		mGuiRendererAdapter;

	/// Editbox representing console input area
	CEGUI::Editbox*									mEditBox;
	/// Listbox representing console output area
	CEGUI::Listbox*									mListBox;

	/// Reference to Lua console is stored for convenience ( console is a singleton )
	LuaConsole&											mConsole;

public:
	TheApp( ) :
			ExampleApplication(  true /*graphics*/,
			                     false /*physics*/,
			                     true /*scripting*/,
			                     true /*input*/,
			                     false /*script bindings*/,
			                     false /*audio*/),
			mGuiRendererAdapter( NULL ),
			mConsole( ConsoleHolder::instance().getConsole() )
	{}

	/**
	 *   Load CEGUI renderer adapter plugin.
	 * @param file Plugin library file name
	 * @return Plugin handle
	 */
	ceguiadapter::RendererAdapterPlugin* loadPlugin( const String& file )
	{
		yake::base::Library* pDynLib = new yake::base::Library( file );
		YAKE_ASSERT( pDynLib ).debug( "Out of memory" );

		YAKE_LOG( "Loading plugin " + file );

		yake::base::YakeDynLibStartPluginFn pfnStartPlugin = (yake::base::YakeDynLibStartPluginFn)pDynLib->getSymbol( "dynlibStartPlugin" );
		YAKE_ASSERT( pfnStartPlugin ).debug( "Cannot find export in dynamic library" );

		ceguiadapter::RendererAdapterPlugin* pPlugin = static_cast<ceguiadapter::RendererAdapterPlugin*>(pfnStartPlugin());
		YAKE_ASSERT( pPlugin ).debug( "Plugin startup function failed" );

		//
		bool bRet = pPlugin->initialise();
		YAKE_ASSERT( bRet ).debug( "Plugin initialization failed" );

		if (!bRet)
		{
			delete pPlugin;
			return 0;
		}

		return pPlugin;
	}

	/**
	 *   Keyboard key pressed event handler.
	 * Implements console key combinations processing. Feeds events to CEGUI.
	 * @param rEvent 
	 */
	void onKeyDown( const yake::input::KeyboardEvent& rEvent )
	{
		YAKE_LOG( "KEY: " + boost::lexical_cast<String>( rEvent.keyCode ) );

		if ( rEvent.keyCode == input::KC_ESCAPE )
			requestShutdown();

		if ( rEvent.keyCode == input::KC_RETURN || rEvent.keyCode == input::KC_TAB )
		{
			mConsole.execute();
		}
		else if ( rEvent.keyCode == input::KC_UP )
		{
			mConsole.previousCommand();
		}
		else if ( rEvent.keyCode == input::KC_DOWN )
		{
			mConsole.nextCommand();
		}
		else if ( ( getKeyboard()->isKeyDown(input::KC_LSHIFT) || getKeyboard()->isKeyDown(input::KC_RSHIFT) ) && getKeyboard()->isKeyDown(input::KC_PGUP) )
		{
			mConsole.scrollOutputUp( 30 );
		}
		else if ( ( getKeyboard()->isKeyDown(input::KC_LSHIFT) || getKeyboard()->isKeyDown(input::KC_RSHIFT) ) && getKeyboard()->isKeyDown(input::KC_PGDOWN) )
		{
			mConsole.scrollOutputDown( 30 );
		}
		else
		{
			CEGUI::System::getSingleton().injectKeyDown( rEvent.keyCode );

			long modifiers = getKeyboard()->isKeyDown(input::KC_LSHIFT) || getKeyboard()->isKeyDown(input::KC_RSHIFT) ? 1 : 0;

			CEGUI::System::getSingleton().injectChar( getKeyChar( rEvent.keyCode, modifiers ) );
		}
	}

	/**
	 *   Keyboard key release event handler.
	 * Feeds event to CEGUI.
	 * @param rEvent 
	 */
	void onKeyUp( const yake::input::KeyboardEvent& rEvent )
	{
		YAKE_LOG( "KEY released: " + boost::lexical_cast<String>( rEvent.keyCode ) );

		CEGUI::System::getSingleton().injectKeyUp( rEvent.keyCode );
	}

	/**
	 *   Mouse button event handler
	 * @param btn Button index
	 */
	void onMBDown( uint8 btn )
	{
		YAKE_LOG( "MB: " + boost::lexical_cast<String>( static_cast<int>( btn ) ) );
		CEGUI::System::getSingleton().injectMouseButtonDown( convertYakeButtonToCegui( btn ) );
	}

	/**
	 *   Mouse button event handler
	 * @param btn Button index
	 */
	void onMBUp( uint8 btn )
	{
		YAKE_LOG( "MB: " + boost::lexical_cast<String>( static_cast<int>( btn ) ) );
		CEGUI::System::getSingleton().injectMouseButtonUp( convertYakeButtonToCegui( btn ) );
	}

	/**
	 *   Mouse movement handler
	 * @param rDelta Mouse movement since last such event
	 */
	void onMouseMoved( const Vector3& rDelta )
	{
		const real scale = 1;
		if (CEGUI::System::getSingletonPtr())
		{
			CEGUI::System::getSingleton().injectMouseMove( rDelta.x*scale, rDelta.y*scale );
		}
	}

	/**
	 *   Create camera and viewport.
	 * Camera is attached to viewport. Camera/Viewport pair is saved in mVPs map.
	 * Origin is in top-left corner.
	 * @param sx Starting x position of viewport in relative units [0..1] 
	 * @param sy Starting y position of viewport in relative units [0..1] 
	 * @param w Viewport width in relative units [0..1] 
	 * @param h Viewport height in relative units [0..1] 
	 * @param z Z-order of viewport
	 * @return Index of creaed camera/viewport pair in mVPs map
	 */
	int createCameraViewportPair( real sx, real sy, real w, real h, int z )
	{
		ICamera* pC = mGWorld->createCamera();
		YAKE_ASSERT( pC );
		pC->setNearClipDistance( 1. );

		pC->setFarClipDistance( 2000 );

		mVPs.push_back( std::pair<IViewport*,ICamera*>(mGWorld->createViewport( pC ), pC) );
		size_t idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		return static_cast<int>(idx);
	}

	/**
	 *   Helper method to add console message.
	 * Used from scripts.
	 * @param msg Message to add.
	 */
	static void addConsoleMessage( const Msg::string_t& msg )
	{
		Msg message;
		message.text_ = msg;

		ConsoleHolder::instance().getConsole().addMessage( message );
	}
	
	/**
	 *   Setup console.
	 * Here occurs fine-tuning of individual console components.
	 */
	void setupConsole()
	{
		mConsole.setInputWidget( mEditBox );
		mConsole.setOutputWidget( mListBox );

		yake::scripting::IVM* pVM = getScriptingSystem().createVM();
		YAKE_ASSERT( pVM );

		mConsole.setScriptingVirtualMachine( pVM );

		scripting::LuaVM* pL = static_cast<scripting::LuaVM*>( pVM );
		YAKE_ASSERT( pL );
		if (!pL)
			return;

		using namespace luabind;

		module( pL->getLuaState() )
				[
				class_<TheApp>( "app" )
				.scope
				[
				def( "addConsoleMessage", &TheApp::addConsoleMessage )
				]
				];

		YAKE_LOG( "Executing console init script... " );

		// executing console initialisation script
		pL->execute( "dofile( '../../media/gui.addons.scripts/console_redirect.lua' );" );

	}

	/**
	 *   Initialise GUI.
	 * Load CEGUI windows, setup console.
	 */
	void initGui()
	{
		//mGuiRendererAdapter = loadPlugin( "CEGUIOgreAdapter" );
		mGuiRendererAdapter = loadPlugin( "ceguiOgreRendererAdapter" );
		YAKE_ASSERT( mGuiRendererAdapter );

		YAKE_LOG( "Starting adapter version " + mGuiRendererAdapter->getName() );
		YAKE_LOG( "Initialising CEGUI..." );

		CEGUI::System* ceguiSys = new CEGUI::System( mGuiRendererAdapter->getRenderer() );
		ceguiSys = 0; // we don't use sys pointer but want to avoid unused variable warning

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

			Window* sheet = wmgr.loadWindowLayout( "console.layout" );
			System::getSingleton().setGUISheet(sheet);

			CEGUI::Window* pEditBoxWnd = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"/Console/Wnd/Edit");
			mEditBox = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().getWindow("/Console/Wnd/Edit"));
			mListBox = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().getWindow("/Console/Wnd/List"));
		}
		
		// catch to prevent exit (errors will be logged).
		catch( CEGUI::Exception& e)
		{
			YAKE_LOG( yake::String( "CEGUI Exception: " ) + e.getMessage().c_str() );
		}
	
		setupConsole();
	}

	/**
	 *   Cleanup GUI resources.
	 * Executed on app shutdown.
	 */
	void cleanupGui()
	{
		if ( CEGUI::System::getSingletonPtr() )
			delete CEGUI::System::getSingletonPtr();
		if (mGuiRendererAdapter)
			mGuiRendererAdapter->shutdown();
		//YAKE_SAFE_DELETE( mGuiRendererAdapter ); // do NOT delete, the plugin is in the list of plugins ExampleApplication holds and will be destroyed later.
	}

	/**
	 *   Setup input events.
	 * Subscribe to input subsystem signals.
	 */
	void setupInputEvents()
	{
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKeyDown, this ) );
		mKeyboardEventGenerator.subscribeToKeyUp( Bind1( &TheApp::onKeyUp, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMBDown, this ) );
		mMouseEventGenerator.subscribeToMouseButtonUp( Bind1( &TheApp::onMBUp, this ) );
		mMouseEventGenerator.subscribeToMouseMoved( Bind1( &TheApp::onMouseMoved, this ) );
	}
	
	/**
	 *   Setup graphics.
	 * Sets up graphical world and cameras/viewports.
	 */
	void setupGraphics()
	{
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );

		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );
	}

	/**
	 *   Run the application main cycle.
	 * Performs initialisation, then enters main program loop.
	 */
	virtual void run()
	{
		// setup event input generators
		setupInputEvents();
		
		// graphics
		setupGraphics();

		// initialise gui
		initGui();

		// main loop
		real lastTime = native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = native::getTime();
			real timeElapsed = time - lastTime;
			lastTime = time;

			// handle input
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();

			// render the scene
			if (!shutdownRequested())
				mGWorld->render( timeElapsed );
		}

		// Shutting down
		cleanupGui();
		mGWorld.reset();
	}
};

/**
 * Main program entry point.
 * @param argc Number of passed command line arguments
 * @param argv Arguments as strings
 * @return Exit code
 */
int main( int argc, char** argv )
{
	try
	{
		TheApp theApp;
		theApp.initialise();
		theApp.run();
	}
	catch ( const yake::Exception& rException )
	{
		YAKE_LOG( rException.what() );
	}

	return 0;
}
