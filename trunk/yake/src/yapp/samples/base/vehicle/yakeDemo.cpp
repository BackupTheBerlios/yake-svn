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
	{ return MakeStringVector() << "graphicsOgre" << "inputOgre" << "physicsNx"; }

	//virtual StringVector getScriptingSystems()
	//{ return MakeStringVector() << "lua"; }

	virtual StringVector getGraphicsSystems()
	{ return MakeStringVector() << "ogre3d"; }

	virtual StringVector getInputSystems()
	{ return MakeStringVector() << "ogre"; }

	virtual StringVector getPhysicsSystems()
	{ return MakeStringVector() << "nx"; }
};

/** Main application state */
class TheMainState : public raf::RtMainState
{
public:
	TheMainState(raf::Application& owner) :
		raf::RtMainState(owner),
		mVehicle(0),
		mComplex(0)
	{}
	~TheMainState()
	{
	}
private:
	void _createThrusterVisual(size_t mtPtIdx, graphics::ISceneNode& parentSN)
	{
		graphics::ISceneNode* pSN = parentSN.createChildNode();
		graphics::IParticleSystem* pPS = getGraphicalWorld()->createParticleSystem("thruster1");
		pSN->attachParticleSystem( pPS );
		mComplex->addLink( mVehicle->getMountPoint(mtPtIdx), pSN );
		_regThrusterPs( mtPtIdx, *pPS );
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
		getDefaultCamera()->setFixedYawAxis(Vector3::kUnitY);
		getDefaultCamera()->setPosition(Vector3(100,20,-400));
		getDefaultCamera()->lookAt(Vector3(0,100,0));

		// vehicle
		SharedPtr<vehicle::IVehicleSystem> pVS = //create<vehicle::IVehicleSystem>("ode");
			SharedPtr<vehicle::IVehicleSystem>(new vehicle::OdeVehicleSystem());
		vehicle::VehicleTemplate tpl;
		tpl.mChassis.mChassisShapes.push_back(
			new physics::IShape::BoxDesc(Vector3(1,1,2)) );
		// mount point 0 for right thruster
		tpl.mMountPoints.push_back(
			vehicle::VehicleTemplate::MountPointTpl( Vector3(80,0,0), Vector3(1,0,0) ) );
		// mount point 1 for left thruster
		tpl.mMountPoints.push_back(
			vehicle::VehicleTemplate::MountPointTpl( Vector3(-80,0,0), Vector3(-1,0,0) ) );
		// mount point 2 for rear thruster
		tpl.mMountPoints.push_back(
			vehicle::VehicleTemplate::MountPointTpl( Vector3(0,0,-80), Vector3(0,0,-1) ) );
		// mount point 3 for front thruster
		tpl.mMountPoints.push_back(
			vehicle::VehicleTemplate::MountPointTpl( Vector3(0,0,70), Vector3(0,0,1) ) );
		// upward mount point - at center of vehicle
		// thrusters
		tpl.mEngines.push_back( new vehicle::VehicleTemplate::ThrusterTpl(0.,10.,0) );
		tpl.mEngines.push_back( new vehicle::VehicleTemplate::ThrusterTpl(0.,10.,1) );
		tpl.mEngines.push_back( new vehicle::VehicleTemplate::ThrusterTpl(0.,20.,2) );
		tpl.mEngines.push_back( new vehicle::VehicleTemplate::ThrusterTpl(0.,20.,3) );
		mVehicle = pVS->create( tpl, *getPhysicalWorld() );

		//pV->getChassisPosition();

		// create container (e.g. for graphical objects and links)
		mComplex = new model::complex::Model();
		model::Graphical* pG = new model::Graphical();
		mComplex->addGraphical( SharedPtr<model::Graphical>(pG) );

		// create visuals
		// - ship body
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode("root");
		pSN->attachEntity( getGraphicalWorld()->createEntity("razor.mesh") );
		pG->addSceneNode(pSN);

		// - thruster visuals
		_createThrusterVisual( 0, *pSN );
		_createThrusterVisual( 1, *pSN );
		_createThrusterVisual( 2, *pSN );
		_createThrusterVisual( 3, *pSN );

		// create visual <-> physics links
		mComplex->addLink( mVehicle->getChassisMovable(), pSN );

		// sky box
		getGraphicalWorld()->setSkyBox("Examples/SpaceSkyBox");
	}
	virtual void onDestroyScene()
	{
		YAKE_SAFE_DELETE( mComplex );
		YAKE_SAFE_DELETE( mVehicle );
	}
	virtual void onEnter()
	{
		RtMainState::onEnter();
		using namespace input;
		mActionMap.reg( ACTIONID_STRAFE_LEFT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_LEFT, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_STRAFE_LEFT, boost::bind(TheMainState::onStrafeLeft,this) );

		mActionMap.reg( ACTIONID_STRAFE_RIGHT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_RIGHT, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_STRAFE_RIGHT, boost::bind(TheMainState::onStrafeRight,this) );

		mActionMap.reg( ACTIONID_FORWARD,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_UP, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_FORWARD, boost::bind(TheMainState::onForward,this) );

		mActionMap.reg( ACTIONID_REVERSE,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_DOWN, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_REVERSE, boost::bind(TheMainState::onReverse,this) );
	}
	virtual void onExit()
	{
	}
	virtual void onFrame(const real timeElapsed)
	{
		getApp().getInputSystem()->update();
		getApp().getKeyboardEventGenerator()->update();
		getApp().getMouseEventGenerator()->update();
		mActionMap.update();

		for (size_t i=0; i<4; ++i)
		{
			mVehicle->getEngineInterface(i)->setThrottle(
				mVehicle->getEngineInterface(i)->getThrottle() - timeElapsed * 1.7 );
		}
		ConstDequeIterator< ActionIdList > itAction( mActiveActions );
		while (itAction.hasMoreElements())
		{
			const input::ActionId activeId = itAction.getNext();
			if (activeId == input::ACTIONID_STRAFE_LEFT)
				mVehicle->getEngineInterface(1)->setThrottle(1.);
			else if (activeId == input::ACTIONID_STRAFE_RIGHT)
				mVehicle->getEngineInterface(0)->setThrottle(1.);
			else if (activeId == input::ACTIONID_FORWARD)
				mVehicle->getEngineInterface(2)->setThrottle(1.);
			else if (activeId == input::ACTIONID_REVERSE)
				mVehicle->getEngineInterface(3)->setThrottle(1.);
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
private:
	void _regThrusterPs(size_t idx, graphics::IParticleSystem& ps)
	{
		mEmitterMinVel[ idx ] = ps.getMinVelocity(0);
		mEmitterMaxVel[ idx ] = ps.getMaxVelocity(0);
		mEmitterEmissionRate[ idx ] = ps.getEmissionRate(0);
		mThrusterPs[ idx ] = &ps;
	}
	void _updateThrusterPs()
	{
		ConstDequeIterator< ThrusterPsMap > itM( mThrusterPs );
		while (itM.hasMoreElements())
		{
			std::pair<size_t,graphics::IParticleSystem*> p = itM.getNext();
			size_t idxMtPt = p.first;
			graphics::IParticleSystem* ps = p.second;
			const real throttle = mVehicle->getEngineInterface(idxMtPt)->getThrottle();

			real rate = throttle * mEmitterEmissionRate[ idxMtPt ];
			ps->setEmissionRate( 0, rate );

			real vel = throttle * mEmitterMaxVel[ idxMtPt ];
			ps->setMaxVelocity( 0, vel );

			vel = throttle * mEmitterMinVel[ idxMtPt ];
			ps->setMinVelocity( 0, vel );
		}
	}
private:
	vehicle::IVehicle*		mVehicle;
	model::complex::Model*	mComplex;
	typedef AssocVector<size_t,real> EmitterRealMap;
	EmitterRealMap			mEmitterMinVel;
	EmitterRealMap			mEmitterMaxVel;
	EmitterRealMap			mEmitterEmissionRate;
	typedef AssocVector<size_t,graphics::IParticleSystem*> ThrusterPsMap;
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
	return (raf::runApplication( TheApp() )) ? 0 : 1;
}

