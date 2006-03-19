#include <yapp/samples/base/vehicle/yakePCH.h>
#include <yake/audio/yakeAudio.h>
#include <yapp/raf/yakeRaf.h>
#include <yapp/model/yakeModelMovableLink.h>
#include <yapp/vehicle/yakeVehicle.h>
#include <yake/input/yakeInput.h>

#include <yapp/vehicle/yakeNativeOde.h>

#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>

#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/PhysicsSystemODE.h>

#include <yapp/plugins/terrainPhysMgr/TerrainOdeActor.h>
#include <yapp/plugins/terrainPhysMgr/TerrainPhysicsManager.h>

#include <OgrePagingLandScapeData2DManager.h>

using namespace yake;

/** Configuration of the application */
struct TheConfiguration : public raf::ApplicationConfiguration
{
	virtual StringVector getLibraries()
	{ return MakeStringVector() << "graphicsOgre" << "inputOgre" << "physicsODE"; }

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
		raf::RtMainState(owner)
		{
			enableInstantQuitByKey( input::KC_ESCAPE );
		}

		~TheMainState()
		{}
		
		void setupTerrain()
		{
			YAKE_LOG( "INITIALIZING TERRAIN");

			Ogre::PagingLandScapeSceneManager * sceneMgr = (Ogre::PagingLandScapeSceneManager *) getSceneMgr();
			
			sceneMgr->setWorldGeometry("paginglandscape2.cfg"); 
						
			pG = new model::Graphical();
			YAKE_ASSERT( pG);
			
			pPh = new model::Physical();
			YAKE_ASSERT( pPh);

  			sceneMgr->setOption( "LoadNow", NULL); 
// 			sceneMgr->InitScene();
// 			sceneMgr->PagingLandScapeOctreeResize();
			
			physics::TerrainPhysicsManager::instance().init( sceneMgr, getPhysicalWorld() );
		}

		void onMouseMoved( const math::Vector3& rDelta )
		{
			getDefaultCamera()->yaw( -(rDelta.x)*0.13 );
			getDefaultCamera()->pitch( -(rDelta.y)*0.13 );
		}

protected:
	
	void createDebugBox( model::complex::Model * model, model::Graphical* pG, model::Physical* pP, math::Vector3  position, math::Vector3  dims)
	{
		static int ID = 0; ++ID;
		
		/// visual scenenode & entity creation
		graphics::IEntity* pBoxE = getGraphicalWorld()->createEntity("box_1x1x1.mesh");
		pBoxE->setMaterial("box");
				
		graphics::ISceneNode* pBoxSN = getGraphicalWorld()->createSceneNode();
		YAKE_ASSERT( pBoxSN);
		
		pBoxSN->attachEntity( pBoxE );
		pBoxSN->setScale( dims);
		pBoxSN->setPosition( position );
				
		/// Graphical model
		pG->addSceneNode( pBoxSN );
			
		physics::IActor::Desc debugActorDesc( physics::ACTOR_MOVABLE );
		physics::IActorPtr pBoxActor = getPhysicalWorld()->createActor( debugActorDesc);
		
		pBoxActor->createShape( physics::IShape::BoxDesc( dims));
				
		pBoxActor->setPosition( position) ;
		
// 		pBoxActor->getBody().setMass( 0.1);
	
		pP->addActor( pBoxActor, String("debugBox" ) << ID);
				
		/// Physical model
		physics::IShapePtrList shapes = pBoxActor->getShapes();
		ConstDequeIterator<physics::IShapePtrList> itShape( shapes );
		const physics::IShapePtr pShape = itShape.getNext();
		
 		model::ModelMovableLink* pLink = new model::ModelMovableLink();
				
 		pLink->setSource( pShape );
 		pLink->subscribeToPositionChanged( pBoxSN );
 		pLink->subscribeToOrientationChanged( pBoxSN );
				
 		model->addGraphical( pG);
		model->addPhysical( pP );
		
 		model->addLink( pBoxActor, pBoxSN);
		
		physics::TerrainPhysicsManager::instance().placeActorOnTerrain( position, pBoxActor);
	}
	
	virtual void onCreateScene()
	{
		YAKE_LOG_INFORMATION("Creating scene");

		// create a light
		graphics::ILight* pLight = getGraphicalWorld()->createLight();
		pLight->setType( graphics::ILight::LT_DIRECTIONAL );
		pLight->setDirection( math::Vector3(0,-1,1) );
		getGraphicalWorld()->createSceneNode("lightnode0")->attachLight( pLight );

		// position camera and look at the razor
		getDefaultCamera()->setNearClipDistance(1);
		getDefaultCamera()->setFarClipDistance(100000);
		
		getDefaultCamera()->setPosition(math::Vector3(150, 650, 150));
		getDefaultCamera()->lookAt( math::Vector3( 0, 0, 0));

		getPhysicalWorld()->setGlobalGravity( math::Vector3( 0, -20, 0 ) ); 

		// create ground
		mGround = new model::complex::Model();
		
		math::Vector3 dims( 100, 100, 100);
		int a = 1500, b = 600;
		
		for ( int i = 0; i < 15; i++)
			createDebugBox( mGround, pG, pPh, math::Vector3( -a + 210*i, b, -a + 210*i), dims);
	}

	Ogre::SceneManager * getSceneMgr()
	{
		graphics::ogre3d::GraphicsSystem* pSys = (graphics::ogre3d::GraphicsSystem *)getApp().getGraphicsSystem();
			
		graphics::ogre3d::OgreCore * ogreCore = pSys->getCore();
				
		return ogreCore->getSceneMgr();
	}
		
	virtual void onDestroyScene()
	{
		YAKE_SAFE_DELETE( mGround );
	}

	virtual void onEnter()
	{
		setupTerrain();
		
		RtMainState::onEnter();
		using namespace input;

		//camera controls
		mActionMap.reg( ACTIONID_A,
				new input::KeyboardActionCondition( getApp().getKeyboard(), KC_A, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_A, boost::bind(&TheMainState::onCameraLeft,this) );
		
		mActionMap.reg( ACTIONID_D,
				new input::KeyboardActionCondition( getApp().getKeyboard(), KC_D, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_D, boost::bind(&TheMainState::onCameraRight,this) );

		mActionMap.reg( ACTIONID_W,
				new input::KeyboardActionCondition( getApp().getKeyboard(), KC_W, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_W, boost::bind(&TheMainState::onCameraUp,this) );

		mActionMap.reg( ACTIONID_S,
				new input::KeyboardActionCondition( getApp().getKeyboard(), KC_S, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_S, boost::bind(&TheMainState::onCameraDown,this) );

		mActionMap.reg( ACTIONID_T,
				new input::KeyboardActionCondition( getApp().getKeyboard(), KC_T, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_T, boost::bind(&TheMainState::cycleCameraDetailWiews,this) );

		// front wheel controls

		mActionMap.reg( ACTIONID_LEFT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_COMMA, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_LEFT, boost::bind(&TheMainState::onFrontWheelLeft,this) );

		mActionMap.reg( ACTIONID_RIGHT,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_PERIOD, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_RIGHT, boost::bind(&TheMainState::onFrontWheelRight,this) );

		// thruster controls

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
		
		mActionMap.reg( ACTIONID_DOWN,
			new input::KeyboardActionCondition( getApp().getKeyboard(), KC_PGDOWN, KAM_CONTINUOUS ) );
		mActionMap.subscribeToActionId( ACTIONID_DOWN, boost::bind(&TheMainState::onDown,this) );
	
	}

	virtual void onExit()
	{}

	virtual void onFrame(const real timeElapsed)
	{
		mActionMap.update();

		ConstDequeIterator< ActionIdList > itAction( mActiveActions );
		
		while (itAction.hasMoreElements())
		{
 			const input::ActionId activeId = itAction.getNext();

			 if (activeId == input::ACTIONID_A)
			{
				getDefaultCamera()->translate(  math::Vector3( -2.5, 0,0));
			}
			else if (activeId == input::ACTIONID_D)
			{	
				getDefaultCamera()->translate(  math::Vector3( 2.5, 0,0));
			}
			else if (activeId == input::ACTIONID_S)
			{	
				math::Vector3 currentDir = getDefaultCamera()->getDirection();
				currentDir *= -15;
				
				getDefaultCamera()->translate(  currentDir);
			}
			else if (activeId == input::ACTIONID_W)
			{	
				math::Vector3 currentDir = getDefaultCamera()->getDirection();
				currentDir *= 15;
				
				getDefaultCamera()->translate(  currentDir);
			}
			else if( activeId == input::ACTIONID_UP)
			{
				math::Vector3 dims( 100, 100, 100);
				int a = 1500, b = 600;
		
				for ( int i = 0; i < 15; i++)
					createDebugBox( mGround, pG, pPh, math::Vector3( -a + 210*i, b, -a + 210*i), dims);
			}
		}
   		mGround->updatePhysics( timeElapsed);
 		mGround->updateGraphics( timeElapsed);
		
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
	
	void onDown()
	{ mActiveActions.insert( input::ACTIONID_DOWN);}
	
	void onFrontWheelLeft()
	{ mActiveActions.insert( input::ACTIONID_LEFT ); }
	
	void onFrontWheelRight()
	{ mActiveActions.insert( input::ACTIONID_RIGHT ); }
	
	void onCameraLeft()
	{ mActiveActions.insert( input::ACTIONID_A); }
	
	void onCameraRight()
	{ mActiveActions.insert( input::ACTIONID_D); }

	void onCameraUp()
	{ mActiveActions.insert( input::ACTIONID_W); }

	void onCameraDown()
	{ mActiveActions.insert( input::ACTIONID_S); }

	void cycleCameraDetailWiews()
	{
		char currentDetailLevel = getDefaultCamera()->getDetailLevel();
//  		currentDetailLevel++;
//  		if( currentDetailLevel > graphics::ICamera::SDL_SOLID)
			currentDetailLevel = graphics::SDL_WIREFRAME;
		
		getDefaultCamera()->setDetailLevel ( (graphics::SceneDetailLevel)currentDetailLevel);
	}

private:
	model::complex::Model*	mGround;
	
	input::ActionMap		mActionMap;

	typedef std::set<input::ActionId> ActionIdList;
	ActionIdList			mActiveActions;
	
	model::Graphical* pG;
	model::Physical* pPh;
};

/** The mighty application itself! */
class TheApp : public raf::ExampleApplication<TheConfiguration>
{
public:
	TheApp() 
	{
		mainState = new TheMainState( *this);
	}

	void onMouseMoved( const math::Vector3& rDelta )
	{
		mainState->onMouseMoved( rDelta);
	}

	bool onInitialise() 
	{ 
 		getMouseEventGenerator()->subscribeToMouseMoved( Bind1( &TheApp::onMouseMoved, this ) );
		return true; 
	}

protected:
	TheMainState * mainState;
		
	virtual raf::MainState* createMainState()
	{
		return mainState;
	}
};

int main( int argc, char** argv )
{
	// Use default executor for convenience.
	// It's always possible to manually execute TheApp::initialise() etc.
	TheApp theApp;
	
	return (raf::runApplication( theApp )) ? 0 : 1;
}

