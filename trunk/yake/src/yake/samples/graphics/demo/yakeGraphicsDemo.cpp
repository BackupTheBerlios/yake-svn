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
using namespace yake::math;
using namespace yake::graphics;
using namespace yake::data;

// todo template approach for subsystems
class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector<std::pair<IViewport*,ICamera*> >	mVPs;
	SharedPtr< IWorld >					mGWorld;

	graphics::ISceneNode*				mLightOneNode;
	graphics::ILight*					mLightOne;

	struct SimpleOne {
		graphics::ISceneNode*			pSN;
	};
	SimpleOne							mNinja;
	SimpleOne							mGround;
	graphics::ILight*					mSunLight;
	SharedPtr<graphics::ISceneNode>		mSunLightNode;

	SimpleSpline						mCamPath;

	graphics::StringVector				mTechniques;
	size_t								mCurrentTechnique;
public:
	TheApp() : ExampleApplication( true /*graphics*/,
					false /*physics*/,
					false /*scripting*/,
					true /*input*/,
					false /*script bindings*/,
					false /*audio*/),
				mLightOneNode(0),
				mCurrentTechnique(0)
	{
		mNinja.pSN = 0;
		mGround.pSN = 0;
	}

	void changeShadowTechnique()
	{
		if (mTechniques.empty())
			return;

		mCurrentTechnique = ++mCurrentTechnique % mTechniques.size();
		const String& name = mTechniques[mCurrentTechnique];
		graphics::StringMap params;
		//if (name == "texture_modulative")
		{
			params["tex_size"] = "1024";
			params["tex_count"] = "3";
			params["far_distance"] = "1000";
			params["directional_light_extrusion_distance"] = "1000";
		}
		mGWorld->selectShadowTechnique( mTechniques[mCurrentTechnique], params );
		if (name == "stencil_additive")
		{
			mSunLight->setCastsShadows( true );

            mLightOne->setType(ILight::LT_POINT);
            mLightOne->setCastsShadows(true);
			mLightOne->setDiffuseColour( Color(0.9,0.7,0.7) );
			mLightOne->setSpecularColour( Color(1,1,1) );
            mLightOne->setAttenuation(8000,1,0.0005,0);
		}
		else if (name == "stencil_modulative")
		{
			// use only one light.
			mSunLight->setCastsShadows( false );

			mLightOne->setType(ILight::LT_POINT);
            mLightOne->setCastsShadows( true );
			mLightOne->setDiffuseColour( Color(0.9,0.7,0.7) );
			mLightOne->setSpecularColour( Color(1,1,1) );
            mLightOne->setAttenuation(8000,1,0.0005,0);
		}
		else if (name == "texture_modulative")
		{
			mSunLight->setCastsShadows( true );

			// Change fixed point light to spotlight
            mLightOne->setType(ILight::LT_SPOT);
            mLightOne->setDirection(-Vector3::kUnitZ);
            mLightOne->setCastsShadows(true);
			mLightOne->setDiffuseColour( Color(0.9,0.7,0.7) );
			mLightOne->setSpecularColour( Color(1,1,1) );
            mLightOne->setAttenuation(8000,1,0.0005,0);
            mLightOne->setSpotlightRange(80,90,1);
		}
		std::cout << "SHADOW TECHNIQUE: " << name.c_str() << "\n";
	}

	void onKey(const yake::input::KeyboardEvent & e)
	{
		std::cout << "Key pressed: " << e.keyCode << "\n";
		
		if (e.keyCode == input::KC_ESCAPE)
			requestShutdown();
		else if (e.keyCode == input::KC_T)
		{
			changeShadowTechnique();
		}
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
        // incase infinite far distance is not supported
        pC->setFarClipDistance(100000);

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
		mNinja.pSN = mGWorld->createSceneNode("sn_ninja");
		YAKE_ASSERT( mNinja.pSN );

		graphics::IEntity* pE = mGWorld->createEntity("ninja.mesh");
		//pE = mGWorld->createEntity("box_1x1x1.mesh");
		YAKE_ASSERT( pE );
		pE->setCastsShadow( true );

		mNinja.pSN->attachEntity( pE );
		mNinja.pSN->setPosition( Vector3(0,0,-200) );
	}

	void setupGround()
	{
		// create entity
		graphics::IEntity* pE = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( pE );
		pE->setCastsShadow( false  );

		// create scene node and attach entity to node
		mGround.pSN = mGWorld->createSceneNode("sn_ground");
		YAKE_ASSERT( mGround.pSN );
		mGround.pSN->attachEntity( pE );
		mGround.pSN->setScale( Vector3(100,1,100) );
	}

	void setupLights()
	{
		// fixed light (sun)
		mSunLight = mGWorld->createLight();
		mSunLightNode.reset( mGWorld->createSceneNode("sn_sunlight") );
		mSunLightNode->attachLight( mSunLight );

		mSunLight->setType(ILight::LT_SPOT);
		//mSunLight->setType(ILight::LT_DIRECTIONAL);
        mSunLightNode->setPosition(Vector3(1000,1250,500));
		mSunLight->setSpotlightRange(30,50,1);
        Vector3 dir = -mSunLightNode->getPosition();
        mSunLight->setDirection(dir.normalisedCopy());
        mSunLight->setDiffuseColour(Color(0.35, 0.35, 0.38));
        mSunLight->setSpecularColour(Color(0.9, 0.9, 1));

		// movable light 1
		YAKE_ASSERT(0==mLightOneNode);
		mLightOneNode = mGWorld->createSceneNode("sn_lightOne");
		YAKE_ASSERT( mLightOneNode );
		mLightOne = mGWorld->createLight();
		YAKE_ASSERT( mLightOne );
		mLightOneNode->attachLight( mLightOne );
		mLightOne->setType( graphics::ILight::LT_POINT );
		//mLightOne->setType( graphics::ILight::LT_SPOT );
		mLightOne->setDiffuseColour( Color(0.6,0.7,0.8) );
		mLightOne->setSpecularColour( Color(1,1,1) );
		mLightOne->setAttenuation( 8000, 1, 0.0005, 0 );

		//mLightOne->setDirection( Vector3(0,0,1) );
		//mLightOne->setSpotlightRange(80,100,1);
		//mLightOne->setCastsShadows( true );

        mLightOneNode->setPosition(Vector3(300,250,-300));
	}

	void setupScene()
	{
		// cube
		graphics::ISceneNode* node = 0;
        node = mGWorld->createSceneNode("sn_cube");
		YAKE_ASSERT( node );
		graphics::IEntity* ent = 0;
        ent = mGWorld->createEntity( "cube.mesh" );
		YAKE_ASSERT( ent );
		ent->setCastsShadow(true);
		ent->setMaterial("Examples/Athene/Basic");
        node->attachEntity( ent );
        node->translate(Vector3(0,-20, 0));
        //node->yaw(90);

		// Floor plane
		ent = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( ent );
		node = mGWorld->createSceneNode("sn_plane");
		YAKE_ASSERT( node );
		node->attachEntity( ent );
		node->setScale( Vector3(1000,1,1000) );
		ent->setMaterial("Examples/BumpyMetal");
		ent->setCastsShadow(false);

		// column
		ent = mGWorld->createEntity("column.mesh");
		YAKE_ASSERT( ent );
		node = mGWorld->createSceneNode("sn_col1");
		YAKE_ASSERT( node );
		node->attachEntity( ent );
		ent->setMaterial("Examples/Athene/Basic");
		ent->setCastsShadow(true);
		node->translate(Vector3(200,100, -200));

		// column
		ent = mGWorld->createEntity("column.mesh");
		YAKE_ASSERT( ent );
		node = mGWorld->createSceneNode("sn_col2");
		YAKE_ASSERT( node );
		node->attachEntity( ent );
		ent->setMaterial("Examples/Rockwall");
		ent->setCastsShadow(true);
		node->translate(Vector3(-200,0, -200));

		// column
		ent = mGWorld->createEntity("column.mesh");
		YAKE_ASSERT( ent );
		node = mGWorld->createSceneNode("sn_col3");
		YAKE_ASSERT( node );
		node->attachEntity( ent );
		ent->setMaterial("Examples/Rockwall");
		ent->setCastsShadow(true);
		node->translate(Vector3(-200,0, 200));

		// column
		ent = mGWorld->createEntity("column.mesh");
		YAKE_ASSERT( ent );
		node = mGWorld->createSceneNode("sn_col4");
		YAKE_ASSERT( node );
		node->attachEntity( ent );
		ent->setMaterial("Examples/Rockwall");
		ent->setCastsShadow(true);
		node->translate(Vector3(200,0, 200));
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMB, this ) );

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

		//
		mCamPath.addPoint( Vector3(-400,150,-400) );
		mCamPath.addPoint( Vector3( 300,150,-400) );
		mCamPath.addPoint( Vector3( 300,150, 400) );
		mCamPath.addPoint( Vector3(-400,150, 400) );
		mCamPath.addPoint( Vector3(-400,150,-400) );
		mCamPath.recalcTangents();

		// objects
		setupLights();
		setupNinja();
		setupGround();
		setupScene();

		mTechniques = mGWorld->getShadowTechniques();
		changeShadowTechnique();
		mGWorld->setShadowsEnabled( true );

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

			//
			YAKE_ASSERT( getKeyboard() );
			if (getKeyboard())
			{
				static  unsigned iCam = 0;
				real distance = -200. * timeElapsed;
				if (getKeyboard()->isKeyDown(input::KC_LEFT))
					mVPs[iCam].second->translate( Vector3(distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_RIGHT))
					mVPs[iCam].second->translate( Vector3(-distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_UP))
					mVPs[iCam].second->translate( Vector3(0, 0, distance) );
				if (getKeyboard()->isKeyDown(input::KC_DOWN))
					mVPs[iCam].second->translate( Vector3(0, 0, -distance) );
				if (getKeyboard()->isKeyDown(input::KC_A))
					mNinja.pSN->translate( Vector3(distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_D))
					mNinja.pSN->translate( Vector3(-distance, 0, 0) );
			}

			static real p1 = 0;
			p1 += timeElapsed * 0.05;
			const real max = 1. - (1. / mCamPath.getNumPoints());
			while (p1 > max)
				p1 -= max;
			mLightOneNode->setPosition( mCamPath.interpolate(p1) );
// 			mLightOneNode->setFixedYawEnabled( true );
// 			mLightOneNode->setFixedYawAxis( Vector3(0,1,0) );
// 			mLightOneNode->lookAt( Vector3(0,0,0) );

			static real sunv = 100;
			Vector3 v(0, 0, sunv * timeElapsed);
			mSunLightNode->translate( v );
			if (fabs(mSunLightNode->getPosition().z) > 800)
			{
				sunv *= -1;
				mSunLightNode->translate( -v );
			}

			// render the scene
			if (!shutdownRequested())
				mGWorld->render( timeElapsed );
		}

		mSunLightNode.reset();
		mSunLight = 0; // destroyed automatically by destroying the node it's attached to.

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
