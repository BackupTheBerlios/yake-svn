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
#include <yapp/samples/model/yakePCH.h>
#include <yake/samples/graphics/pch.h>
#include <yapp/model/yakeGraphical.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakePhysical.h>
#include <yapp/loader/yakeXODEParser.h>
#include <yapp/loader/yakeDotLinkLoader.h>
#include <yapp/model/yakeComplexModel.h>

using namespace yake;
using namespace yake::graphics;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::data;

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector< std::pair<IViewport*,ICamera*> >	mVPs;
	
	SharedPtr<physics::IWorld>					mPWorld;
	SharedPtr<graphics::IWorld>					mGWorld;
	SharedPtr<app::model::Physical>				mPhysical;
	SharedPtr<app::model::Graphical>			mGraphical;
	SharedPtr<app::model::complex::Model>		mModel;
		
	String	mfnXODE;
	String	mfnScene;
	String	mfnLink;
public:
	TheApp( const String fnLink, const String fnXODE, const String fnScene ) :
					 ExampleApplication( 	true /*graphics*/,
								true /*physics*/,
								false /*scripting*/,
								true /*input*/,
								false /*script bindings*/,
								false /*audio*/),
					mfnLink( fnLink ),
					mfnXODE( fnXODE ),
					mfnScene( fnScene )
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

	void setupWorld()
	{
		// Loading graphical part
		mGraphical.reset( new app::model::Graphical() );
		YAKE_ASSERT( mGraphical );
		
		mGraphical->fromDotScene( mfnScene, mGWorld.get() );

		// Loading physical part
		mPhysical.reset( new app::model::Physical() );
		YAKE_ASSERT( mPhysical );
		
		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( mfnXODE, false );
		YAKE_ASSERT( ser.getDocumentNode() );
		
		parser::xode::XODEParserV1 parser( *mPhysical );
		parser.load( ser.getDocumentNode() , mPWorld.get() );
		
		// Bringing it all together!
		mModel = SharedPtr<app::model::complex::Model>( new app::model::complex::Model() );
		
		mModel->addGraphical( mGraphical, "baseGraphical" );
		mModel->addPhysical( mPhysical, "basePhysical" );
		
		app::model::DotLinkLoader dotLinkLoader;
		dotLinkLoader.load( mfnLink, *mModel.get() );
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMB, this ) );
		
		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		
		//@todo mPWorld->setGlobalGravity( Vector3( 0, 0, -9.81 ) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );
		
		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );

		if (mVPs[0].second)
		{
			mVPs[0].second->translate( Vector3( 100,0,300 ) );
			//mVPs[0].second->rotate( Vector3( 0, 1, 0 ), 45 );
			//mVPs[0].second->pitch(-10);
		}

		// objects
		setupWorld();

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = base::native::getTime();
			real timeElapsed = time - lastTime;
			lastTime = time;
			
			// handle input
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();
			
			YAKE_ASSERT( getKeyboard() );
			if ( getKeyboard() )
			{
				static  unsigned iCam = 0;
				real distance = 200. * timeElapsed;
				real angle = 10 * timeElapsed;
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
				if (getKeyboard()->isKeyDown(input::KC_Q))
					mVPs[0].second->rotate( Vector3( 0, 1, 0 ), angle );
				if (getKeyboard()->isKeyDown(input::KC_W))
					mVPs[0].second->rotate( Vector3( 0, 1, 0 ), -angle );
			}

			// step simulation
			if ( !shutdownRequested() )
			{
				mPWorld->step( timeElapsed );
				
				//mModel->updatePhysics( timeElapsed );
				mModel->updateControllers( timeElapsed );
				
				mGWorld->render( timeElapsed );
			}
		}

		mModel.reset();
		mGraphical.reset();
		mGWorld.reset();
		mPhysical.reset();
		mPWorld.reset();
	}
};

//============================================================================
//    MAIN ENTRY
//============================================================================

int main( int argc, char** argv )
{
	if ( argc <= 3 )
	{
		std::cout << "Usage: dotLinkDemo fn0.link fn1.xode fn2.scene" << std::endl;
		return 0; 
	}
	
	try
	{
		std::cout << std::endl << "A simple demo ;) provided for YAKE by Nikita Buida" << std::endl;
		std::cout << std::endl;
		
		TheApp theApp( argv[1], argv[2], argv[3] );
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
