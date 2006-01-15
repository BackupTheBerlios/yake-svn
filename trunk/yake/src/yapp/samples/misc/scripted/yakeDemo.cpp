#include <yapp/samples/misc/scripted/yakePCH.h>
#include <yapp/model/yakeGraphical.h>
#include <yapp/plugins/entLua/yakeEntLua.h>

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

	model::Graphical* 							mScene;
	String										mSceneFilename;

public:
	TheApp( const String filename ) : ExampleApplication(  
								true /*graphics*/,
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
		if ( rEvent.keyCode == input::KC_ESCAPE )
			requestShutdown();
	}
	
	void onMB(uint8 btn)
	{
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
		mScene = new model::Graphical();
		YAKE_ASSERT( mScene );
		mScene->fromDotScene( mSceneFilename, mGWorld.get() );
	}

	void initSim(ent::Simulation& theSim)
	{
		//@fixme: static initialization !?
		ent::Entity::init();
		ent::Pawn::init();
		ent::Light::init();

		// add Lua binder
		ent::lua::bindSim( theSim );

		// register entities with a specific sim
		theSim.regClass<ent::Pawn>("Pawn");
		theSim.regClass<ent::Light>("Light");

		// init done!

		// create entities
		ent::Entity* e1 = ent::Entity::cast( 
											theSim.createObject("Pawn", 
												MakeStringPairVector() << StringPair("visual","pawn.visual"),
												MakeStringVector() << "../../media/samples/scripted/ent_pawn_basic.lua") );
		ent::Entity* eLight1 = ent::Entity::cast( 
											theSim.createObject("Light", 
												MakeStringPairVector() << StringPair("visual","light.visual"), 
												MakeStringVector() << "../../media/samples/scripted/ent_light.lua") );
		eLight1->setProperty( "position", Vector3(50,100,0) );
		eLight1->setProperty( "diffuseColour", Color(1,1,1,1) );
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

		// create sim
		ent::Simulation theSim;
		theSim.start( &getScriptingSystem(), mGWorld.get() );
		initSim(theSim);

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
				real distance = -200.f * timeElapsed;
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
			{
				theSim.tick(timeElapsed);
				mGWorld->render( timeElapsed );
			}
		}

		theSim.stop();

		//YAKE_SAFE_DELETE( mScene );	

		mGWorld.reset();
	}
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main( int argc, char** argv )
{
	try
	{
		std::cout << std::endl;
		
		TheApp theApp( "../../media/samples/dotScene/DotScene1.scene" );
		theApp.initialise();
		theApp.run();
	}
	catch (const yake::Exception& rException)
	{
		std::cout << std::endl << rException.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
