#include <yapp/samples/misc/vehicle/yakePCH.h>
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
const input::ActionId ACTIONID_TOGGLE_VEHICLE( input::ACTIONID_USER+2, "toggle_vehicle" );
const real razorMeshScale = 0.1;

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
		mGround(0),
		mActive(0)
	{
		enableInstantQuitByKey( input::KC_ESCAPE );
	}
	~TheMainState()
	{
	}
private:
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
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.5f, 0.01f ), "wheel" );

		// vehicle(s)

		YAKE_ASSERT( mVehicleSystem.get() );
		mVehicleSystem->loadTemplates("../../media/vehicles/delorean.xml");
		mVehicleSystem->loadTemplates("../../media/vehicles/jet.xml");

		// It's possible retrieve the template and make further adjustments to it before
		// instantiating vehicles.
		// e.g.: vehicle::VehicleTemplate* tpl = pVS->getTemplate("delorean");

		// sky box
		getGraphicalWorld()->setSkyBox("Examples/SpaceSkyBox");

		// create car
		mCar.reset( new Car() );
		mCar->init(mCentralController,mModelMgr,*getGraphicalWorld(),*mVehicleSystem);

		// create Jet
		mJet.reset( new Jet() );
		mJet->init(mCentralController,mModelMgr,*getGraphicalWorld(),*mVehicleSystem);
	}
	virtual void onDestroyScene()
	{
		mCar.reset();
		mJet.reset();

		mModelMgr.clear();
		mVehicleSystem.reset();

		YAKE_SAFE_DELETE( mGround );
	}
	virtual void onEnter()
	{
		RtMainState::onEnter();

		mActionMap.reg( ACTIONID_TOGGLE_VEHICLE,
			new input::KeyboardActionCondition( getApp().getKeyboard(), input::KC_T, input::KAM_RELEASED ) );
		mActionMap.subscribeToActionId( ACTIONID_TOGGLE_VEHICLE, boost::bind(&TheMainState::toggleVehicle,this) );

		YAKE_ASSERT( !mActive );
		mActive = mCar.get();
		mActive->onEnter(mActionMap,getApp().getKeyboard());
	}
	virtual void onExit()
	{
		YAKE_ASSERT( mActive );
		if (mActive)
		{
			mActive->onExit();
			mActive = 0;
		}

		RtMainState::onExit();
	}
	virtual void onFrame(const real timeElapsed)
	{
		mActionMap.update();

		mCentralController.triggerPhysicsUpdateSignal(0,timeElapsed);
		mCentralController.triggerGraphicsUpdateSignal(0,timeElapsed);

		if (mCar)
			mCar->onFrame( timeElapsed );
		if (mJet)
			mJet->onFrame( timeElapsed );
		if (mActive)
		{
			getDefaultCamera()->lookAt(mActive->getPosition());
		}
	}
private:
	model::Model*						mGround;

	model::ModelManager					mModelMgr;
	model::CentralControllerBase		mCentralController;
	SharedPtr<vehicle::IVehicleSystem>	mVehicleSystem;

	input::ActionMap					mActionMap;

	struct GoVehicleBase
	{
		virtual ~GoVehicleBase() {}
		virtual Vector3 getPosition() const = 0;
		virtual void onEnter(input::ActionMap& actionMap, input::KeyboardDevice* keyboard) = 0;
		virtual void onExit()
		{
			for( SignalConnectionList::iterator it = mInputSignalConnections.begin(); it != mInputSignalConnections.end(); ++it )
				it->disconnect();
			mInputSignalConnections.clear();

			for( ConditionConnectionList::iterator it = mInputConditionConnections.begin(); it != mInputConditionConnections.end(); ++it )
				it->disconnect();
			mInputConditionConnections.clear();
		}
		virtual void onFrame(const real timeElapsed) = 0;
		void addInputSignalConnection(const SignalConnection& conn)
		{
			mInputSignalConnections.push_back( conn );
		}
		void addInputConditionConnection(const input::ConditionConnection& conn)
		{
			mInputConditionConnections.push_back( conn );
		}
	private:
		typedef std::deque<SignalConnection> SignalConnectionList;
		SignalConnectionList	mInputSignalConnections;
		typedef std::deque<input::ConditionConnection> ConditionConnectionList;
		ConditionConnectionList	mInputConditionConnections;
	};
	struct Car : public GoVehicleBase
	{
		Car() : mModel(0), mVehicle(0)
		{}
		~Car()
		{
			destroy();
		}
		virtual Vector3 getPosition() const
		{
			YAKE_ASSERT( mVehicle );
			return mVehicle->getChassisPosition();
		}

		void init(model::CentralControllerBase& centralController,model::ModelManager& modelMgr,graphics::IWorld& gworld,vehicle::IVehicleSystem& vehSys)
		{
			// instantiate vehicle model (physics + representation containers)
			YAKE_ASSERT( !mModel );
			mModel = modelMgr.createModel("theCar",
					"physics/empty:name=p"
					"|graphics/empty:name=g"
					"|yake/dotVehicle:name=veh;template=delorean;physical=p"
					);
			YAKE_ASSERT( mModel );
			vehicle::VehicleModelComponent* vehComp = static_cast<vehicle::VehicleModelComponent*>(mModel->getComponentByTag("veh"));
			YAKE_ASSERT( vehComp );
			mVehicle = vehComp->getVehicle();
			YAKE_ASSERT( mVehicle );

			// create visuals

			model::Graphical* pG = dynamic_cast<model::Graphical*>(mModel->getComponentByTag("g"));
			YAKE_ASSERT( pG );

			//mVehicle->enableDebugGeometry( *getGraphicalWorld() );

			// create visuals
			// - ship body
			graphics::ISceneNode* pSN = gworld.createSceneNode("root");
			graphics::IEntity* pE = gworld.createEntity("delorean.mesh");
			pE->setCastsShadow( true );
			pSN->attachEntity( pE );
			//pSN->setScale( Vector3::kUnitScale * razorMeshScale );
			pG->addSceneNode(pSN, "delorean_root");

			// - wheel visuals
			if (mVehicle->getWheelInterface("leftFrontWheel"))
			{
				_createCarWheelVisual( centralController, gworld, "leftFrontWheel", *pSN );
				_createCarWheelVisual( centralController, gworld, "rightFrontWheel", *pSN );
				_createCarWheelVisual( centralController, gworld, "leftRearWheel", *pSN );
				_createCarWheelVisual( centralController, gworld, "rightRearWheel", *pSN );
			}

			// create visual <-> physics links
			model::ModelMovableLink* pLink = mModel->createLink( mVehicle->getChassisMovable(), pSN );
			centralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );
		}
		void destroy()
		{
			//@todo
			mModel = 0;
			mVehicle = 0;
		}
		virtual void onEnter(input::ActionMap& actionMap, input::KeyboardDevice* keyboard)
		{
			using namespace input;

			// front wheel controls

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_LEFT,
					new input::KeyboardActionCondition( keyboard, KC_LEFT, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_LEFT, boost::bind(&Car::onFrontWheelLeft,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_RIGHT,
					new input::KeyboardActionCondition( keyboard, KC_RIGHT, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_RIGHT, boost::bind(&Car::onFrontWheelRight,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_FORWARD,
					new input::KeyboardActionCondition( keyboard, KC_UP, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_FORWARD, boost::bind(&Car::onForward,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_BRAKE,
					new input::KeyboardActionCondition( keyboard, KC_DOWN, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_BRAKE, boost::bind(&Car::onReverse,this) ) );
		}
		void onExit()
		{
			GoVehicleBase::onExit();
		}
		virtual void onFrame(const real timeElapsed)
		{
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

			ConstDequeIterator< WheelPsMap > itWheel( mWheelPs );
			while (itWheel.hasMoreElements())
			{
				const String wheelId = itWheel.getNext().first;
				mWheelPs[ wheelId ]->setEmissionRate( 0, mEmitterEmissionRate[wheelId] * mVehicle->getWheelInterface( wheelId )->getSkid() );
			}

			mActiveActions.clear();
		}
	private:
		vehicle::IVehicle*		mVehicle;
		model::Model*			mModel;
		typedef AssocVector<String,real> EmitterRealMap;
		EmitterRealMap			mEmitterMinVel;
		EmitterRealMap			mEmitterMaxVel;
		EmitterRealMap			mEmitterEmissionRate;
		typedef AssocVector<String,graphics::IParticleSystem*> WheelPsMap;
		WheelPsMap				mWheelPs;

		typedef std::set<input::ActionId> ActionIdList;
		ActionIdList			mActiveActions;

	private:
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
		void _createCarWheelVisual(model::CentralControllerBase& centralController, graphics::IWorld& gworld, const String& wheelId, graphics::ISceneNode& parentSN)
		{
			YAKE_ASSERT( mModel );
			YAKE_ASSERT( mVehicle );

			// scene node + wheel mesh
			graphics::ISceneNode* pSN = gworld.createSceneNode();
			graphics::IEntity* pE = gworld.createEntity("wheel1.mesh");
			pE->setCastsShadow( true );
			pSN->attachEntity( pE );

			model::ModelMovableLink* pLink = mModel->createLink( mVehicle->getWheelInterface(wheelId), pSN );
			centralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );

			// scene node + particle system for smoke...
			pSN = gworld.createSceneNode();
			graphics::IParticleSystem* pPS = gworld.createParticleSystem("SkidSmoke");
			mWheelPs[wheelId] = pPS;
			mEmitterEmissionRate[wheelId] = pPS->getEmissionRate(0);
			pSN->attachParticleSystem( pPS );

			pLink = new model::ModelMovableDirectLink();
			pLink->setSource( mVehicle->getWheelInterface(wheelId) );
			pLink->subscribeToPositionChanged( pSN ); //position only!
			mModel->addLink( pLink );
			centralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );
		}
	};

	struct Jet : public GoVehicleBase
	{
		Jet() : mModel(0), mVehicle(0)
		{}
		~Jet()
		{
			destroy();
		}
		virtual Vector3 getPosition() const
		{
			YAKE_ASSERT( mVehicle );
			return mVehicle->getChassisPosition();
		}
		void init(model::CentralControllerBase& centralController,model::ModelManager& modelMgr,graphics::IWorld& gworld,vehicle::IVehicleSystem& vehSys)
		{
			YAKE_ASSERT( !mModel );
			mModel = modelMgr.createModel("theJet",
					"physics/empty:name=p"
					"|graphics/empty:name=g"
					"|yake/dotVehicle:name=veh;template=jet;physical=p"
					);
			YAKE_ASSERT( mModel );
			vehicle::VehicleModelComponent* vehComp = static_cast<vehicle::VehicleModelComponent*>(mModel->getComponentByTag("veh"));
			YAKE_ASSERT( vehComp );
			mVehicle = vehComp->getVehicle();
			YAKE_ASSERT( mVehicle );

			mVehicle->translate(Vector3(-30,0,30));

			// create visuals

			model::Graphical* pG = dynamic_cast<model::Graphical*>(mModel->getComponentByTag("g"));
			YAKE_ASSERT( pG );

			// - ship body
			graphics::ISceneNode* pSN = gworld.createSceneNode();
			pSN->attachEntity( gworld.createEntity("razor.mesh") );
			pSN->setScale( math::Vector3::kUnitScale * razorMeshScale );
			pG->addSceneNode(pSN,"root");

			YAKE_LOG( "Creating thruster visuals" );
			// - thruster visuals
			_createThrusterVisual( centralController, gworld, "left", "left", *pSN );
			_createThrusterVisual( centralController, gworld, "right", "right", *pSN );
			_createThrusterVisual( centralController, gworld, "front", "backward", *pSN );
			_createThrusterVisual( centralController, gworld, "rear", "forward", *pSN );
			_createThrusterVisual( centralController, gworld, "bottom", "upward", *pSN );
			_createThrusterVisual( centralController, gworld, "top", "downward", *pSN );

			// - wheel visuals
			if (mVehicle->getWheelInterface("frontWheel"))
			{
				_createWheelVisual( centralController, gworld, "frontWheel", *pSN );
				_createWheelVisual( centralController, gworld, "leftRearWheel", *pSN );
				_createWheelVisual( centralController, gworld, "rightRearWheel", *pSN );
			}

			// create visual <-> physics links
			model::ModelMovableLink* pLink = mModel->createLink( mVehicle->getChassisMovable(), pSN );
			centralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );
		}
		void destroy()
		{
			//@todo
			mModel = 0;
			mVehicle = 0;
		}
		virtual void onEnter(input::ActionMap& actionMap, input::KeyboardDevice* keyboard)
		{
			using namespace input;

			// front wheel controls

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_LEFT,
					new input::KeyboardActionCondition( keyboard, KC_COMMA, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_LEFT, boost::bind(&Jet::onFrontWheelLeft,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_RIGHT,
					new input::KeyboardActionCondition( keyboard, KC_PERIOD, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_RIGHT, boost::bind(&Jet::onFrontWheelRight,this) ) );

			// thruster controls

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_STRAFE_LEFT,
					new input::KeyboardActionCondition( keyboard, KC_LEFT, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_STRAFE_LEFT, boost::bind(&Jet::onStrafeLeft,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_STRAFE_RIGHT,
					new input::KeyboardActionCondition( keyboard, KC_RIGHT, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_STRAFE_RIGHT, boost::bind(&Jet::onStrafeRight,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_FORWARD,
					new input::KeyboardActionCondition( keyboard, KC_UP, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_FORWARD, boost::bind(&Jet::onForward,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_REVERSE,
					new input::KeyboardActionCondition( keyboard, KC_DOWN, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_REVERSE, boost::bind(&Jet::onReverse,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_UP,
					new input::KeyboardActionCondition( keyboard, KC_PGUP, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_UP, boost::bind(&Jet::onUp,this) ) );

			this->addInputConditionConnection(
				actionMap.reg( ACTIONID_DOWN,
					new input::KeyboardActionCondition( keyboard, KC_PGDOWN, KAM_CONTINUOUS ) ) );
			this->addInputSignalConnection(
				actionMap.subscribeToActionId( ACTIONID_DOWN, boost::bind(&Jet::onDown,this) ) );
		}
		virtual void onExit()
		{
			GoVehicleBase::onExit();
		}
		virtual void onFrame(const real timeElapsed)
		{
			vehicle::IEnginePtrList engines = mVehicle->getEngineInterfaces();
			ConstDequeIterator< vehicle::IEnginePtrList > itEngine( engines );
			while (itEngine.hasMoreElements())
			{
				vehicle::IEngine* pEngine = itEngine.getNext();
				pEngine->setInputSignal( std::max(0., pEngine->getInputSignal() - timeElapsed * 1.7) );
			}

			real steering0 = 0;

			ConstDequeIterator< ActionIdList > itAction( mActiveActions );
			while (itAction.hasMoreElements())
			{
				const input::ActionId activeId = itAction.getNext();
				if (activeId == input::ACTIONID_STRAFE_LEFT)
					mVehicle->getEngineInterface("right")->setInputSignal(1.);
				else if (activeId == input::ACTIONID_STRAFE_RIGHT)
					mVehicle->getEngineInterface("left")->setInputSignal(1.);
				else if (activeId == input::ACTIONID_FORWARD)
					mVehicle->getEngineInterface("forward")->setInputSignal(1.);
				else if (activeId == input::ACTIONID_REVERSE)
					mVehicle->getEngineInterface("backward")->setInputSignal(1.);
				else if (activeId == input::ACTIONID_UP)
					mVehicle->getEngineInterface("upward")->setInputSignal(1.);
				else if (activeId == input::ACTIONID_DOWN)
					mVehicle->getEngineInterface("downward")->setInputSignal(1.);
				else if (activeId == input::ACTIONID_LEFT)
					steering0 -= 0.3;
				else if (activeId == input::ACTIONID_RIGHT)
					steering0 += 0.3;
			}
			mVehicle->setSteering( 0, steering0 );

			mVehicle->updateSimulation( timeElapsed );

			_updateThrusterPs();

			mActiveActions.clear();
		}
	private:
		vehicle::IVehicle*		mVehicle;
		model::Model*			mModel;

		typedef AssocVector<String,real> EmitterRealMap;
		EmitterRealMap			mEmitterMinVel;
		EmitterRealMap			mEmitterMaxVel;
		EmitterRealMap			mEmitterEmissionRate;
		typedef AssocVector<String,graphics::IParticleSystem*> ThrusterPsMap;
		ThrusterPsMap			mThrusterPs;
		input::ActionMap		mActionMap;

		typedef std::set<input::ActionId> ActionIdList;
		ActionIdList			mActiveActions;

	private:
		void onStrafeLeft()
		{ mActiveActions.insert( input::ACTIONID_STRAFE_LEFT ); }
		void onStrafeRight()
		{ mActiveActions.insert( input::ACTIONID_STRAFE_RIGHT ); }
		void onForward()
		{ mActiveActions.insert( input::ACTIONID_FORWARD ); }
		void onReverse()
		{ mActiveActions.insert( input::ACTIONID_REVERSE ); }
		void onUp()
		{ mActiveActions.insert( input::ACTIONID_UP ); }
		void onDown()
		{ mActiveActions.insert( input::ACTIONID_DOWN ); }
		void onFrontWheelLeft()
		{ mActiveActions.insert( input::ACTIONID_LEFT ); }
		void onFrontWheelRight()
		{ mActiveActions.insert( input::ACTIONID_RIGHT ); }
		void _createThrusterVisual(model::CentralControllerBase& centralController, graphics::IWorld& gworld, const String& mtPtId, const String& engineId, graphics::ISceneNode& parentSN)
		{
			graphics::ISceneNode* pSN = parentSN.createChildNode( mtPtId );

			graphics::IParticleSystem* pPS = gworld.createParticleSystem("thruster2");
			pSN->attachParticleSystem( pPS );

			YAKE_ASSERT( mModel );
			YAKE_ASSERT( mVehicle );
			model::ModelMovableLink* pLink = mModel->createLink( mVehicle->getMountPoint(mtPtId), pSN );
			centralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );

			_regThrusterPs( engineId, *pPS );
		}
		void _createWheelVisual(model::CentralControllerBase& centralController, graphics::IWorld& gworld, const String& wheelId, graphics::ISceneNode& parentSN)
		{
			graphics::ISceneNode* pSN = gworld.createSceneNode();
			graphics::IEntity* pE = gworld.createEntity("sphere_d1.mesh");
			pSN->attachEntity( pE );
			const math::Vector3 scale = math::Vector3::kUnitScale * 2.f * mVehicle->getWheelInterface(wheelId)->getRadius();
			pSN->setScale( scale );

			YAKE_ASSERT( mModel );
			YAKE_ASSERT( mVehicle );
			model::ModelMovableLink* pLink = mModel->createLink( mVehicle->getWheelInterface(wheelId), pSN );
			centralController.subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );
		}
		void _regThrusterPs(const String& engineId, graphics::IParticleSystem& ps)
		{
			mEmitterMinVel[ engineId ] = ps.getMinVelocity(0);
			mEmitterMaxVel[ engineId ] = ps.getMaxVelocity(0);
			mEmitterEmissionRate[ engineId ] = ps.getEmissionRate(0);
			mThrusterPs[ engineId ] = &ps;
		}
		void _updateThrusterPs()
		{
			ConstDequeIterator< ThrusterPsMap > itM( mThrusterPs );
			while (itM.hasMoreElements())
			{
				std::pair<String,graphics::IParticleSystem*> p = itM.getNext();
				const String mtPtId = p.first;
				graphics::IParticleSystem* ps = p.second;
				const real throttle = mVehicle->getEngineInterface(mtPtId)->getInputSignal();

				real rate = throttle * mEmitterEmissionRate[ mtPtId ];
				ps->setEmissionRate( 0, rate );
				std::cout << "r=" << rate << " t=" << throttle << "\n";

				real vel = throttle * mEmitterMaxVel[ mtPtId ];
				ps->setMaxVelocity( 0, vel );

				vel = throttle * mEmitterMinVel[ mtPtId ];
				ps->setMinVelocity( 0, vel );
			}
		}
	};
	SharedPtr<Jet>				mJet;
	SharedPtr<Car>				mCar;
	GoVehicleBase*				mActive;
	void toggleVehicle()
	{
		// 1st time
		if (!mActive)
			mActive = mCar.get();
		else
			mActive->onExit();

		// toggle
		if (mActive == static_cast<GoVehicleBase*>(mJet.get()))
			mActive = mCar.get();
		else
			mActive = mJet.get();

		mActive->onEnter(mActionMap,getApp().getKeyboard());
	}
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

