#include <yapp/samples/misc/car/yakePCH.h>
#include <yake/audio/yakeAudio.h>
#include <yapp/raf/yakeRaf.h>
#include <yake/model/model.h>
#include <yake/model/model_link.h>
#include <yapp/vehicle/yakeVehicle.h>
#include <yake/input/yakeInput.h>

// temporary stuff (until factory registration issues have been sorted out):
#include <yapp/vehicle/yakeNativeOde.h>

using namespace yake;

const input::ActionId ACTIONID_BRAKE( input::ACTIONID_USER+1, "brake" );

/** Configuration of the application */
struct TheConfiguration : public raf::ApplicationConfiguration
{
	virtual StringVector getLibraries()
	{ return MakeStringVector() << "graphicsOgre" << "inputOgre" << "physicsOde"; }

	//virtual StringVector getScriptingSystems()
	//{ return MakeStringVector() << "lua"; }

	virtual StringVector getGraphicsSystems()
	{ return MakeStringVector() << "ogre3d"; }

	virtual StringVector getInputSystems()
	{ return MakeStringVector() << "ogre"; }

	virtual StringVector getPhysicsSystems()
	{ return MakeStringVector() << "ode"; }
};

class VehicleHumanInputController
{
public:
	void setVehicle(vehicle::IVehicle*);
};

/** Main application state */
class TheMainState : public raf::RtMainState
{
public:
	TheMainState(raf::Application& owner) :
		raf::RtMainState(owner),
		mVehicle(0),
		mVehicleModel(0),
		mGround(0)
	{
		enableInstantQuitByKey( input::KC_ESCAPE );
	}
	~TheMainState()
	{
	}
private:
	void _createWheelVisual(model::Model& theModel, vehicle::IVehicle& theVehicle, const String& wheelId, graphics::ISceneNode& parentSN)
	{
		// scene node + wheel mesh
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode();
		graphics::IEntity* pE = getGraphicalWorld()->createEntity("wheel1.mesh");
		pE->setCastsShadow( true );
		pSN->attachEntity( pE );
		model::ModelMovableLink* pLink = theModel.createLink( theVehicle.getWheelInterface(wheelId), pSN );
		mCentralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );

		// scene node + particle system for smoke...
		pSN = getGraphicalWorld()->createSceneNode();
		graphics::IParticleSystem* pPS = getGraphicalWorld()->createParticleSystem("SkidSmoke");
		mWheelPs[wheelId] = pPS;
		mEmitterEmissionRate[wheelId] = pPS->getEmissionRate(0);
		pSN->attachParticleSystem( pPS );
		pLink = new model::ModelMovableDirectLink();
		pLink->setSource( theVehicle.getWheelInterface(wheelId) );
		pLink->subscribeToPositionChanged( pSN ); //position only!
		theModel.addLink( pLink );
		mCentralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );
	}
	void onModelComponent_preInit(const model::ComponentCreationContext& ctx, model::ModelComponent&)
	{
		YAKE_LOG("demo: onModelComponent_preInit()");
	}
	void onModelComponent_postInit(const model::ComponentCreationContext& ctx, model::ModelComponent&)
	{
		YAKE_LOG("demo: onModelComponent_postInit()");
	}
protected:
	virtual void onCreateScene()
	{
		YAKE_LOG_INFORMATION("Creating scene");

		// create vehicle system
		mVehicleSystem.reset(new vehicle::GenericVehicleSystem());

		// set up the model creation context.
		mModelMgr.setCreationContext_GraphicalWorld( getGraphicalWorld() );
		mModelMgr.setCreationContext_PhysicalWorld( getPhysicalWorld() );
		mModelMgr.setCreationContext_CentralController( &mCentralController );
		mModelMgr.setCreationContext_NamedParameter("vehicleSystem",static_cast<vehicle::IVehicleSystem*>(mVehicleSystem.get()));

		// set up model component creation hooks.
		mModelMgr.subscribeToComponentPreInitializeSignal( Bind2(&TheMainState::onModelComponent_preInit,this) );
		mModelMgr.subscribeToComponentPostInitializeSignal( Bind2(&TheMainState::onModelComponent_postInit,this) );



		// create a light
		graphics::ILight* pLight = getGraphicalWorld()->createLight();
		pLight->setType( graphics::ILight::LT_DIRECTIONAL );
		pLight->setDirection( Vector3(0,-1,1) );
		getGraphicalWorld()->createSceneNode("lightnode0")->attachLight( pLight );

		getGraphicalWorld()->setShadowsEnabled( true );

		// set up camera
		getDefaultCamera()->setNearClipDistance( 1 );
		getDefaultCamera()->setFixedYawAxis(Vector3::kUnitY);
		getDefaultCamera()->setPosition(Vector3(7,4,-7));

		//
		getPhysicalWorld()->setGlobalGravity(Vector3(0,-9.81,0));

		// create ground
		mGround = new model::Model();
		{
			const real groundHeight = -2;
			// visual
			graphics::ISceneNode* pGroundSN = getGraphicalWorld()->createSceneNode();
			graphics::IEntity* pGroundE = getGraphicalWorld()->createEntity("plane_1x1.mesh");
			pGroundE->setMaterial("box");
			pGroundE->setCastsShadow( false );
			pGroundSN->attachEntity( pGroundE );
			pGroundSN->setScale( Vector3(200,1,200) );
			pGroundSN->setPosition( Vector3(0,groundHeight,0) );

			model::Graphical* pG = new model::Graphical();
			pG->addSceneNode( pGroundSN, "groundSN" );
			mGround->addComponent( pG );

			// physical
			physics::IActorPtr pGroundPlane = getPhysicalWorld()->createActor( physics::ACTOR_STATIC );
			pGroundPlane->createShape( physics::IShape::PlaneDesc( Vector3(0,1,0), groundHeight ) );

			model::Physical* pP = new model::Physical();
			pP->addActor( pGroundPlane, "groundPlane" );
			mGround->addComponent( pP );
		}

		// materials @todo read from .physics:
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.01f, 0.01f ), "chassis" );
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.2f, 0.01f ), "chassisTop" );
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.25f, 0.01f ), "wheel" );

		// vehicle(s)

		YAKE_ASSERT( mVehicleSystem.get() );
		mVehicleSystem->loadTemplates("../../media/vehicles/delorean.xml");

		// It's possible retrieve the template and make further adjustments to it before
		// instantiating vehicles.
		// e.g.: vehicle::VehicleTemplate* tpl = pVS->getTemplate("delorean");

		// instantiate vehicle model (physics + representation containers)
		mVehicleModel = mModelMgr.createModel("theCar",
				"physics/empty:name=p"
				"|graphics/empty:name=g"
				"|yake/dotVehicle:name=veh;template=delorean;physical=p"
				);
		YAKE_ASSERT( mVehicleModel );
		vehicle::VehicleModelComponent* vehComp = static_cast<vehicle::VehicleModelComponent*>(mVehicleModel->getComponentByTag("veh"));
		YAKE_ASSERT( vehComp );
		mVehicle = vehComp->getVehicle();
		YAKE_ASSERT( mVehicle );

		//mVehicle->enableDebugGeometry( *getGraphicalWorld() );

		// create visuals
		// - ship body
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode("root");
		graphics::IEntity* pE = getGraphicalWorld()->createEntity("delorean.mesh");
		pE->setCastsShadow( true );
		pSN->attachEntity( pE );
		//pSN->setScale( Vector3::kUnitScale * razorMeshScale );
		model::Graphical* pG = dynamic_cast<model::Graphical*>(mVehicleModel->getComponentByTag("g"));
		YAKE_ASSERT( pG );
		pG->addSceneNode(pSN, "delorean_root");

		// - wheel visuals
		if (mVehicle->getWheelInterface("leftFrontWheel"))
		{
			_createWheelVisual( *mVehicleModel, *mVehicle, "leftFrontWheel", *pSN );
			_createWheelVisual( *mVehicleModel, *mVehicle, "rightFrontWheel", *pSN );
			_createWheelVisual( *mVehicleModel, *mVehicle, "leftRearWheel", *pSN );
			_createWheelVisual( *mVehicleModel, *mVehicle, "rightRearWheel", *pSN );
		}

		// create visual <-> physics links
		model::ModelMovableLink* pLink = mVehicleModel->createLink( mVehicle->getChassisMovable(), pSN );
		mCentralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );

		// sky box
		getGraphicalWorld()->setSkyBox("Examples/SpaceSkyBox");
	}
	virtual void onDestroyScene()
	{
		mVehicleModel = 0;
		mVehicle = 0;

		mModelMgr.clear();
		mVehicleSystem.reset();

		YAKE_SAFE_DELETE( mGround );
	}
	virtual void onEnter()
	{
		RtMainState::onEnter();
		using namespace input;

		// front wheel controls

		mActionMap.reg( ACTIONID_LEFT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_LEFT, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_LEFT, boost::bind(&TheMainState::onFrontWheelLeft,this) );

		mActionMap.reg( ACTIONID_RIGHT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_RIGHT, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_RIGHT, boost::bind(&TheMainState::onFrontWheelRight,this) );

		mActionMap.reg( ACTIONID_FORWARD,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_UP, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_FORWARD, boost::bind(&TheMainState::onForward,this) );

		mActionMap.reg( ACTIONID_BRAKE,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_DOWN, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_BRAKE, boost::bind(&TheMainState::onReverse,this) );

		//mActionMap.reg( ACTIONID_UP,
		//	new input::KeyboardActionCondition( getApp().getKeyboard(), KC_PGUP, KAM_CONTINUOUS ) );
		//mActionMap.subscribeToActionId( ACTIONID_UP, boost::bind(&TheMainState::onUp,this) );
	}
	virtual void onExit()
	{
		RtMainState::onExit();
	}
	virtual void onFrame(const real timeElapsed)
	{
		mActionMap.update();

		vehicle::IEnginePtrList engines = mVehicle->getEngineInterfaces();
		ConstDequeIterator< vehicle::IEnginePtrList > itEngine( engines );
		while (itEngine.hasMoreElements())
		{
			vehicle::IEngine* pEngine = itEngine.getNext();
			pEngine->setInputSignal( pEngine->getInputSignal() - timeElapsed * 1.7 );
		}

		real steering0 = 0;
		real braking0 = 0;

		ConstDequeIterator< ActionIdList > itAction( mActiveActions );
		while (itAction.hasMoreElements())
		{
			const input::ActionId activeId = itAction.getNext();
			if (activeId == input::ACTIONID_FORWARD)
				mVehicle->getEngineInterface("main")->setInputSignal(1.);
			else if (activeId == ACTIONID_BRAKE)
				braking0 = 1.;
			else if (activeId == input::ACTIONID_LEFT)
				steering0 -= real(0.7);
			else if (activeId == input::ACTIONID_RIGHT)
				steering0 += real(0.7);
		}
		mVehicle->setSteering( 0, steering0 );
		mVehicle->getWheelInterface("leftFrontWheel")->brake(braking0);
		mVehicle->getWheelInterface("rightFrontWheel")->brake(braking0);

		mVehicle->updateSimulation( timeElapsed );
		//mVehicleModel->updatePhysics( timeElapsed );

		ConstDequeIterator< WheelPsMap > itWheel( mWheelPs );
		while (itWheel.hasMoreElements())
		{
			const String wheelId = itWheel.getNext().first;
			mWheelPs[ wheelId ]->setEmissionRate( 0, mEmitterEmissionRate[wheelId] * mVehicle->getWheelInterface( wheelId )->getSkid() );
		}

		getDefaultCamera()->lookAt(mVehicle->getChassisPosition());
		//mVehicleModel->updateGraphics( timeElapsed );

		mCentralController.triggerPhysicsUpdateSignal(0,timeElapsed);
		mCentralController.triggerGraphicsUpdateSignal(0,timeElapsed);

		mActiveActions.clear();
	}
protected:
	void onForward()
	{ mActiveActions.insert( input::ACTIONID_FORWARD ); }
	void onReverse()
	{ mActiveActions.insert( ACTIONID_BRAKE ); }
	void onUp()
	{ mActiveActions.insert( input::ACTIONID_UP ); }
	void onFrontWheelLeft()
	{ mActiveActions.insert( input::ACTIONID_LEFT ); }
	void onFrontWheelRight()
	{ mActiveActions.insert( input::ACTIONID_RIGHT ); }
private:
private:
	model::Model*			mGround;
	vehicle::IVehicle*		mVehicle;
	model::Model*			mVehicleModel;
	typedef AssocVector<String,real> EmitterRealMap;
	EmitterRealMap			mEmitterMinVel;
	EmitterRealMap			mEmitterMaxVel;
	EmitterRealMap			mEmitterEmissionRate;
	typedef AssocVector<String,graphics::IParticleSystem*> WheelPsMap;
	WheelPsMap				mWheelPs;
	input::ActionMap		mActionMap;

	typedef std::set<input::ActionId> ActionIdList;
	ActionIdList			mActiveActions;

	model::ModelManager					mModelMgr;
	model::CentralControllerBase		mCentralController;
	SharedPtr<vehicle::IVehicleSystem>	mVehicleSystem;
};

/** The mighty application itself! */
class TheApp : public raf::ExampleApplication<TheConfiguration>
{
public:
	TheApp() {}
protected:
	virtual raf::MainState* createMainState()
	{
		return new TheMainState(*this);
	}
};

int main( int argc, char** argv )
{
	// Use default executor for convenience.
	// It's always possible to manually execute TheApp::initialise() etc.
	TheApp theApp;
	return (raf::runApplication( theApp )) ? 0 : 1;
}

