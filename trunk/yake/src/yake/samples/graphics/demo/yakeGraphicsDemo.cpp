//****************************************************************************
//**
//**    Sample
//**
//**	Simpler than simple sample! Beware!
//**	It's WIP towards a real sample :)
//**
//**	When running, press ESC to exit.
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <yake/base/yake.h>
#include <yake/samples/common/yakeExampleApplication.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::templates;
using namespace yake::graphics;
using namespace yake;
using namespace yake::base::math;

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector<std::pair<IViewport*,ICamera*> >	mVPs;
	Pointer< IGraphicalWorld >				mGWorld;
	physics::IWorld*						mPWorld;

	struct SimpleOne {
		graphics::ISceneNode*		pSN;
		graphics::IEntity*			pE;
		physics::IComplexObject*	pCO;
	};
	SimpleOne								mNinja;
	SimpleOne								mBox;
	SimpleOne								mGround;
public:
	TheApp() : ExampleApplication( true /*graphics*/,
									true /*physics*/,
									false /*scripting*/,
									true /*input*/,
									false /*script bindings*/)
	{
	}

	void onKey(const yake::input::KeyboardEvent & e)
	{
		std::cout << "KEY: " << e.keyCode << std::endl;
		if (e.keyCode == input::KC_ESCAPE)
			requestShutdown();
	}
	void onMB(uint8 btn)
	{
		std::cout << "MB: " << static_cast<int>(btn) << std::endl;
	}

	int createCameraViewportPair( real sx, real sy, real w, real h, int z )
	{
		ICamera* pC = mGWorld->createCamera();
		YAKE_ASSERT( pC );
		pC->setNearClipDistance( 1. );
		pC->setFarClipDistance( 1000. );
		mVPs.push_back( std::pair<IViewport*,ICamera*>(mGWorld->createViewport( pC ), pC) );
		int idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		return idx;
	}

	void setupNinja()
	{
		// setup NINJA
		mNinja.pCO = 0; // no physical representation

		mNinja.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mNinja.pSN );

		mNinja.pE = mGWorld->createEntity("ninja.mesh");
		YAKE_ASSERT( mNinja.pE );

		mNinja.pSN->attachEntity( mNinja.pE );
		mNinja.pSN->setPosition( Vector3(0,0,-500) );
	}

	void setupGround()
	{
		// setup GROUND

		// setup physical representation
		mGround.pCO = mPWorld->createPlane( 0, 1, 0, 0 );
		YAKE_ASSERT( mGround.pCO );

		// create entity
		mGround.pE = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( mGround.pE );

		// create scene node and attach entity to node
		mGround.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mGround.pSN );
		mGround.pSN->attachEntity( mGround.pE );
	}

	void setupBox()
	{
		// setup BOX

		// setup physical representation
		mBox.pCO = mPWorld->createBox( 1, 1, 1 );
		YAKE_ASSERT( mBox.pCO );
		mBox.pCO->setPosition( Vector3(0,10,-100) );

		// create entity
		mBox.pE = mGWorld->createEntity("box_1x1x1.mesh");
		YAKE_ASSERT( mBox.pE );

		// create scene node and attach entity to node
		mBox.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mBox.pSN );
		mBox.pSN->attachEntity( mBox.pE );

		mBox.pSN->setPosition( mBox.pCO->getPosition() );
		mBox.pSN->setOrientation( mBox.pCO->getOrientation() );
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( TheApp::onMB, this ) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );

		createCameraViewportPair( 0.0, 0.0, 0.5, 0.5, 10 );
		createCameraViewportPair( 0.5, 0.0, 0.5, 0.5, 11 );
		createCameraViewportPair( 0.0, 0.5, 0.5, 0.5, 12 );
		createCameraViewportPair( 0.5, 0.5, 0.5, 0.5, 13 );

		mVPs[1].second->setPosition( Vector3(0,2,-80) );
		mVPs[3].second->setPosition( Vector3(0,50,-250) );

		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		mPWorld->setGlobalGravity( Vector3(0,-9.81,0) );

		// objects
		setupNinja();
		setupGround();
		setupBox();

		// main loop

		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = base::native::getTime();
			real timeElapsed = time-lastTime;//timer->getSeconds();
			lastTime = time;

			// handle input
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();

			// process physics
			mPWorld->update( timeElapsed );

			// update visual box representation from the physical one
			mBox.pSN->setPosition( mBox.pCO->getPosition() );
			mBox.pSN->setOrientation( mBox.pCO->getOrientation() );

			// render the scene
			mGWorld->render( timeElapsed );
		}

		// clean up

		YAKE_SAFE_DELETE( mBox.pCO );
		YAKE_SAFE_DELETE( mBox.pSN );
		YAKE_SAFE_DELETE( mNinja.pSN );

		YAKE_SAFE_DELETE( mPWorld );
		mGWorld.reset();
	}
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main()
{
	try
	{
		std::cout << std::endl << "A simple demo :) provided for YAKE by Stephan Kaiser" << std::endl;
		TheApp theApp;
		theApp.initialise();
		theApp.run();
	}
	catch (const yake::base::Exception & e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}