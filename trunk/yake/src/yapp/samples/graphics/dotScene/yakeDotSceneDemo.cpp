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

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yapp/samples/graphics/dotScene/yakePCH.h>
#include <yapp/model/yakeGraphical.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::math;
using namespace yake::graphics;
using namespace yake::data;

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector< std::pair<IViewport*,ICamera*> >	mVPs;
	SharedPtr< IWorld >							mGWorld;

	app::model::Graphical* 						mGraphical;
	
	String	mSceneFilename;
public:
	TheApp( const String filename ) : ExampleApplication(  true /*graphics*/,
								false /*physics*/,
								false /*scripting*/,
								true /*input*/,
								false /*script bindings*/,
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
		mGraphical = new app::model::Graphical();
		YAKE_ASSERT( mGraphical );
		mGraphical->fromDotScene( mSceneFilename, mGWorld.get() );
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
			mVPs[0].second->translate( Vector3(0,100,700) );
			mVPs[0].second->pitch(-10);
		}
		if (mVPs.size() > 1 && mVPs[1].second)
			mVPs[1].second->setPosition( Vector3(0,2,-80) );
		if (mVPs.size() > 2 && mVPs[2].second)
		{
			mVPs[2].second->translate( Vector3(0,15,30) );
			mVPs[2].second->pitch(-30);
		}
		if (mVPs.size() > 3 && mVPs[3].second)
		{
			mVPs[3].second->setPosition( Vector3(0,50,0) );
			mVPs[3].second->pitch(-90);
		}

		// objects
		setupScene();

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = base::native::getTime();
			real timeElapsed = time - lastTime;//timer->getSeconds();
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

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main( int argc, char** argv )
{
	if ( argc <= 1 )
	{
		std::cout << "Usage: DotSceneGraphicsDemo filename.scene " << std::endl;
		return 0; 
	}
	
	try
	{
		std::cout << std::endl << "A simple demo (viewing tool) ;) provided for YAKE by Nikita Buida" << std::endl;
		std::cout << std::endl << "(Some code borrowed from GraphicsDemo)";
		std::cout << std::endl << "Intended to be used as a debug tool by Simba";
		std::cout << std::endl << "Use arrow keys and Z, X to move camera around" << std::endl;
		std::cout << std::endl;
		
		TheApp theApp( argv[1] );
		theApp.initialise();
		theApp.run();
	}
	catch (const yake::base::Exception& rException)
	{
		std::cout << std::endl << rException.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
