//****************************************************************************
//**
//**    Sample
//**
//**	Simpler than simple sample! Beware!
//**	It's WIP towards a real sample :)
//**
//**	When running, press ESC to exit.
//**
//**	You may want to try to use A and D for steering,
//**	W for accelerating and RETURN and SHIFT to
//**	shift up / down.
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yapp/samples/base/vehicle/yakePCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeModel.h>
#include <yapp/model/yakeModelMovableLink.h>
#include <yapp/loader/yakeDotLinkLoader.h>
#include <yake/samples/data/demo/yakeDotScene.h> // temporarily
#include <yapp/samples/common/yakeHelper.h>
#include <yapp/common/yakePawn.h>
#include <yapp/model/yakeVehicleSystem.h>

#define PROFILER_BEGINFRAME
#define PROFILER_ENDFRAME
#define PROFILER_DESTROY
#define PROFILERMANAGER_DESTROY
/*
#include <yake/base/templates/yakeObjectLink.h>
#include <yake/samples/common/yakeModelLink.h> // temporarily
#include <yake/samples/common/yakeXODE.h> // temporarily
#include <yake/samples/graphics/Profiler.h>
*/
#pragma warning( disable: 4305 ) // truncation from 'double' to 'float'

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::graphics;
using namespace yake::data;

using namespace yapp;
using namespace yake::app;

namespace yake {
	struct LocalHumanInputData
	{
		input::InputSystem*				pInputSystem_;
		input::KeyboardEventGenerator*	pKeyboardEventGenerator_;
		input::MouseEventGenerator*		pMouseEventGenerator_;
		const input::KeyboardDevice*			pKeyboard_;
		LocalHumanInputData(
			input::InputSystem* pInputSystem,
			input::KeyboardEventGenerator* pKeyboardEventGenerator,
			input::MouseEventGenerator* pMouseEventGenerator,
			const input::KeyboardDevice* pKeyboard ) :
					pInputSystem_( pInputSystem ),
					pKeyboardEventGenerator_( pKeyboardEventGenerator ),
					pMouseEventGenerator_( pMouseEventGenerator ),
					pKeyboard_( pKeyboard )
		{
		}
		bool isValid() const
		{
			return (pInputSystem_ && pKeyboardEventGenerator_ && pMouseEventGenerator_ );
		}
	};
	class GameControl
	{
	public:
		GameControl( const LocalHumanInputData & gatherInputData ) :
		  mLastTime( native::getTime() ),
		  mLocalHumanInputData( gatherInputData )
		{}
		typedef Signal1<void(const real)> VoidRealSignal;
		typedef Signal0<void> VoidSignal;

		typedef VoidSignal SenseSignal;
		void subscribeToSense(const SenseSignal::slot_type & slot)
		{ mSense.connect( slot ); }

		typedef VoidSignal ThinkSignal;
		void subscribeToThink(const ThinkSignal::slot_type & slot)
		{ mThink.connect( slot ); }

		typedef VoidSignal ActStepSignal;
		void subscribeToActStep(const ActStepSignal::slot_type & slot)
		{ mActStep.connect( slot ); }

		typedef VoidRealSignal PhysicsStepSignal;
		void subscribeToPhysicsStep(const PhysicsStepSignal::slot_type & slot)
		{ mPhysicsStep.connect( slot ); }

		typedef VoidRealSignal PostPhysicsStepSignal;
		void subscribeToPostPhysicsStep(const PostPhysicsStepSignal::slot_type & slot)
		{ mPostPhysicsStep.connect( slot ); }

		typedef VoidRealSignal RenderStepSignal;
		void subscribeToRenderStep(const RenderStepSignal::slot_type & slot)
		{ mRenderStep.connect( slot ); }

		void update()
		{
			//sense
			mSense();
			//think about it, make decisions
			mThink();
			//act depending on decisions
			mActStep();

			//timing
			static bool firstFrame = true;
			real currentTime = native::getTime();
			real timeElapsed = currentTime - mLastTime;
			if (firstFrame)
			{
				firstFrame = false;
				timeElapsed = 0.0;
			}

			//update physics
			mPhysicsStep( timeElapsed );
			//update graphics/audio/... from physics
			mPostPhysicsStep( timeElapsed );
			//render
			mRenderStep( timeElapsed ),

			// prepare for next frame
			mLastTime = currentTime;
		}
		const LocalHumanInputData& getLocalHumanInputData() const
		{ return mLocalHumanInputData; }
	private:
		LocalHumanInputData		mLocalHumanInputData;
		real					mLastTime;
		SenseSignal				mSense;
		ThinkSignal				mThink;
		ActStepSignal			mActStep;
		PhysicsStepSignal		mPhysicsStep;
		PostPhysicsStepSignal	mPostPhysicsStep;
		RenderStepSignal		mRenderStep;
	private:
		task::TaskManager		mTaskMgr;
	};
	class PawnVisual : public model::VisualComponent
	{
	public:
		void attachModel( model::complex::Model* pComplex )
		{
			YAKE_ASSERT( pComplex );
			mComplex = pComplex;
		}
	private:
		model::complex::Model*	mComplex;
	};
	class PawnLocalHumanInput : public model::InputComponent
	{
	public:
		PawnLocalHumanInput( GameControl& gameCtrl ) : mpGameCtrl(&gameCtrl), mPull(false)
		{
		}
		void update()
		{
			YAKE_ASSERT( mpGameCtrl );
			YAKE_ASSERT( mpGameCtrl->getLocalHumanInputData().isValid() );
			if (mpGameCtrl->getLocalHumanInputData().pKeyboard_->isKeyDown( input::KC_UP ))
				mPull = true;
			else
				mPull = false;
		}
		bool isPulling() const
		{ return mPull; }
	private:
		GameControl*	mpGameCtrl;
		bool			mPull;
	};
	class PawnControl : public model::ActorControl
	{
	public:
		PawnControl( PawnLocalHumanInput* pLHI ) : mLHI(pLHI)
		{
		}
		void setControlledMovable( physics::IComplexObject* pMovable )
		{
			mMovable = pMovable;
		}
		void update()
		{
			YAKE_ASSERT( mMovable );
			if (!mLHI)
				return;
			if (mLHI->isPulling())
			{
				mMovable->getBody()->addForce( Vector3(0,20,0) );
			}
		}
	private:
		physics::IComplexObject*		mMovable;
		PawnLocalHumanInput*			mLHI;
	};
}

class TheApp : public yake::exapp::ExampleApplication
{
//private:
public:
	task::TaskManager					mTaskMgr;
	//@{ tasks
	// task: render
	class RenderTask : public task::DefTask
	{
	private:
		IWorld*	mWorld;
	public:
		RenderTask( IWorld* pWorld );
		virtual void execute(real timeElapsed);
	};
	RenderTask*							mRenderTask;

	// task: physics
	class PhysicsTask : public task::DefTask
	{
	private:
		physics::IWorld*	mWorld;
	public:
		PhysicsTask( physics::IWorld* pWorld );
		virtual void execute(real timeElapsed);
	};
	PhysicsTask*						mPhysicsTask;

	// task: input
	class InputTask : public task::DefTask
	{
	private:
		TheApp*	mApp;
		//::yake::app::model::complex::vehicle::LandVehicle*	mCar;
	public:
		InputTask( 	TheApp* theApp/*, model::complex::vehicle::LandVehicle* theCar*/ );
		virtual void execute(real timeElapsed);
	};
	InputTask*							mInputTask;
	//@}

	//@{ name worlds
	SharedPtr< IWorld >			mGWorld;
	physics::IWorld*						mPWorld;
	audio::IWorld*							mpAWorld;
	//@}

	Vector<std::pair<IViewport*,ICamera*> >	mVPs; /// Viewport / Camera pairs

	//@{ audio related
	audio::IListener*					mpAListener;
	audio::ISource*						mpASource;
	audio::ISoundData*					mpAData;
	//@}

	//@{ testing stuff
	graphics::ISceneNode*				mLightOneNode;
	graphics::IParticleSystem*			mSmoke;

	struct SimpleOne {
		graphics::ISceneNode*			pSN;
		graphics::IEntity*				pE;
		physics::IComplexObject*		pCO;
		model::MovableUpdateController	updCtrl;
	};
	SimpleOne								mGround;
	//@}
	//@{ name controllable entities
	model::Pawn*							mPawn1;
	model::vehicle::Vehicle*				mVehicle1;
	//@}
public:
	TheApp() : ExampleApplication( true /*graphics*/,
									true /*physics*/,
									false /*scripting*/,
									true /*input*/,
									false /*script bindings*/,
									false /*audio*/),
				mLightOneNode(0),
				mRenderTask(0),
				mPhysicsTask(0),
				mInputTask(0),
				mPawn1(0),
				mVehicle1(0)
	{
	}
	void onKey(const yake::input::KeyboardEvent & e)
	{
		if (e.keyCode == input::KC_ESCAPE)
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

		// used for shadows
		// incase infinite far distance is not supported
		//pC->setFarClipDistance( 2000. );

		mVPs.push_back( std::pair<IViewport*,ICamera*>(mGWorld->createViewport( pC ), pC) );
		size_t idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs.back().first );
		mVPs.back().first->setDimensions( sx, sy, w, h );
		mVPs.back().first->setZ( z );
		return static_cast<int>(idx);
	}

	void setupDotScene()
	{
		// dotScene test

		// 1. read dotscene file into DOM

		dom::xml::XmlSerializer ser;
		ser.parse("../../media/graphics.scenes/arena0/arena0_export.scene", false);

		// 2. parse DOM and create graphical scene

		yake::data::serializer::dotscene::DotSceneSerializerV1 dss;
		dss.load( ser.getDocumentNode(), mGWorld.get() );

		// create a root scene node for newly loaded scene
		graphics::ISceneNode* pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( pSN );

		// copy free root scene nodes into a parent scene node for newly loaded scene
		yake::data::serializer::dotscene::SceneNodeList nodes = dss.getRootLevelSceneNodes();
		for (yake::data::serializer::dotscene::SceneNodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			pSN->addChildNode( *it );
		}
	}
	void setupGround()
	{
		// setup GROUND

		// setup physical representation
		mGround.pCO = mPWorld->createMesh( "../../media/graphics.scenes/arena0/arena0ground.physics" );
		//mGround.pCO = mPWorld->createPlane( Vector3(0, 1, 0), 0 );
		YAKE_ASSERT( mGround.pCO );
		mGround.pCO->setFriction( 1.5 );
		mGround.pCO->setFriction2( 1.5 );

		// create entity
		mGround.pE = 0;
		mGround.pSN = 0;

		/*
		mGround.pE = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( mGround.pE );
		mGround.pE->setMaterial("Examples/BumpyMetal");
		mGround.pE->setCastsShadow( false  );

		// create scene node and attach entity to node
		mGround.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mGround.pSN );
		mGround.pSN->attachEntity( mGround.pE );
		mGround.pSN->setScale(Vector3(100,1,100));
		*/
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
		//pL->setType( graphics::ILight::LT_POINT );
		//pL->setType( graphics::ILight::LT_SPOT );
		//pL->setSpotlightRange( 80, 100, 1 );
		pL->setType( graphics::ILight::LT_DIRECTIONAL );
		Vector3 n(-1,-1,0);
		n.normalise();
		pL->setDirection( n );
		pL->setCastsShadow( true );
		pL->setDiffuseColour( Color(0.75, 0.75, 0.78) );
		pL->setAttenuation( 8000, 1, 0.0005, 0 );
		pL->setSpecularColour( Color(0.9, 0.9, 1) );
		mLightOneNode->setPosition(Vector3(0,50,0));
	}

	model::Pawn* setupPawn(GameControl & gameCtrl)
	{
		// - model
		model::complex::Model* pComplex = new model::complex::Model();

		// - add physical representation
		model::Physical* pPhysical = new model::Physical();
		pComplex->addPhysical( SharedPtr<model::Physical>( pPhysical ), "basePhysical" );

		physics::IComplexObject* pCO = mPWorld->createSphere( 2 );
		YAKE_ASSERT( pCO );
		pCO->setPosition( Vector3(0,15,-10) );
		pCO->getBody()->setMass( 1 );
		pPhysical->addComplex( SharedPtr<physics::IComplexObject>( pCO ), "base" );

		// - add graphical representation
		model::Graphical* pGraphical = new model::Graphical();
		pComplex->addGraphical( SharedPtr<model::Graphical>( pGraphical ), "baseGraphical" );

		graphics::ISceneNode* pSN = mGWorld->createSceneNode();
		graphics::IEntity* pE = mGWorld->createEntity("sphere_d1.mesh");
		pSN->attachEntity( pE );
		pGraphical->addSceneNode( pSN, "base", true );
		pSN->setPosition( Vector3(0,5,0) );

		// - model link
		/*
		//SharedPtr<app::model::ModelMovableLink> pMML = create<app::model::ModelMovableLink>("yake.movable");
		SharedPtr<app::model::IObjectController> pMML = ::yake::createModelMovableLink( pCO, pSN, true, true );
		pComplex->addController( pMML );
		*/
		app::model::DotLinkLoader dll;
		dll.load( "../../media/dotlink_test.link", *pComplex );

		// -- Pawn
		model::Pawn* pPawn = new model::Pawn();

		PawnVisual* pVis = new PawnVisual();
		pPawn->addComponent( pVis, "defaultVis" );
		pPawn->setModel( pComplex );

		PawnLocalHumanInput* pRawInput = new PawnLocalHumanInput(gameCtrl);
		pPawn->addComponent( pRawInput, "defaultInput" );

		PawnControl* pCtrl = new PawnControl(pRawInput);
		pPawn->addComponent( pCtrl, "defaultCtrl" );
		pPawn->setMovable( pCO );
		pCtrl->setControlledMovable( pCO );

		gameCtrl.subscribeToPostPhysicsStep( Bind1( &model::Pawn::onPostPhysics, pPawn ) );
		gameCtrl.subscribeToSense( boost::bind( &model::Pawn::onSense, pPawn ) );
		gameCtrl.subscribeToThink( boost::bind( &model::Pawn::onThink, pPawn ) );
		gameCtrl.subscribeToActStep( boost::bind( &model::Pawn::onAct, pPawn ) );

		return pPawn;
	}

	void setupVehicle(GameControl & gameCtrl)
	{
		if (mVehicle1)
			return;
		SharedPtr<model::vehicle::IVehicleSystem> pVS = create<model::vehicle::IVehicleSystem>("yake.native");
		mVehicle1 = pVS->createVehicle("../../media/vehicles/fordboss.vehicle", mPWorld, mGWorld.get() );
		gameCtrl.subscribeToActStep( boost::bind( &model::vehicle::Vehicle::act, mVehicle1 ) );
	}
	void cleanupVehicle()
	{
		YAKE_SAFE_DELETE( mVehicle1 );
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( TheApp::onMB, this ) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );

		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );
		//createCameraViewportPair( 0.0, 0.0, 0.5, 0.5, 10 );
		//createCameraViewportPair( 0.5, 0.0, 0.5, 0.5, 11 );
		//createCameraViewportPair( 0.0, 0.5, 0.5, 0.5, 12 );
		//createCameraViewportPair( 0.5, 0.5, 0.5, 0.5, 13 );

		if (mVPs[0].second)
		{
			mVPs[0].second->translate( Vector3(0,1.6,20) );
			//mVPs[0].second->pitch(-30);
			mVPs[0].second->lookAt( Vector3(0,0,0) );
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

		mGWorld->setShadowsEnabled( false );

		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		//mPWorld->setGlobalGravity( Vector3(0,-9.81,0) );
		mPWorld->setGlobalGravity( Vector3(0,-9.81,0) );

		// audio
		//setupAudio();

		LocalHumanInputData gatherInputData( &getInputSystem(), &mKeyboardEventGenerator, &mMouseEventGenerator, getKeyboard() );
		GameControl gameCtrl(gatherInputData);

		// objects
		setupLights();
		//setupNinja();
		setupGround();
		setupDotScene();
		//setupCar();
		setupVehicle(gameCtrl);

		/*CameraFollowController cfc( mVPs[0].second, mVPs[0].second, dbgChassis );
		mVPs[0].second->setFixedYawEnabled( true );
		mVPs[0].second->setFixedYawAxis( Vector3(0,1,0) );
		mVPs[0].second->setPosition( Vector3(10,2,30) );*/

		// tasks
		mRenderTask = new RenderTask( mGWorld.get() );
		YAKE_ASSERT( mRenderTask );
		mTaskMgr.addTask( mRenderTask );

		mPhysicsTask = new PhysicsTask( mPWorld );
		YAKE_ASSERT( mPhysicsTask );
		mTaskMgr.addTask( mPhysicsTask );

		mInputTask = new InputTask( this );
		YAKE_ASSERT( mInputTask );
		mTaskMgr.addTask( mInputTask );

		// main loop
		mPawn1 = setupPawn(gameCtrl);
		//gameCtrl.subscribeToGatherInputStep( Bind1( &InputTask::execute, mInputTask ) );
		gameCtrl.subscribeToPhysicsStep( Bind1( &PhysicsTask::execute, mPhysicsTask ) );
		gameCtrl.subscribeToRenderStep( Bind1( &RenderTask::execute, mRenderTask ) );
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();

			gameCtrl.update();
		}
		mRenderTask = 0;
		mPhysicsTask = 0;
		mInputTask = 0;

		// clean up (FIXME: still incomplete)
		// TODO: use a yake::base::State for handling all scene specific stuff...

		cleanupVehicle();

		YAKE_SAFE_DELETE( mLightOneNode );

		//YAKE_SAFE_DELETE( mCar );
		//YAKE_SAFE_DELETE( mCarModel ); is done by destructor of mCar

		YAKE_SAFE_DELETE( mPWorld );
		mGWorld.reset();

		//profiler::ProfilerManager::instance().getProfiler("default").logToCout();
		PROFILER_DESTROY;
		PROFILERMANAGER_DESTROY;
	}
};

	TheApp::RenderTask::RenderTask(IWorld* pWorld) :
		DefTask( -1, task::TASKPRIO_HIGH ),
		mWorld(pWorld)
	{
		YAKE_ASSERT( mWorld );
	}
	void TheApp::RenderTask::execute(real timeElapsed)
	{
		YAKE_ASSERT( mWorld );
		mWorld->render( timeElapsed );
		//std::cout << "render(" << timeElapsed << ")!\n";
	}

	TheApp::PhysicsTask::PhysicsTask(physics::IWorld* pWorld) :
		DefTask( -1, task::TASKPRIO_HIGH+1 ),
		mWorld(pWorld)
	{
		YAKE_ASSERT( mWorld );
	}
	void TheApp::PhysicsTask::execute(real timeElapsed)
	{
		//std::cout << "physics(" << timeElapsed << ")!\n";
		YAKE_ASSERT( mWorld );
		mWorld->update( timeElapsed );
	}

	TheApp::InputTask::InputTask(
			TheApp* theApp ) :
		DefTask( -1, task::TASKPRIO_HIGH+2 ),
		mApp(theApp)
	{
		YAKE_ASSERT( mApp );
	}
	void TheApp::InputTask::execute(real timeElapsed)
	{
		//std::cout << "input(" << timeElapsed << ")!\n";
		YAKE_ASSERT( mApp );

		// handle input
		mApp->getInputSystem().update();
		mApp->mMouseEventGenerator.update();
		mApp->mKeyboardEventGenerator.update();

		//
		YAKE_ASSERT( mApp->getKeyboard() );
		if (mApp->getKeyboard())
		{
			static iCam = 0;
			real distance = -5. * timeElapsed;
			if (mApp->getKeyboard()->isKeyDown(input::KC_LEFT))
				mApp->mVPs[iCam].second->translate( Vector3(distance, 0, 0) );
			if (mApp->getKeyboard()->isKeyDown(input::KC_RIGHT))
				mApp->mVPs[iCam].second->translate( Vector3(-distance, 0, 0) );
			if (mApp->getKeyboard()->isKeyDown(input::KC_UP))
				mApp->mVPs[iCam].second->translate( Vector3(0, 0, distance) );
			if (mApp->getKeyboard()->isKeyDown(input::KC_DOWN))
				mApp->mVPs[iCam].second->translate( Vector3(0, 0, -distance) );

/*
			if (mApp->getKeyboard()->isKeyDown(input::KC_A))
				mCar->setSteer( -0.7 );
			else if (mApp->getKeyboard()->isKeyDown(input::KC_D))
				mCar->setSteer( 0.7 );
			else
				mCar->setSteer( 0 );

			if (mApp->getKeyboard()->isKeyDown(input::KC_W))
			{
				mCar->getEngine()->setThrottle(
					mCar->getEngine()->getThrottle() + timeElapsed );
			}
			else
				mCar->getEngine()->setThrottle(
					mCar->getEngine()->getThrottle() - 2. * timeElapsed );
			static real last = 0.;
			last -= timeElapsed;
			if (last < 0. && mApp->getKeyboard()->isKeyDown(input::KC_RETURN))
			{
				mCar->getEngine()->shiftGearUp();
				last = 0.2;
			}
			if (last < 0. && mApp->getKeyboard()->isKeyDown(input::KC_RSHIFT))
			{
				mCar->getEngine()->shiftGearDown();
				last = 0.2;
			}
			if (mApp->getKeyboard()->isKeyDown(input::KC_SPACE))
				mCar->brake( 0, 1. );
			else
				mCar->brake( 0, 0. );
			*/
		}
	}
//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main()
{
	try
	{
		std::cout << std::endl << "A simple vehicle demo :) provided for YAKE by Stephan Kaiser" << std::endl;
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