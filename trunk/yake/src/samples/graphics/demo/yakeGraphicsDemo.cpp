//****************************************************************************
//**
//**    Sample
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <inc/base/yake.h>
#include <inc/samples/common/yakeExampleApplication.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::templates;
using namespace yake::graphics;
using namespace yake;

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector<std::pair<IViewport*,ICamera*> >	mVPs;
	Pointer< IGraphicalWorld >				mWorld;
public:
	TheApp() : ExampleApplication( true /*graphics*/,
									true /*physics*/,
									false /*scripting*/,
									true /*input*/ )
	{
	}

	void onKey(const yake::input::KeyboardEvent & e)
	{
		std::cout << "KEY: " << e.keyCode << std::endl;
	}
	void onMB(uint8 btn)
	{
		std::cout << "MB: " << btn << std::endl;
	}

	int createCameraViewportPair( real sx, real sy, real w, real h, int z )
	{
		ICamera* pC = mWorld->createCamera();
		YAKE_ASSERT( pC );
		mVPs.push_back( std::pair<IViewport*,ICamera*>(mWorld->createViewport( pC ), pC) );
		int idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		return idx;
	}

	virtual void run()
	{
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( TheApp::onMB, this ) );

		mWorld = getGraphicsSystem().createWorld();

		createCameraViewportPair( 0.0, 0.0, 0.5, 0.5, 10 );
		createCameraViewportPair( 0.5, 0.0, 0.5, 0.5, 11 );
		createCameraViewportPair( 0.0, 0.5, 0.5, 0.5, 12 );
		createCameraViewportPair( 0.5, 0.5, 0.5, 0.5, 13 );

		ISceneNode* pSN = mWorld->createSceneNode();
		YAKE_ASSERT( pSN );

		IEntity* pE = mWorld->createEntity("ninja.mesh");
		YAKE_ASSERT( pE );

		pSN->attachEntity( pE );
		pSN->setPosition( yake::base::math::Vector3(0,0,-500) );

		int i = 0;
		while (i++ < 500)
		{
			mWorld->render( 0.01 );
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();
		}

		YAKE_SAFE_DELETE( pSN );
		mWorld.reset();
	}
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main()
{
	try
	{
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