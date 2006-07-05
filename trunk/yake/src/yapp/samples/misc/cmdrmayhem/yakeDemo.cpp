#include <yapp/samples/misc/cmdrmayhem/yakePCH.h>
#include <yake/audio/yakeAudio.h>
#include <yapp/raf/yakeRaf.h>
#include <yake/model/model.h>
#include <yake/input/yakeInput.h>
#include <yapp/common/yakeCameraControllers.h>

using namespace yake;

/** Configuration of the application */
struct TheConfiguration : public raf::ApplicationConfiguration
{
	virtual StringVector getLibraries()
	{ return MakeStringVector() << "graphicsOgre" << "inputOgre" << "physicsODE"; }

	//virtual StringVector getScriptingSystems()
	//{ return MakeStringVector() << "lua"; }

	virtual StringVector getGraphicsSystems()
	{ return MakeStringVector() << "ogre3d"; }

	virtual StringVector getInputSystems()
	{ return MakeStringVector() << "ogre"; }

	virtual StringVector getPhysicsSystems()
	{ return MakeStringVector() << "ode"; }

	virtual bool loadCEGUI()
	{ return true; }
};


class CharacterController
{
};


struct Env
{
	SharedPtr<model::Model> model_;
};
void loadEnvironment(Env& env, graphics::IWorld* pGWorld, physics::IWorld* pPWorld, const String& dotSceneFn, const String& dotXodeFn, const String& dotLinkFn)
{
	YAKE_ASSERT( pGWorld );
	YAKE_ASSERT( pPWorld );

	YAKE_ASSERT( 0 );
}

/** Main application state */
class TheMainState : public raf::RtMainState, public yake::model::CentralControllerBase
{
public:
	TheMainState(raf::Application& owner) :
		raf::RtMainState(owner),
		mGround(0),
		mComplex(0),
		mPlayerRep(0),
		mEnv(0)
	{
		enableInstantQuitByKey( input::KC_ESCAPE );
	}
	~TheMainState()
	{
	}
private:
protected:
	virtual void onCreateScene()
	{
		YAKE_LOG_INFORMATION("Creating scene");

		getPhysicalWorld()->setGlobalGravity(Vector3(0,-2,0));

		// create a light
		graphics::ILight* pLight = getGraphicalWorld()->createLight();
		pLight->setType( graphics::ILight::LT_DIRECTIONAL );
		pLight->setDirection( math::Vector3(0,-1,1) );
		getGraphicalWorld()->createSceneNode("lightnode0")->attachLight( pLight );

		getGraphicalWorld()->setShadowsEnabled( true );

		// position camera and look at the ninja
		getDefaultCamera()->setNearClipDistance( 1 );
		getDefaultCamera()->setFixedYawAxis(math::Vector3::kUnitY);
		getDefaultCamera()->setPosition(math::Vector3(7,4,-7));

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
			pGroundSN->setScale( math::Vector3(100,1,100) );
			pGroundSN->setPosition( math::Vector3(0,groundHeight,0) );

			model::Graphical* pG = new model::Graphical();
			pG->addSceneNode( pGroundSN, "ground", true );
			mGround->addComponent( pG, "p" );

			// physical
			physics::IActorPtr pGroundPlane = getPhysicalWorld()->createActor( physics::ACTOR_STATIC );
			pGroundPlane->createShape( physics::IShape::PlaneDesc( math::Vector3(0,1,0), groundHeight ) );

			model::Physical* pP = new model::Physical();
			pP->addActor( pGroundPlane, "groundPlane" );
			mGround->addComponent( pP, "g" );
		}

		// sky box
		getGraphicalWorld()->setSkyBox("Examples/SpaceSkyBox");

		// graphics / physics environment
		mEnv = new Env();
		//const String dirMediaRoot = "../../media/samples/dotLink/DynTest/";
		//const String sceneBaseName = "DotScene1";
		//loadEnvironment( *mEnv, getGraphicalWorld(), getPhysicalWorld(),
		//	dirMediaRoot+sceneBaseName+".scene", dirMediaRoot+sceneBaseName+".xode", dirMediaRoot+sceneBaseName+".link" );

		// create player/avatar
		mPlayerRep = getPhysicalWorld()->createAvatar(
				physics::IAvatar::Desc(
						math::Vector3(0.5,2,0), // dimensions
						math::Vector3::kZero // initial position
					) 
			);
		YAKE_ASSERT( mPlayerRep );
		{
			mComplex = new model::Model();
			model::Graphical* pG = new model::Graphical();
			mComplex->addComponent( pG );
			graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode();
			pG->addSceneNode( pSN, "playerSN" );
			graphics::IEntity* pE = getGraphicalWorld()->createEntity("sphere_d1.mesh");
			pSN->attachEntity( pE );
			pSN->setScale( 0.5 * math::Vector3::kUnitScale );

			model::ModelMovableDirectLink* pLink = new model::ModelMovableDirectLink();
			mComplex->addLink( pLink );
			pLink->setSource( mPlayerRep );
			pLink->subscribeToPositionChanged( pSN );
			pLink->subscribeToOrientationChanged( pSN );
			this->subscribeToGraphicsUpdate( Bind2(&model::ModelMovableDirectLink::update,pLink) );
		}

		// top-down camera controller
		mTopDownCtrlr.setOffset( math::Vector3(10,10,-10) );
		mTopDownCtrlr.setCamera( getDefaultCamera() );
	}
	virtual void onDestroyScene()
	{
		mTopDownCtrlr.setCamera( 0 );
		YAKE_SAFE_DELETE( mComplex );
		YAKE_SAFE_DELETE( mPlayerRep );
		YAKE_SAFE_DELETE( mGround );
		YAKE_SAFE_DELETE( mEnv );
	}
	virtual void onEnter()
	{
		RtMainState::onEnter();
		using namespace input;

		// controls

		mActionMap.reg( ACTIONID_LEFT, new input::KeyboardActionCondition( getApp().getKeyboard(), KC_A, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_LEFT, boost::bind(&TheMainState::onLeft,this) );

		mActionMap.reg( ACTIONID_RIGHT, new input::KeyboardActionCondition( getApp().getKeyboard(), KC_D, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_RIGHT, boost::bind(&TheMainState::onRight,this) );

		mActionMap.reg( ACTIONID_FORWARD, new input::KeyboardActionCondition( getApp().getKeyboard(), KC_W, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_FORWARD, boost::bind(&TheMainState::onForward,this) );

		mActionMap.reg( ACTIONID_REVERSE, new input::KeyboardActionCondition( getApp().getKeyboard(), KC_S, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_REVERSE, boost::bind(&TheMainState::onReverse,this) );

		mActionMap.reg( ACTIONID_UP, new input::KeyboardActionCondition( getApp().getKeyboard(), KC_SPACE, KAM_PRESSED ) );
		mActionMap.subscribeToActionId( ACTIONID_UP, boost::bind(&TheMainState::onUp,this) );

		mActionMap.reg( ACTIONID_DOWN, new input::KeyboardActionCondition( getApp().getKeyboard(), KC_C, KAM_PRESSED ) );
		mActionMap.subscribeToActionId( ACTIONID_DOWN, boost::bind(&TheMainState::onCrouch,this) );

		// for cursor
		getApp().enableMouseInputForCEGUI(true);
	}
	virtual void onExit()
	{
		getApp().enableMouseInputForCEGUI(false);
	}
	virtual void onFrame(const real timeElapsed)
	{
		mActionMap.update();

		math::Vector3 projViewDir = mTopDownCtrlr.getOffset();
		projViewDir.y = 0.;
		if (projViewDir.length() <= 0.01)
			projViewDir = math::Vector3::kUnitZ;
		projViewDir.normalise();
		math::Quaternion projViewRot;
		projViewRot.FromAxes( projViewDir.crossProduct( math::Vector3::kUnitY ), math::Vector3::kUnitY, -projViewDir );

		const real maxVel = real(3.0);
		math::Vector3 playerTargetVel;
		ConstDequeIterator< ActionIdList > itAction( mActiveActions );
		while (itAction.hasMoreElements())
		{
			const input::ActionId activeId = itAction.getNext();
			if (activeId == input::ACTIONID_FORWARD)
				playerTargetVel += math::Vector3(0,0,+1);
			else if (activeId == input::ACTIONID_REVERSE)
				playerTargetVel += math::Vector3(0,0,-1);
			else if (activeId == input::ACTIONID_LEFT)
				playerTargetVel += math::Vector3(+1,0,0);
			else if (activeId == input::ACTIONID_RIGHT)
				playerTargetVel += math::Vector3(-1,0,0);
			else if (activeId == input::ACTIONID_UP)
				mPlayerRep->jump();
			else if (activeId == input::ACTIONID_DOWN)
			{
				mPlayerRep->duck( !mPlayerRep->isDucking() );
			}
		}
		playerTargetVel = projViewRot * playerTargetVel;
		mPlayerRep->setTargetVelocity( playerTargetVel.normalisedCopy() * maxVel );

		if (mComplex)
		{
			this->triggerGraphicsUpdateSignal(0,timeElapsed);
			this->triggerPhysicsUpdateSignal(0,timeElapsed);
		}

		{ // top-down controller
			const real dist = timeElapsed * 4.;
			math::Vector3 offset = mTopDownCtrlr.getOffset();
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_I))
				offset += dist * math::Vector3::kUnitZ;
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_K))
				offset += - dist * math::Vector3::kUnitZ;
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_J))
				offset += dist * math::Vector3::kUnitX;
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_L))
				offset += - dist * math::Vector3::kUnitX;
			mTopDownCtrlr.setOffset( offset );
		}

		mTopDownCtrlr.setTarget( mPlayerRep->getPosition() );
		mTopDownCtrlr.update( timeElapsed );

		// prepare for next step
		mActiveActions.clear();
	}
protected:
	void onForward()
	{ mActiveActions.insert( input::ACTIONID_FORWARD ); }
	void onReverse()
	{ mActiveActions.insert( input::ACTIONID_REVERSE ); }
	void onUp()
	{ mActiveActions.insert( input::ACTIONID_UP ); }
	void onLeft()
	{ mActiveActions.insert( input::ACTIONID_LEFT ); }
	void onRight()
	{ mActiveActions.insert( input::ACTIONID_RIGHT ); }
	void onCrouch()
	{ mActiveActions.insert( input::ACTIONID_DOWN ); }
private:
	physics::IAvatarPtr		mPlayerRep;
	model::Model*	mGround;
	model::Model*	mComplex;
	input::ActionMap		mActionMap;

	typedef std::set<input::ActionId> ActionIdList;
	ActionIdList			mActiveActions;

	Env*					mEnv;

	TopDownCameraController	mTopDownCtrlr;
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

