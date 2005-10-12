#include <yapp/samples/base/vehicle/yakePCH.h>
#include <yake/audio/yakeAudio.h>
#include <yapp/raf/yakeRaf.h>
#include <yapp/model/yakeModelMovableLink.h>
#include <yapp/vehicle/yakeVehicle.h>
#include <yake/input/yakeInput.h>

// temporary stuff (until factory registration issues have been sorted out):
#include <yapp/vehicle/yakeNativeOde.h>

using namespace yake;

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

const real razorMeshScale = 0.1;

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
	void _createThrusterVisual(const String& mtPtId, const String& engineId, graphics::ISceneNode& parentSN)
	{
		graphics::ISceneNode* pSN = parentSN.createChildNode();
		graphics::IParticleSystem* pPS = getGraphicalWorld()->createParticleSystem("thruster2");
		pSN->attachParticleSystem( pPS );
		mComplex->addLink( mVehicle->getMountPoint(mtPtId), pSN );
		_regThrusterPs( engineId, *pPS );
	}
	void _createWheelVisual(const String& wheelId, graphics::ISceneNode& parentSN)
	{
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode();
		graphics::IEntity* pE = getGraphicalWorld()->createEntity("sphere_d1.mesh");
		pSN->attachEntity( pE );
		const Vector3 scale = Vector3::kUnitScale * mVehicle->getWheelInterface(wheelId)->getRadius();
		pSN->setScale( scale );
		mComplex->addLink( mVehicle->getWheelInterface(wheelId), pSN );
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

		// position camera and look at the ninja
		getDefaultCamera()->setNearClipDistance( 1 );
		getDefaultCamera()->setFixedYawAxis(Vector3::kUnitY);
		getDefaultCamera()->setPosition(Vector3(0,0,-40));

		// create ground
		mGround = new model::complex::Model();
		{
			// visual
			graphics::ISceneNode* pGroundSN = getGraphicalWorld()->createSceneNode();
			graphics::IEntity* pGroundE = getGraphicalWorld()->createEntity("plane_1x1.mesh");
			pGroundE->setMaterial("box");
			pGroundSN->attachEntity( pGroundE );
			pGroundSN->setScale( Vector3(200,1,200) );
			pGroundSN->setPosition( Vector3(0,-10,0) );

			model::Graphical* pG = new model::Graphical();
			pG->addSceneNode( pGroundSN );
			mGround->addGraphical( pG );

			// physical
			physics::IActorPtr pGroundPlane = getPhysicalWorld()->createActor( physics::ACTOR_STATIC );
			pGroundPlane->createShape( physics::IShape::PlaneDesc( Vector3(0,1,0), -10 ) );

			model::Physical* pP = new model::Physical();
			pP->addActor( pGroundPlane, "groundPlane" );
			mGround->addPhysical( pP );
		}

		// vehicle
		SharedPtr<vehicle::IVehicleSystem> pVS = //create<vehicle::IVehicleSystem>("generic");
			SharedPtr<vehicle::IVehicleSystem>(new vehicle::GenericVehicleSystem());

		pVS->loadTemplates("../../media/vehicles/jet.xml");

#if 0	// <= Set to 1 in order to create the jet vehicle in code, or
		//    set to 0 to load it from .vehicle file (see below).

		vehicle::VehicleTemplate tpl;
		tpl.mChassis.mChassisShapes.push_back(
			new physics::IShape::BoxDesc(Vector3(1,1,2)) );
		// mount points:
		// mount point for left thruster
		tpl.mMountPoints["left"] = 
			vehicle::VehicleTemplate::MountPointTpl( Vector3(7,0,0), Vector3(1,0,0) );
		// mount point for right thruster
		tpl.mMountPoints["right"] = 
			vehicle::VehicleTemplate::MountPointTpl( Vector3(-7,0,0), Vector3(-1,0,0) );
		// mount point for rear thruster
		tpl.mMountPoints["rear"] = 
			vehicle::VehicleTemplate::MountPointTpl( Vector3(0,0,-8), Vector3(0,0,-1) );
		// mount point for front thruster
		tpl.mMountPoints["front"] = 
			vehicle::VehicleTemplate::MountPointTpl( Vector3(0,0,7), Vector3(0,0,1) );
		// thrusters:
		tpl.mEngines["left"] = new vehicle::VehicleTemplate::ThrusterTpl(0.,10.,"left");
		tpl.mEngines["right"] = new vehicle::VehicleTemplate::ThrusterTpl(0.,10.,"right");
		tpl.mEngines["forward"] = new vehicle::VehicleTemplate::ThrusterTpl(0.,20.,"rear");
		tpl.mEngines["backward"] = new vehicle::VehicleTemplate::ThrusterTpl(0.,20.,"front");
		mVehicle = pVS->create( tpl, *getPhysicalWorld() );
#else
		// add wheels
		const real wheelRadius = 2;
		const real wheelRelativeMass = 0.1; // mass relative to chassis
		vehicle::VehicleTemplate* tpl = pVS->getTemplate("jet");
		YAKE_ASSERT( tpl );
		tpl->mWheels["frontWheel"] = 
			vehicle::VehicleTemplate::WheelTpl( Vector3(0,-3,5), wheelRadius, wheelRelativeMass, true );
		tpl->mWheels["leftRearWheel"] = 
			vehicle::VehicleTemplate::WheelTpl( Vector3(-3,-3,-3), wheelRadius, wheelRelativeMass, true );
		tpl->mWheels["rightRearWheel"] = 
			vehicle::VehicleTemplate::WheelTpl( Vector3(3,-3,-3), wheelRadius, wheelRelativeMass, true );

		// instantiate
		mVehicle = pVS->create("jet", *getPhysicalWorld() );
#endif

		// create container (e.g. for graphical objects and links)
		mComplex = new model::complex::Model();
		model::Graphical* pG = new model::Graphical();
		mComplex->addGraphical( SharedPtr<model::Graphical>(pG) );

		// create visuals
		// - ship body
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode("root");
		pSN->attachEntity( getGraphicalWorld()->createEntity("razor.mesh") );
		pSN->setScale( Vector3::kUnitScale * razorMeshScale );
		pG->addSceneNode(pSN);

		// - thruster visuals
		_createThrusterVisual( "left", "left", *pSN );
		_createThrusterVisual( "right", "right", *pSN );
		_createThrusterVisual( "front", "backward", *pSN );
		_createThrusterVisual( "rear", "forward", *pSN );
		_createThrusterVisual( "bottom", "upward", *pSN );

		// - wheel visuals
		if (mVehicle->getWheelInterface("frontWheel"))
		{
			_createWheelVisual( "frontWheel", *pSN );
			_createWheelVisual( "leftRearWheel", *pSN );
			_createWheelVisual( "rightRearWheel", *pSN );
		}

		// create visual <-> physics links
		mComplex->addLink( mVehicle->getChassisMovable(), pSN );

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
		mActionMap.reg( ACTIONID_STRAFE_LEFT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_LEFT, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_STRAFE_LEFT, boost::bind(&TheMainState::onStrafeLeft,this) );

		mActionMap.reg( ACTIONID_STRAFE_RIGHT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_RIGHT, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_STRAFE_RIGHT, boost::bind(&TheMainState::onStrafeRight,this) );

		mActionMap.reg( ACTIONID_FORWARD,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_UP, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_FORWARD, boost::bind(&TheMainState::onForward,this) );

		mActionMap.reg( ACTIONID_REVERSE,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_DOWN, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_REVERSE, boost::bind(&TheMainState::onReverse,this) );

		mActionMap.reg( ACTIONID_UP,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_PGUP, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_UP, boost::bind(&TheMainState::onUp,this) );
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
			pEngine->setThrottle( pEngine->getThrottle() - timeElapsed * 1.7 );
		}
		ConstDequeIterator< ActionIdList > itAction( mActiveActions );
		while (itAction.hasMoreElements())
		{
			const input::ActionId activeId = itAction.getNext();
			if (activeId == input::ACTIONID_STRAFE_LEFT)
				mVehicle->getEngineInterface("right")->setThrottle(1.);
			else if (activeId == input::ACTIONID_STRAFE_RIGHT)
				mVehicle->getEngineInterface("left")->setThrottle(1.);
			else if (activeId == input::ACTIONID_FORWARD)
				mVehicle->getEngineInterface("forward")->setThrottle(1.);
			else if (activeId == input::ACTIONID_REVERSE)
				mVehicle->getEngineInterface("backward")->setThrottle(1.);
			else if (activeId == input::ACTIONID_UP)
				mVehicle->getEngineInterface("upward")->setThrottle(1.);
		}

		mVehicle->updateSimulation( timeElapsed );
		mComplex->updatePhysics( timeElapsed );

		_updateThrusterPs();
		getDefaultCamera()->lookAt(mVehicle->getChassisPosition());
		mComplex->updateGraphics( timeElapsed );

		mActiveActions.clear();
	}
protected:
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
private:
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
			const real throttle = mVehicle->getEngineInterface(mtPtId)->getThrottle();

			real rate = throttle * mEmitterEmissionRate[ mtPtId ];
			ps->setEmissionRate( 0, rate );

			real vel = throttle * mEmitterMaxVel[ mtPtId ];
			ps->setMaxVelocity( 0, vel );

			vel = throttle * mEmitterMinVel[ mtPtId ];
			ps->setMinVelocity( 0, vel );
		}
	}
private:
	model::complex::Model*	mGround;
	vehicle::IVehicle*		mVehicle;
	model::complex::Model*	mComplex;
	typedef AssocVector<String,real> EmitterRealMap;
	EmitterRealMap			mEmitterMinVel;
	EmitterRealMap			mEmitterMaxVel;
	EmitterRealMap			mEmitterEmissionRate;
	typedef AssocVector<String,graphics::IParticleSystem*> ThrusterPsMap;
	ThrusterPsMap			mThrusterPs;
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

