#include <yapp/samples/misc/cmdrmayhem/yakePCH.h>
#include <yake/audio/yakeAudio.h>
#include <yapp/raf/yakeRaf.h>
#include <yapp/model/yakeModelMovableLink.h>
#include <yake/input/yakeInput.h>
#include <yapp/loader/yakeXODEParser.h>
#include <yapp/model/yakePhysicalDataImporter.h>
#include <yapp/loader/yakeDotLinkLoader.h>
#include <yapp/common/yakeCameraControllers.h>

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

	virtual bool loadCEGUI()
	{ return true; }
};


class CharacterController
{
};


struct Env
{
	SharedPtr<model::complex::Model> model_;
};
void loadEnvironment(Env& env, graphics::IWorld* pGWorld, physics::IWorld* pPWorld, const String& dotSceneFn, const String& dotXodeFn, const String& dotLinkFn)
{
	YAKE_ASSERT( pGWorld );
	YAKE_ASSERT( pPWorld );

	// Loading graphical part
	model::Graphical* pG = new model::Graphical();
	pG->fromDotScene( dotSceneFn, pGWorld );

	// Loading physical part
	model::Physical* pP = new model::Physical();

	yake::data::dom::xml::XmlSerializer ser;
	ser.parse( dotXodeFn, false );
	YAKE_ASSERT( ser.getDocumentNode() )( dotXodeFn ).error("Could not load XODE document!");

	data::parser::xode::XODEParserV1 parser;

	using namespace yake::model;
	model::XODEListener listener( *pP, pPWorld );

	parser.subscribeToBodySignal( Bind1( &XODEListener::processBody, &listener ) );
	parser.subscribeToGeomSignal( Bind1( &XODEListener::processGeom, &listener ) );

	parser.load( ser.getDocumentNode() );

	// Bringing it all together!
	env.model_.reset( new model::complex::Model() );

	env.model_->addGraphical( pG, "baseGraphical" );
	env.model_->addPhysical( pP, "basePhysical" );

	model::DotLinkLoader dotLinkLoader;
	dotLinkLoader.load( dotLinkFn, *env.model_.get() );
}

/** Main application state */
class TheMainState : public raf::RtMainState
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

		// create ground
		mGround = new model::complex::Model();
		{
			const real groundHeight = -2;
			// visual
			graphics::ISceneNode* pGroundSN = getGraphicalWorld()->createSceneNode();
			graphics::IEntity* pGroundE = getGraphicalWorld()->createEntity("plane_1x1.mesh");
			pGroundE->setMaterial("box");
			pGroundE->setCastsShadow( false );
			pGroundSN->attachEntity( pGroundE );
			pGroundSN->setScale( Vector3(100,1,100) );
			pGroundSN->setPosition( Vector3(0,groundHeight,0) );

			model::Graphical* pG = new model::Graphical();
			pG->addSceneNode( pGroundSN );
			mGround->addGraphical( pG );

			// physical
			physics::IActorPtr pGroundPlane = getPhysicalWorld()->createActor( physics::ACTOR_STATIC );
			pGroundPlane->createShape( physics::IShape::PlaneDesc( Vector3(0,1,0), groundHeight ) );

			model::Physical* pP = new model::Physical();
			pP->addActor( pGroundPlane, "groundPlane" );
			mGround->addPhysical( pP );
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
						Vector3(0.5,2,0), // dimensions
						Vector3::kZero // initial position
					) 
			);
		YAKE_ASSERT( mPlayerRep );
		{
			mComplex = new model::complex::Model();
			model::Graphical* pG = new model::Graphical();
			mComplex->addGraphical( pG );
			graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode();
			pG->addSceneNode( pSN );
			graphics::IEntity* pE = getGraphicalWorld()->createEntity("sphere_d1.mesh");
			pSN->attachEntity( pE );
			pSN->setScale( 0.5 * Vector3::kUnitScale );

			model::ModelMovableLink* pLink = new model::ModelMovableLink();
			mComplex->addGraphicsController( pLink );
			pLink->setSource( mPlayerRep );
			pLink->subscribeToPositionChanged( pSN );
			pLink->subscribeToOrientationChanged( pSN );
		}

		// top-down camera controller
		mTopDownCtrlr.setOffset( Vector3(10,10,-10) );
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

		Vector3 projViewDir = mTopDownCtrlr.getOffset();
		projViewDir.y = 0.;
		if (projViewDir.length() <= 0.01)
			projViewDir = Vector3::kUnitZ;
		projViewDir.normalise();
		Quaternion projViewRot;
		projViewRot.FromAxes( projViewDir.crossProduct( Vector3::kUnitY ), Vector3::kUnitY, -projViewDir );

		const real maxVel = real(3.0);
		Vector3 playerTargetVel;
		ConstDequeIterator< ActionIdList > itAction( mActiveActions );
		while (itAction.hasMoreElements())
		{
			const input::ActionId activeId = itAction.getNext();
			if (activeId == input::ACTIONID_FORWARD)
				playerTargetVel += Vector3(0,0,+1);
			else if (activeId == input::ACTIONID_REVERSE)
				playerTargetVel += Vector3(0,0,-1);
			else if (activeId == input::ACTIONID_LEFT)
				playerTargetVel += Vector3(+1,0,0);
			else if (activeId == input::ACTIONID_RIGHT)
				playerTargetVel += Vector3(-1,0,0);
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
			mComplex->updatePhysics( timeElapsed );
			mComplex->updateGraphics( timeElapsed );
		}

		{ // top-down controller
			const real dist = timeElapsed * 4.;
			Vector3 offset = mTopDownCtrlr.getOffset();
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_I))
				offset += dist * Vector3::kUnitZ;
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_K))
				offset += - dist * Vector3::kUnitZ;
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_J))
				offset += dist * Vector3::kUnitX;
			if (this->getApp().getKeyboard()->isKeyDown(input::KC_L))
				offset += - dist * Vector3::kUnitX;
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
	model::complex::Model*	mGround;
	model::complex::Model*	mComplex;
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

