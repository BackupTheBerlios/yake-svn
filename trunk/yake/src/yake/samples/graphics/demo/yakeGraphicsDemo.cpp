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
#include <yake/samples/graphics/pch.h>

#pragma warning( disable: 4305 ) // truncation from 'double' to 'float'

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::graphics;
using namespace yake::data;

void createDebugGeometryForComplexPhysical(
			const physics::IComplexObject* pComplex,
			graphics::IGraphicalWorld* pWorld,
			SharedSceneNodeList & nodes,
			SharedMUCList & ctrlrs)
{
	Vector<graphics::ISceneNode*> newNodes;
	createDebugGeometryForComplexPhysical( pComplex, pWorld, newNodes, ctrlrs );
	VectorIterator< Vector<graphics::ISceneNode*> > it( newNodes.begin(), newNodes.end() );
	while (it.hasMoreElements())
	{
		graphics::ISceneNode* pSN = it.getNext();
		nodes.push_back( SharedPtr<graphics::ISceneNode>( pSN ) );
	}
}

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector<std::pair<IViewport*,ICamera*> >	mVPs;
	SharedPtr< IGraphicalWorld >			mGWorld;

	graphics::ISceneNode*				mLightOneNode;

	struct SimpleOne {
		graphics::ISceneNode*			pSN;
		graphics::IEntity*				pE;
	};
	SimpleOne							mNinja;
	SimpleOne							mGround;
public:
	TheApp() : ExampleApplication( true /*graphics*/,
									false /*physics*/,
									false /*scripting*/,
									true /*input*/,
									false /*script bindings*/,
									false /*audio*/),
				mLightOneNode(0)
	{
		mNinja.pSN = 0;
		mGround.pSN = 0;
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

		// used for shadows
		// incase infinite far distance is not supported
		pC->setFarClipDistance( 100000 );

		mVPs.push_back( std::pair<IViewport*,ICamera*>(mGWorld->createViewport( pC ), pC) );
		size_t idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		return static_cast<int>(idx);
	}

	void setupNinja()
	{
		// setup NINJA
		mNinja.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mNinja.pSN );

		mNinja.pE = mGWorld->createEntity("ninja.mesh");
		YAKE_ASSERT( mNinja.pE );
		mNinja.pE->setCastsShadow( true );

		mNinja.pSN->attachEntity( mNinja.pE );
		mNinja.pSN->setPosition( Vector3(0,0,-500) );
	}

	void setupGround()
	{
		// create entity
		mGround.pE = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( mGround.pE );
		mGround.pE->setCastsShadow( false  );

		// create scene node and attach entity to node
		mGround.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mGround.pSN );
		mGround.pSN->attachEntity( mGround.pE );
		mGround.pSN->setScale( Vector3(100,100,100) );
	}

	void setupLights()
	{
		if (mLightOneNode)
			return;
		mLightOneNode = mGWorld->createSceneNode();
		YAKE_ASSERT( mLightOneNode );
		graphics::ILight* pL = mGWorld->createLight();
		YAKE_ASSERT( pL );
		mLightOneNode->attachLight( pL );
		pL->setType( graphics::ILight::LT_POINT );
		pL->setCastsShadow( false  );
		pL->setDiffuseColour( Color(1,1,1) );
		pL->setAttenuation( 8000, 1, 0.0005, 0 );
		pL->setSpecularColour( Color(1,1,1) );
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( TheApp::onMB, this ) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );

		mGWorld->setShadowsEnabled( true );

		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );
		//createCameraViewportPair( 0.0, 0.0, 0.5, 0.5, 10 );
		//createCameraViewportPair( 0.5, 0.0, 0.5, 0.5, 11 );
		//createCameraViewportPair( 0.0, 0.5, 0.5, 0.5, 12 );
		//createCameraViewportPair( 0.5, 0.5, 0.5, 0.5, 13 );

		if (mVPs[0].second)
		{
			mVPs[0].second->translate( Vector3(0,1.6,20) );
			mVPs[0].second->pitch(-30);
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
		setupLights();
		setupNinja();
		setupGround();

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = base::native::getTime();
			real timeElapsed = time-lastTime;//timer->getSeconds();
			lastTime = time;

			// handle input
			PROFILE_BEGIN( input );
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();

			//
			YAKE_ASSERT( getKeyboard() );
			if (getKeyboard())
			{
				static iCam = 0;
				real distance = -5. * timeElapsed;
				if (getKeyboard()->isKeyDown(input::KC_LEFT))
					mVPs[iCam].second->translate( Vector3(distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_RIGHT))
					mVPs[iCam].second->translate( Vector3(-distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_UP))
					mVPs[iCam].second->translate( Vector3(0, 0, distance) );
				if (getKeyboard()->isKeyDown(input::KC_DOWN))
					mVPs[iCam].second->translate( Vector3(0, 0, -distance) );
			}

			// update visual box representation from the physical one
			mLightOneNode->setPosition(
				mNinja->pSN->getPosition() + Vector3(0,10,0) );

			// render the scene
			mGWorld->render( timeElapsed );
		}

		YAKE_SAFE_DELETE( mLightOneNode );

		YAKE_SAFE_DELETE( mNinja.pSN );
		YAKE_SAFE_DELETE( mGround.pSN );

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