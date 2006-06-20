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

#include <yake/samples/model/dotScene/pch.h>
#include <yake/model/model.h>

#pragma warning( disable: 4305 ) // truncation from 'double' to 'float'

using namespace yake;

class TheApp : public yake::exapp::ExampleApplication, public yake::model::CentralControllerBase
{
private:
	typedef std::pair<graphics::IViewport*,graphics::ICamera*> ViewportCameraPair;
	Vector<ViewportCameraPair >			mVPs;
	SharedPtr< graphics::IWorld >		mGWorld;
	SharedPtr< physics::IWorld >		mPWorld;

public:
	TheApp() : ExampleApplication(
					true /*graphics*/,
					true /*physics*/,
					false /*scripting*/,
					true /*input*/,
					false /*script bindings*/,
					false /*audio*/)
	{
	}
	void onKey(const yake::input::KeyboardEvent & e)
	{
		std::cout << "Key pressed: " << e.keyCode << "\n";
		
		if (e.keyCode == input::KC_ESCAPE)
			requestShutdown();
	}
	void onMB(uint8 btn)
	{
		std::cout << "MB: " << static_cast<int>(btn) << std::endl;
	}
	void onModelComponent_preInit(const model::ComponentCreationContext& ctx, model::ModelComponent&)
	{
		YAKE_LOG("demo: onModelComponent_preInit()");
	}
	void onModelComponent_postInit(const model::ComponentCreationContext& ctx, model::ModelComponent&)
	{
		YAKE_LOG("demo: onModelComponent_postInit()");
	}
	int createCameraViewportPair( real sx, real sy, real w, real h, int z )
	{
		graphics::ICamera* pC = mGWorld->createCamera();
		YAKE_ASSERT( pC );
		pC->setNearClipDistance( 1. );
        // incase infinite far distance is not supported
        pC->setFarClipDistance(100000);

		mVPs.push_back( ViewportCameraPair(mGWorld->createViewport( pC ), pC) );
		size_t idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		return static_cast<int>(idx);
	}
	void setupScene()
	{
		// Floor plane
		graphics::IEntity* ent = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( ent );
		graphics::ISceneNode* node = mGWorld->createSceneNode("sn_plane");
		YAKE_ASSERT( node );
		node->attachEntity( ent );
		node->setScale( Vector3(1000,1,1000) );
		ent->setMaterial("Examples/BumpyMetal");
		ent->setCastsShadow(false);
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMB, this ) );

		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		mPWorld->setGlobalGravity( Vector3(0,-1,0) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );

		//NOTE: Make sure you create a camera before calling mGWorld->setShadowsEnabled(true)
		// when using "ogre3d" and the terrain scene manager with modulated texture shadows.
		// The first camera created will be used as the primary camera by the scene manager
		// and it crashes wildly if it's a texture shadow camera!

		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );
		//createCameraViewportPair( 0.0, 0.0, 0.5, 0.5, 10 );
		//createCameraViewportPair( 0.5, 0.0, 0.5, 0.5, 11 );
		//createCameraViewportPair( 0.0, 0.5, 0.5, 0.5, 12 );
		//createCameraViewportPair( 0.5, 0.5, 0.5, 0.5, 13 );

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

		// scene
		setupScene();

		//mGWorld->setShadowsEnabled( true );

		// create a model manager and set up the model creation context.
		model::ModelManager modelMgr;
		modelMgr.setCreationContext_GraphicalWorld( mGWorld.get() );
		modelMgr.setCreationContext_PhysicalWorld( mPWorld.get() );
		modelMgr.setCreationContext_CentralController( this );

		// just for demo purposes:
		modelMgr.subscribeToComponentPreInitializeSignal( Bind2(&TheApp::onModelComponent_preInit,this) );
		modelMgr.subscribeToComponentPostInitializeSignal( Bind2(&TheApp::onModelComponent_postInit,this) );

		// create models :)

		// method #1
		model::Model* m = modelMgr.createModel("m1","graphics/dotScene:name=gfx;file=../../media/samples/dotScene/DotScene1.scene");
		YAKE_ASSERT( m );
		((model::Graphical*)m->getComponentByTag("gfx"))->translate(Vector3(40,20,0));

		// method #2 - using dotModel templates
		bool success = modelMgr.loadTemplatesFromDotModel("E:/dev/yake.trunk/yake/common/media/samples/dotLink/DynTest/DotScene1.model");
		YAKE_ASSERT( success );
		m = modelMgr.createModelFromTemplate("m3", "dotscene1");
		YAKE_ASSERT( m );
		((model::Graphical*)m->getComponentByTag("gfx"))->translate(Vector3(-100,100,-200));

		// method #1 - but more complex
		m = modelMgr.createModel("m2",
			"graphics/dotScene:name=baseGraphical;file=../../media/samples/dotLink/DynTest/DotScene1.scene"
			"|physics/dotXODE:name=basePhysical;file=../../media/samples/dotLink/DynTest/DotScene1.xode"
			"|model/dotLink:file=../../media/samples/dotLink/DynTest/DotScene1.link"
			);
		YAKE_ASSERT( m );

		// main loop
		real lastTime = native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = native::getTime();
			real timeElapsed = time-lastTime;//timer->getSeconds();
			lastTime = time;

			// handle input
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();

			//
			YAKE_ASSERT( getKeyboard() );
			if (getKeyboard())
			{
				static unsigned iCam = 0;
				real distance = real(-200. * timeElapsed);
				if (getKeyboard()->isKeyDown(input::KC_LEFT))
					mVPs[iCam].second->translate( Vector3(distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_RIGHT))
					mVPs[iCam].second->translate( Vector3(-distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_UP))
					mVPs[iCam].second->translate( Vector3(0, 0, distance) );
				if (getKeyboard()->isKeyDown(input::KC_DOWN))
					mVPs[iCam].second->translate( Vector3(0, 0, -distance) );
			}

			// run sim & render the scene
			if (!shutdownRequested())
			{
				mPWorld->step( timeElapsed );
				triggerPhysicsUpdateSignal(0,timeElapsed);
				triggerGraphicsUpdateSignal(0,timeElapsed);

				mGWorld->render( timeElapsed );
			}
		}

		modelMgr.clear();

		mGWorld.reset();
		mPWorld.reset();
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
	catch ( const yake::Exception& e )
	{
		std::cout << std::endl << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << std::endl << "-----------------" << std::endl << "UNHANDLED EXCEPTION" << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
