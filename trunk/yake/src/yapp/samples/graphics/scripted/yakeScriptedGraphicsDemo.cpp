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

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yapp/samples/graphics/scripted/yakePCH.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <luabind/luabind.hpp>

#include <yake/graphics/yakeGraphics.h>
#include <yake/graphics/yakeGeometryAccess.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::math;
using namespace yake::graphics;
using namespace yake::data;

// forward declaration
class TheApp;
TheApp* getApplication();

class TheApp : public yake::exapp::ExampleApplication
{
	YAKE_DECLARE_CLASS( TheApp );
	private:
		Vector< std::pair<IViewport*,ICamera*> >	mVPs;
		SharedPtr< IWorld >							mGWorld;

		model::Graphical* 						mGraphical;
		
		SharedPtr< scripting::IBinder >			mMathLuaBinder;
		SharedPtr< scripting::IBinder >			mBaseLuaBinder;
		SharedPtr< scripting::IBinder >			mGraphicsLuaBinder;
		SharedPtr< scripting::IBinder >			mModelLuaBinder;
		
		String	mSceneFilename;
		
		base::Library*									mLib;
	public:
		TheApp( const String filename ) : ExampleApplication(  true /*graphics*/,
		false /*physics*/,
		true /*scripting*/,
		true /*input*/,
		true /*script bindings*/,
		false /*audio*/),
		mSceneFilename( filename )
		{
		}

		void onKey(const yake::input::KeyboardEvent& rEvent)
		{
			std::cout << "KEY: " << rEvent.keyCode << std::endl;
		
			if ( rEvent.keyCode == input::KC_ESCAPE )
				requestShutdown();
		}
	
		void onMB(uint8 btn)
		{
			std::cout << "MB: " << static_cast<int>( btn ) << std::endl;
		}

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


		void setupScene()
		{
			mGraphical = new model::Graphical();
			YAKE_ASSERT( mGraphical );
//			mGraphical->fromDotScene( mSceneFilename, mGWorld.get() );
		}
		
		model::Graphical* getGraphicalScene()
		{
			return mGraphical;
		}
		
		graphics::IWorld* getGraphicalWorld()
		{
			return mGWorld.get();
		}
		
		void doScripting()
		{
			YAKE_DECLARE_FUNCTION( doScripting );
			
			yake::scripting::IVM* pVM = getScriptingSystem().createVM();
			YAKE_ASSERT( pVM );

			YAKE_LOG( "Scripting VM created" );
			
			SharedPtr<base::Library> pLib = loadLib( "baseLuaBindings" );
			YAKE_ASSERT( pLib ).debug("Cannot load base Lua bindings plugin");

			mMathLuaBinder = create< scripting::IBinder >( "yake::math" );
			mMathLuaBinder->bind( pVM );
			
			YAKE_LOG( "yake::math bound" );
			
			mBaseLuaBinder = create< scripting::IBinder >( "yake::base" );
			mBaseLuaBinder->bind( pVM );
			
			YAKE_LOG( "yake::base bound" );
			
			SharedPtr<base::Library> pGLib = loadLib( "graphicsLuaBindings" );
			YAKE_ASSERT( pGLib ).debug("Cannot load graphics Lua bindings plugin");
			
			mGraphicsLuaBinder = create< scripting::IBinder >( "yake::graphics" );
			mGraphicsLuaBinder->bind( pVM );
			
			YAKE_LOG( "yake::graphics bound" );

			SharedPtr<base::Library> pModelLib = loadLib( "modelLuaBindings" );
			YAKE_ASSERT( pModelLib ).debug("Cannot load model Lua bindings plugin");
			
			mModelLuaBinder = create< scripting::IBinder >( "yapp::model" );
			mModelLuaBinder->bind( pVM );
			
			YAKE_LOG( "yapp::model bound" );
			
			YAKE_LOG( "Binding application services..." );

			scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
			YAKE_ASSERT( pL );
			if (!pL)
				return;

			using namespace luabind;
			
			
#define APP_MODULE pL->getLuaState(), "app"
			module( APP_MODULE )
			[
				def( "getApplication", &getApplication ),
				class_<TheApp>("TheApp")
				.def( "getGraphicalScene", &TheApp::getGraphicalScene )
						.def( "getGraphicalWorld", (yake::graphics::IWorld*(TheApp::*)())&TheApp::getGraphicalWorld )
			];
				
			YAKE_LOG( "Application services bound... Executing scripts..." );
			
			yake::scripting::IScript* pScript = 
					getScriptingSystem().createScriptFromFile( "../../media/samples/scriptedGraphics/math.lua" );
			
			YAKE_LOG( "Executing 'math.lua' " );
			pVM->execute( pScript );
			delete pScript;

			pScript = 
					getScriptingSystem().createScriptFromFile( "../../media/samples/scriptedGraphics/base.lua" );
			
			YAKE_LOG( "Executing 'base.lua' " );
			pVM->execute( pScript );
			delete pScript;
			
			pScript = 
					getScriptingSystem().createScriptFromFile( "../media/samples/scriptedGraphics/graphics.lua" );
			
			YAKE_LOG( "Executing 'graphics.lua' " );
			pVM->execute( pScript );
			delete pScript;
			
			// cleaning out
			delete pVM;
		}

		virtual void run()
		{
		// setup event input generators
			mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKey, this ) );
			mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMB, this ) );

		// graphics
			mGWorld = getGraphicsSystem().createWorld();
			YAKE_ASSERT( mGWorld );

			createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );

			if (mVPs[0].second)
			{
				mVPs[0].second->translate( Vector3( 0, 100, 700 ) );
				mVPs[0].second->pitch(-10);
			}

		// objects
			setupScene();
			
		// setup & execute scripts
			doScripting();
			
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

				//
				YAKE_ASSERT( getKeyboard() );
				if ( getKeyboard() )
				{
					static  unsigned iCam = 0;
					real distance = -200. * timeElapsed;
					if (getKeyboard()->isKeyDown(input::KC_LEFT))
						mVPs[iCam].second->translate( Vector3( distance, 0, 0 ) );
					if (getKeyboard()->isKeyDown(input::KC_RIGHT))
						mVPs[iCam].second->translate( Vector3( -distance, 0, 0 ) );
					if (getKeyboard()->isKeyDown(input::KC_UP))
						mVPs[iCam].second->translate( Vector3( 0, 0, distance ) );
					if (getKeyboard()->isKeyDown(input::KC_DOWN))
						mVPs[iCam].second->translate( Vector3( 0, 0, -distance ) );
					if (getKeyboard()->isKeyDown(input::KC_Z))
						mVPs[iCam].second->translate( Vector3( 0, distance, 0 ) );
					if (getKeyboard()->isKeyDown(input::KC_X))
						mVPs[iCam].second->translate( Vector3( 0, -distance, 0 ) );
			
				}

			// render the scene
				if (!shutdownRequested())
					mGWorld->render( timeElapsed );
			}

			YAKE_SAFE_DELETE( mGraphical );
	
			mGWorld.reset();
		}
};

TheApp* pApp = NULL;
TheApp* getApplication()
{
	YAKE_ASSERT( pApp != NULL ).error( "Application object was not created" );
	return pApp;
}

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main( int argc, char** argv )
{
	if ( argc <= 1 )
	{
		std::cout << "Usage: scriptedGraphics filename.scene " << std::endl;
		return 0; 
	}
	
	try
	{
		std::cout << "\n" << "A bit more advanced scripting ;) provided for YAKE by Nikita Buida" << "\n";
		std::cout << "\n" << "(Some code borrowed from GraphicsDemo)";
		std::cout << "\n";
		
		TheApp theApp( argv[1] );
		pApp = &theApp;
		theApp.initialise();
		theApp.run();
	}
	catch (const yake::Exception& rException)
	{
		std::cout << std::endl << rException.what() << std::endl;
	}
	
	return 0;
}
