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
		mComplex(0),
		mGround(0)
	{
		enableInstantQuitByKey( input::KC_ESCAPE );
	}
	~TheMainState()
	{
	}
private:
	typedef Signal2<void(const uint32,const real)> GraphicsUpdateSignal;
	GraphicsUpdateSignal	graphicsUpdateSig_;
	void subscribeToGraphicsUpdate(const GraphicsUpdateSignal::slot_type& slot)
	{
		graphicsUpdateSig_.connect(slot);
	}
	void _triggerGraphicsUpdate(const uint32 si, const real dt)
	{
		graphicsUpdateSig_(si,dt);
	}
	void _createWheelVisual(const String& wheelId, graphics::ISceneNode& parentSN)
	{
		// scene node + wheel mesh
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode();
		graphics::IEntity* pE = getGraphicalWorld()->createEntity("wheel1.mesh");
		pE->setCastsShadow( true );
		pSN->attachEntity( pE );
		model::ModelMovableLink* pLink = mComplex->createLink( mVehicle->getWheelInterface(wheelId), pSN );
		this->subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );

		// scene node + particle system for smoke...
		pSN = getGraphicalWorld()->createSceneNode();
		graphics::IParticleSystem* pPS = getGraphicalWorld()->createParticleSystem("SkidSmoke");
		mWheelPs[wheelId] = pPS;
		mEmitterEmissionRate[wheelId] = pPS->getEmissionRate(0);
		pSN->attachParticleSystem( pPS );
		pLink = new model::ModelMovableDirectLink();
		pLink->setSource( mVehicle->getWheelInterface(wheelId) );
		pLink->subscribeToPositionChanged( pSN ); //position only!
		mComplex->addLink( pLink );
		this->subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );
	}
protected:
	virtual void onCreateScene()
	{
		YAKE_LOG_INFORMATION("Creating scene");

		// create a light
		graphics::ILight* pLight = getGraphicalWorld()->createLight();
		pLight->setType( graphics::ILight::LT_DIRECTIONAL );
		pLight->setDirection( Vector3(0,-1,1) );
		getGraphicalWorld()->createSceneNode("lightnode0")->attachLight( pLight );

		getGraphicalWorld()->setShadowsEnabled( true );

		// position camera and look at the ninja
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

		// create vehicle container (e.g. for graphical objects and links)
		mComplex = new model::Model();
		model::Graphical* pG = new model::Graphical();
		mComplex->addComponent( pG, "g" );
		model::Physical* pP = new model::Physical();
		mComplex->addComponent( pP, "p" );

		// materials @todo read from .physics:
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.01f, 0.01f ), "chassis" );
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.2f, 0.01f ), "chassisTop" );
		getPhysicalWorld()->createMaterial( physics::IMaterial::Desc( 0.01f, 0.25f, 0.01f ), "wheel" );

		// vehicle
		SharedPtr<vehicle::IVehicleSystem> pVS = //create<vehicle::IVehicleSystem>("generic");
			SharedPtr<vehicle::IVehicleSystem>(new vehicle::GenericVehicleSystem());

		pVS->loadTemplates("../../media/vehicles/delorean.xml");

		// It's possible retrieve the template and make further adjustments to it before
		// instantiating vehicles.
		// e.g.: vehicle::VehicleTemplate* tpl = pVS->getTemplate("delorean");

		// instantiate
		mVehicle = pVS->create("delorean", *getPhysicalWorld(), *pP );

		//mVehicle->enableDebugGeometry( *getGraphicalWorld() );

		// create visuals
		// - ship body
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode("root");
		graphics::IEntity* pE = getGraphicalWorld()->createEntity("delorean.mesh");
		pE->setCastsShadow( true );
		pSN->attachEntity( pE );
		//pSN->setScale( Vector3::kUnitScale * razorMeshScale );
		pG->addSceneNode(pSN, "delorean_root");

		// - wheel visuals
		if (mVehicle->getWheelInterface("leftFrontWheel"))
		{
			_createWheelVisual( "leftFrontWheel", *pSN );
			_createWheelVisual( "rightFrontWheel", *pSN );
			_createWheelVisual( "leftRearWheel", *pSN );
			_createWheelVisual( "rightRearWheel", *pSN );
		}

		// create visual <-> physics links
		model::ModelMovableLink* pLink = mComplex->createLink( mVehicle->getChassisMovable(), pSN );
		this->subscribeToGraphicsUpdate( Bind2(&model::ModelMovableLink::update,pLink) );

		// sky box
		getGraphicalWorld()->setSkyBox("Examples/SpaceSkyBox");
	}
	virtual void onDestroyScene()
	{
		YAKE_SAFE_DELETE( mGround );

		YAKE_SAFE_DELETE( mComplex );
		YAKE_SAFE_DELETE( mVehicle );
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
		//mComplex->updatePhysics( timeElapsed );

		ConstDequeIterator< WheelPsMap > itWheel( mWheelPs );
		while (itWheel.hasMoreElements())
		{
			const String wheelId = itWheel.getNext().first;
			mWheelPs[ wheelId ]->setEmissionRate( 0, mEmitterEmissionRate[wheelId] * mVehicle->getWheelInterface( wheelId )->getSkid() );
		}

		getDefaultCamera()->lookAt(mVehicle->getChassisPosition());
		//mComplex->updateGraphics( timeElapsed );

		this->_triggerGraphicsUpdate(0,timeElapsed);

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
	model::Model*			mComplex;
	typedef AssocVector<String,real> EmitterRealMap;
	EmitterRealMap			mEmitterMinVel;
	EmitterRealMap			mEmitterMaxVel;
	EmitterRealMap			mEmitterEmissionRate;
	typedef AssocVector<String,graphics::IParticleSystem*> WheelPsMap;
	WheelPsMap				mWheelPs;
	input::ActionMap		mActionMap;

	typedef std::set<input::ActionId> ActionIdList;
	ActionIdList			mActiveActions;
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

