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

#include <yake/base/templates/yakeObjectLink.h>
#include <yake/samples/data/demo/yakeDotScene.h> // temporarily
//#include <yake/samples/common/yakeXODE.h> // temporarily
#include <yake/samples/common/yakeModel.h> // temporarily
#include <yake/samples/graphics/Profiler.h>
#include <yake/samples/common/yakeComplexModel.h> // temporarily
#include <yake/samples/common/yakeVehicle.h> // temporarily
//#include <yake/samples/common/yakeModelLink.h> // temporarily

#pragma warning( disable: 4305 ) // truncation from 'double' to 'float'

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::graphics;
using namespace yake::data;

class CameraFollowController
{
private:
	Movable*		mCamMovable;
	Movable*		mTarget;
public:
	CameraFollowController( Movable* pCamMovable, Movable* pTarget ) :
	  mTarget(pTarget), mCamMovable(pCamMovable)
	 {}

	 void update( real timeElapsed )
	 {
		 YAKE_ASSERT( mCamMovable );
		 YAKE_ASSERT( mTarget );

		 Vector3 targetDir = mTarget->getOrientation() * Vector3::kUnitZ;
		 mCamMovable->setPosition( mTarget->getPosition() + Vector3(0,7,14) );
		 //mCamMovable->setOrientation( mTarget->getOrientation() );
	 }
};

model::MovableUpdateController* createMovableUpdateController(
			Movable* pSrc, Movable* pTgt, bool position, bool orientation )
{
	YAKE_ASSERT( pSrc );
	YAKE_ASSERT( pTgt );
	YAKE_ASSERT( position || orientation );
	model::MovableUpdateController* pMUC = new model::MovableUpdateController();
	YAKE_ASSERT( pMUC );
	pMUC->setUpdateSource( pSrc );
	if (position)
		pMUC->subscribeToPositionChanged( pTgt );
	if (orientation)
		pMUC->subscribeToOrientationChanged( pTgt );
	return pMUC;
}

typedef Vector<SharedPtr<graphics::ISceneNode> > SharedSceneNodeList;
typedef Vector<SharedPtr<model::MovableUpdateController> > SharedMUCList;
typedef Vector<physics::ICollisionGeometry*> CollisionGeomList;

void createDebugGeometryForComplexPhysical(
			const physics::IComplexObject* pComplex,
			graphics::IGraphicalWorld* pWorld,
			Vector<graphics::ISceneNode*> & nodes,
			SharedMUCList & ctrlrs)
{
	YAKE_ASSERT( pComplex );
	YAKE_ASSERT( pWorld );
	const CollisionGeomList geoms = pComplex->getCollisionGeometryVector();
	if (geoms.size() == 0)
		return;
	ConstVectorIterator< const CollisionGeomList > it( geoms.begin(), geoms.end() );
	while (it.hasMoreElements())
	{
		physics::ICollisionGeometry* pGeom = it.getNext();
		YAKE_ASSERT( pGeom );

		graphics::ISceneNode* pSN = pWorld->createSceneNode();
		YAKE_ASSERT( pSN );
		graphics::IEntity* pE = 0;

		switch (pGeom->getType())
		{
		case physics::ICollisionGeometry::CGT_BOX:
			{
			pE = pWorld->createEntity("box_1x1x1.mesh");
			pE->setCastsShadow( false );
			Vector3 dim = pGeom->boxGetDimensions();
			pSN->setScale( dim );
			}
			break;
		case physics::ICollisionGeometry::CGT_SPHERE:
			{
			pE = pWorld->createEntity("wheel1.mesh");
			//pE = pWorld->createEntity("sphere_d1.mesh");
			pE->setCastsShadow( false  );
			real scale = 4 /* x2 because diameter is 1, therefore radius is 0.5 by default*/ * pGeom->sphereGetRadius();
			if (scale < 0.0001)
				scale = 1.;
			pSN->setScale( Vector3(scale,scale,scale) );
			}
			break;
		default:
			break;
		}

		YAKE_ASSERT( pE );
		pSN->attachEntity( pE );
		nodes.push_back( pSN );

		model::MovableUpdateController* pMUC = new model::MovableUpdateController();
		YAKE_ASSERT( pMUC );
		pMUC->setUpdateSource( pGeom );
		pMUC->subscribeToPositionChanged( pSN );
		pMUC->subscribeToOrientationChanged( pSN );
		ctrlrs.push_back( SharedPtr<model::MovableUpdateController>(pMUC) );
	}
}

void createDebugGeometryForComplexPhysical(
			const physics::IComplexObject* pComplex,
			graphics::IGraphicalWorld* pWorld,
			SharedSceneNodeList & nodes,
			SharedMUCList & ctrlrs)
{
	Vector<graphics::ISceneNode*> newNodes;
	createDebugGeometryForComplexPhysical( pComplex, pWorld, newNodes, ctrlrs );
	VectorIterator< Vector<graphics::ISceneNode*> > it( newNodes.begin(), newNodes.end() );
	while (it.hasMoreElements())
	{
		graphics::ISceneNode* pSN = it.getNext();
		nodes.push_back( SharedPtr<graphics::ISceneNode>( pSN ) );
	}
}

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector<std::pair<IViewport*,ICamera*> >	mVPs;
	SharedPtr< IGraphicalWorld >			mGWorld;
	physics::IWorld*						mPWorld;
	audio::IWorld*							mpAWorld;

	physics::IComplexObject* dbgChassis;

	audio::IListener*					mpAListener;
	audio::ISource*						mpASource;
	audio::ISoundData*					mpAData;

	graphics::ISceneNode*				mLightOneNode;

	struct SimpleOne {
		graphics::ISceneNode*			pSN;
		graphics::IEntity*				pE;
		physics::IComplexObject*		pCO;
		model::MovableUpdateController	updCtrl;
	};
	class SimpleTwo {
	private:
		model::Physical*				pPhysical;
		SharedSceneNodeList				nodes;
		SharedMUCList					ctrlrs;

	public:
		SimpleTwo() : pPhysical(0)
		{}
		~SimpleTwo()
		{
			nodes.clear();
			ctrlrs.clear();
			YAKE_SAFE_DELETE( pPhysical );
		}
		model::Physical* getPhysical()
		{
			if (!pPhysical)
				pPhysical = new model::Physical();
			YAKE_ASSERT( pPhysical );
			return pPhysical;
		}
		SharedMUCList& getCtrlrs()
		{
			return ctrlrs;
		}
		SharedSceneNodeList& getNodes()
		{
			return nodes;
		}
		void update( const real timeElapsed )
		{
			for (SharedMUCList::const_iterator it = ctrlrs.begin();
				it != ctrlrs.end();
				++it)
			{
				(*it).get()->update( timeElapsed );
			}
		}
	};
	SimpleOne								mNinja;
	SimpleTwo*								mpBox;
	SimpleTwo*								mpSphere;
	SimpleOne								mGround;
	model::complex::Model*					mCarModel;
	model::complex::vehicle::LandVehicle*	mCar;
public:
	TheApp() : ExampleApplication( true /*graphics*/,
									true /*physics*/,
									false /*scripting*/,
									true /*input*/,
									false /*script bindings*/,
									true /*audio*/),
				mCarModel(0),
				mCar(0),
				mpBox(0),
				mpSphere(0),
				mpAWorld(0),
				mpASource(0),
				mpAListener(0),
				mpAData(0),
				mLightOneNode(0)
	{
		mNinja.pSN = 0;
	}

	void onKey(const yake::input::KeyboardEvent & e)
	{
		std::cout << "KEY: " << e.keyCode << std::endl;
		if (e.keyCode == input::KC_ESCAPE)
			requestShutdown();
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

		// used for shadows
		// incase infinite far distance is not supported
		pC->setFarClipDistance( 100000 );

		mVPs.push_back( std::pair<IViewport*,ICamera*>(mGWorld->createViewport( pC ), pC) );
		size_t idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		return static_cast<int>(idx);
	}

	void setupDotScene()
	{
		// dotScene test

		// 1. read dotscene file into DOM

		SharedPtr<dom::ISerializer> ser( new dom::xml::XmlSerializer() );
		ser->parse("../../media/graphics.scenes/r2t/r2t_test.scene", false);

		// 2. parse DOM and create graphical scene

		graphics::ISceneNode* pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( pSN );
		yake::data::serializer::dotscene::DotSceneSerializerV1 dss;
		dss.load( ser->getDocumentNode(), mGWorld, pSN );
	}

	void setupNinja()
	{
		// setup NINJA
		mNinja.pCO = 0; // no physical representation

		mNinja.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mNinja.pSN );

		mNinja.pE = mGWorld->createEntity("ninja.mesh");
		YAKE_ASSERT( mNinja.pE );
		mNinja.pE->setCastsShadow( false );

		mNinja.pSN->attachEntity( mNinja.pE );
		mNinja.pSN->setPosition( Vector3(0,0,-500) );
	}

	void setupGround()
	{
		// setup GROUND

		// setup physical representation
		mGround.pCO = mPWorld->createPlane( Vector3(0, 1, 0), 0 );
		YAKE_ASSERT( mGround.pCO );
		mGround.pCO->setFriction( 100 );
		mGround.pCO->setFriction2( 10 );

		// create entity
		mGround.pE = mGWorld->createEntity("plane_1x1.mesh");
		YAKE_ASSERT( mGround.pE );
		mGround.pE->setCastsShadow( false  );

		// create scene node and attach entity to node
		mGround.pSN = mGWorld->createSceneNode();
		YAKE_ASSERT( mGround.pSN );
		mGround.pSN->attachEntity( mGround.pE );
	}

	void setupBox()
	{
		mpBox = new SimpleTwo();
		YAKE_ASSERT( mpBox );

		// setup physical representation
		physics::IComplexObject* pCO = mPWorld->createBox( 2, 2, 2 );
		YAKE_ASSERT( pCO );
		pCO->setPosition( Vector3(0.75,10,-10) );

		YAKE_ASSERT( mpBox->getPhysical() );
		mpBox->getPhysical()->addComplex( SharedPtr<physics::IComplexObject>( pCO ) );

		// setup graphical representation & update controllers
		createDebugGeometryForComplexPhysical(
			pCO,
			mGWorld.get(),
			mpBox->getNodes(),
			mpBox->getCtrlrs() );
	}

	void setupSphere()
	{
		mpSphere = new SimpleTwo();
		YAKE_ASSERT( mpSphere );

		// setup physical representation
		physics::IComplexObject* pCO = mPWorld->createSphere( 2 );
		YAKE_ASSERT( pCO );
		pCO->setPosition( Vector3(0,15,-10) );

		YAKE_ASSERT( mpSphere->getPhysical() );
		mpSphere->getPhysical()->addComplex( SharedPtr<physics::IComplexObject>( pCO ) );

		// setup graphical representation & update controllers
		createDebugGeometryForComplexPhysical(
			pCO,
			mGWorld.get(),
			mpSphere->getNodes(),
			mpSphere->getCtrlrs() );
	}

	void setupAudio()
	{
		YAKE_ASSERT( getAudioSystem() );
		mpAWorld = getAudioSystem()->createWorld();
		YAKE_ASSERT( mpAWorld );
		mpAListener = mpAWorld->createListener();
		YAKE_ASSERT( mpAListener );
		mpASource = mpAWorld->createSource();
		YAKE_ASSERT( mpASource );
		mpAData = mpAWorld->createSoundDataFromFile("bee.wav");
		YAKE_ASSERT( mpAData );
		//mpASource->setSoundData("bee.wav");
		if (mpAData)
		{
			mpAData->setLoopMode( audio::ISoundData::SLM_LOOP_ON );
		}
		mpASource->setSoundData( mpAData );
		mpASource->play();
	}

	void cleanUpAudio()
	{
		YAKE_SAFE_DELETE( mpASource );
		YAKE_SAFE_DELETE( mpAData );
		YAKE_SAFE_DELETE( mpAListener );
	}

			void createWheel(
				model::complex::vehicle::Wheel *& pWheel,
				physics::IComplexObject *& pCO,
				SharedPtr<physics::IJoint> & pJ,
				physics::IComplexObject* pCOChassis,
				const real radius,
				const Vector3 & position,
				const Quaternion & orientation,
				const Vector3 & anchorPt,
				const Vector3 & hingeSteeringAxis,
				const Vector3 & hingeSuspensionAxis,
				const real suspSpring,
				const real suspDamping,
				const real wheelMass )
			{
				pWheel = new model::complex::vehicle::Wheel();
				YAKE_ASSERT( pWheel );

				// physical
				YAKE_ASSERT( pCO == 0 );
				pCO = mPWorld->createSphere(radius);
				YAKE_ASSERT( pCO );
				pCO->getBody()->setMassSphere( radius, 1 );
				pCO->getBody()->setMass( wheelMass );
				pCO->setPosition( position );
				pCO->setOrientation( orientation );
				pCO->setSoftness( 0.001 );

				// joint (to connect to chassis)
				pJ.reset( mPWorld->createJoint( physics::IJoint::JT_HINGE2 ) );
				YAKE_ASSERT( pJ );
				pJ->attach( pCOChassis->getBody(), pCO->getBody() );
				pJ->setAnchor( anchorPt );
				pJ->setAxis1( hingeSteeringAxis );
				pJ->setAxis2( hingeSuspensionAxis );
				pJ->setHighStop(0);
				pJ->setLowStop(0);
				pJ->setSpring( suspSpring );
				pJ->setDamping( suspDamping );

				//
				pWheel->setJoint( pJ );
				pWheel->setComplex( pCO );
			}
			model::complex::vehicle::Wheel* createWheel(	
								const Vector3 & position,
								const Quaternion & orientation,
								physics::IComplexObject* pCOChassis,
								SharedPtr<model::Physical> & pPhysical,
								const real radius,
								const real suspSpring,
								const real suspDamping,
								const real wheelMass,
								Vector<graphics::ISceneNode*> & nodeList,
								SharedMUCList & mucList )
			{
				model::complex::vehicle::Wheel* pWheel = 0;
				physics::IComplexObject* pCO = 0;
				SharedPtr<physics::IJoint> pJ;
				createWheel( pWheel, pCO, pJ, pCOChassis, radius,
					position, orientation, position /*anchorPt*/,
					Vector3::kUnitY /*hingeSteeringAxis*/, Vector3::kUnitX /*hingeSuspensionAxis*/,
					suspSpring, suspDamping, wheelMass );
				YAKE_ASSERT( pCO );
				pCO->setFriction( 20 );
				pCO->setFriction2( 5 );
				pPhysical->addJoint( pJ );
				pPhysical->addComplex( SharedPtr<physics::IComplexObject>(pCO) );

				// setup graphical representation & controllers
				createDebugGeometryForComplexPhysical( pCO, mGWorld.get(), nodeList, mucList );

				return pWheel;
			}

	void setupCar()
	{
		// setup VEHICLE
		YAKE_ASSERT( mCar == 0 );
		mCar = new model::complex::vehicle::LandVehicle();
		YAKE_ASSERT( mCar );

		// setup MODEL

		YAKE_ASSERT( mCarModel == 0 );
		mCarModel = new model::complex::Model();
		YAKE_ASSERT( mCarModel );

		mCar->setModel( mCarModel );

		// - add physical
		SharedPtr<model::Physical> pPhysical( new model::Physical() );
		mCarModel->addPhysical( pPhysical );

		// store scene nodes and controllers...
		Vector<graphics::ISceneNode*> nodeList;	// for debug graphics
		SharedMUCList mucList;

		const Vector3 offset( 0, 4, 0 );
		//const Vector3 carDim( 1.85, 1.14, 4.21 ); // original DeLorean
		const Vector3 carDim( 1.75, 1.0, 4.21 );
		const real suspSpring = 55;
		const real suspDamping = 6;
		const real chassisMass = 2.5; // 1.5 tonnes
		const real wheelMass = chassisMass / 70; // realistic: 60 to 70
		const real wheelRadius = 0.275;
		const real frontWheelZ = 1.175;
		const real rearWheelZ = -1.126;
		const real wheelOffsetX = 0.5 * (carDim.x - wheelRadius) - 0.05;

		// just a single chassis box (for now)
		physics::IComplexObject* pCOChassis = mPWorld->createBox(0.09*carDim.x, 0.5*carDim.y, 0.09*carDim.z);
		dbgChassis = pCOChassis;
		YAKE_ASSERT( pCOChassis );
		pPhysical->addComplex( SharedPtr<physics::IComplexObject>(pCOChassis) );
		//createDebugGeometryForComplexPhysical( pCOChassis, mGWorld.get(), nodeList, mucList );
		pCOChassis->setPosition( Vector3(0,0.33,0) );
		pCOChassis->getBody()->setMassBox( carDim.x, carDim.y, carDim.y, 1. );
		pCOChassis->getBody()->setMass( chassisMass );
		pCOChassis->getBody()->translateMass( Vector3(0,-1,0) );
		pCOChassis->setSoftness( 0.001 );
		pCOChassis->setFriction( 5 ); // 100
		pCOChassis->setFriction2( 0.1 ); // 2
		//if (mucList.size() > 0)
		{
			graphics::ISceneNode* pChassisMeshNode = mGWorld->createSceneNode();
			YAKE_ASSERT( pChassisMeshNode );
			graphics::IEntity* pE = mGWorld->createEntity("delorean.mesh");
			YAKE_ASSERT( pE );
			pChassisMeshNode->attachEntity( pE );
			pE->setCastsShadow( false  );
			SharedPtr<model::MovableUpdateController> pMUC //= mucList.front();
				( new model::MovableUpdateController() );
			mucList.push_back( pMUC );
			pMUC->setUpdateSource( pCOChassis->getBody() );
			pMUC->subscribeToPositionChanged( pChassisMeshNode );
			pMUC->subscribeToOrientationChanged( pChassisMeshNode );
			nodeList.push_back( pChassisMeshNode );
		}

		// left front wheel
		model::complex::vehicle::Wheel* pWheel = createWheel(
			Vector3( -wheelOffsetX, 0, frontWheelZ),
			Quaternion::kIdentity,
			pCOChassis,
			pPhysical,
			wheelRadius, // radius
			suspSpring,
			suspDamping,
			wheelMass,
			nodeList,
			mucList );
		mCar->addWheel( "lf", pWheel );
		mCar->setWheelAffectedByBrake( 0, pWheel, 0.2 );

		// right front wheel
		pWheel = createWheel(
			Vector3( wheelOffsetX, 0, frontWheelZ),
			Quaternion::kIdentity,
			pCOChassis,
			pPhysical,
			wheelRadius, // radius
			suspSpring,
			suspDamping,
			wheelMass,
			nodeList,
			mucList );
		mCar->addWheel( "rf", pWheel );
		mCar->setWheelAffectedByBrake( 0, pWheel, 0.2 );

		// left rear wheel
		pWheel = createWheel(
			Vector3( -wheelOffsetX, 0, rearWheelZ),
			Quaternion::kIdentity,
			pCOChassis,
			pPhysical,
			wheelRadius, // radius
			suspSpring,
			suspDamping,
			wheelMass,
			nodeList,
			mucList );
		mCar->addWheel( "lr", pWheel );
		mCar->setWheelAffectedByBrake( 0, pWheel, 1. );

		// right rear wheel
		pWheel = createWheel(
			Vector3( wheelOffsetX, 0, rearWheelZ),
			Quaternion::kIdentity,
			pCOChassis,
			pPhysical,
			wheelRadius, // radius
			suspSpring,
			suspDamping,
			wheelMass,
			nodeList,
			mucList );
		mCar->addWheel( "rr", pWheel );
		mCar->setWheelAffectedByBrake( 0, pWheel, 1. );

		// FINALIZE COMPLEX MODEL
		// - translate physical model
		pPhysical->translate( offset );
		// - add all created controllers to the model
		{
			ConstVectorIterator<SharedMUCList> it( mucList.begin(), mucList.end() );
			while (it.hasMoreElements())
			{
				SharedPtr<model::IObjectController> pMUC = it.getNext();
				YAKE_ASSERT( pMUC.get() );
				mCarModel->addController( SharedPtr<model::IObjectController>(pMUC) );
			}
		}
		// - add graphical
		SharedPtr<model::Graphical> pGraphical( new model::Graphical() );
		mCarModel->addGraphical( pGraphical );
		// - add created scene nodes to graphical
		YAKE_ASSERT( pGraphical );
		{
			ConstVectorIterator<Vector<graphics::ISceneNode*> > it( nodeList.begin(), nodeList.end() );
			while (it.hasMoreElements())
			{
				pGraphical->addSceneNode( it.getNext(), true );
			}
		}

		// ENGINE
		model::complex::vehicle::Engine* pEngine = new model::complex::vehicle::Engine();
		YAKE_ASSERT( pEngine );
		mCar->setEngine( AutoPtr<model::complex::vehicle::Engine>(pEngine) );

		// AXLES
		// front
		model::complex::vehicle::Axle* pAxle = new model::complex::vehicle::Axle();
		YAKE_ASSERT( pAxle );
		//pEngine->attachAxle( pAxle );
		pAxle->attachAffectedWheel( mCar->getWheel("lf") );
		pAxle->attachAffectedWheel( mCar->getWheel("rf") );
		// rear
		pAxle = new model::complex::vehicle::Axle();
		YAKE_ASSERT( pAxle );
		pEngine->attachAxle( pAxle );
		pAxle->attachAffectedWheel( mCar->getWheel("lr") );
		pAxle->attachAffectedWheel( mCar->getWheel("rr") );
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
		pL->setType( graphics::ILight::LT_POINT );
		pL->setCastsShadow( false  );
		pL->setDiffuseColour( Color(1,1,1) );
		pL->setAttenuation( 8000, 1, 0.0005, 0 );
		pL->setSpecularColour( Color(1,1,1) );
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( TheApp::onMB, this ) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );

		mGWorld->setShadowsEnabled( false );

		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );
		//createCameraViewportPair( 0.0, 0.0, 0.5, 0.5, 10 );
		//createCameraViewportPair( 0.5, 0.0, 0.5, 0.5, 11 );
		//createCameraViewportPair( 0.0, 0.5, 0.5, 0.5, 12 );
		//createCameraViewportPair( 0.5, 0.5, 0.5, 0.5, 13 );

		if (mVPs[0].second)
		{
			mVPs[0].second->translate( Vector3(0,1.6,20) );
			mVPs[0].second->pitch(-30);
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

		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		mPWorld->setGlobalGravity( Vector3(0,-9.81,0) );

		// audio
		setupAudio();

		// objects
		setupLights();
		//setupNinja();
		setupGround();
		setupBox();
		setupSphere();
		setupDotScene();
		setupCar();

		CameraFollowController cfc( mVPs[0].second, dbgChassis );

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			PROFILER_BEGINFRAME;

			PROFILE_BEGIN( frame );

			// timing
			real time = base::native::getTime();
			real timeElapsed = time-lastTime;//timer->getSeconds();
			lastTime = time;

			// handle input
			PROFILE_BEGIN( input );
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();

			//
			YAKE_ASSERT( getKeyboard() );
			if (getKeyboard())
			{
				static iCam = 0;
				real distance = -5. * timeElapsed;
				if (getKeyboard()->isKeyDown(input::KC_LEFT))
					mVPs[iCam].second->translate( Vector3(distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_RIGHT))
					mVPs[iCam].second->translate( Vector3(-distance, 0, 0) );
				if (getKeyboard()->isKeyDown(input::KC_UP))
					mVPs[iCam].second->translate( Vector3(0, 0, distance) );
				if (getKeyboard()->isKeyDown(input::KC_DOWN))
					mVPs[iCam].second->translate( Vector3(0, 0, -distance) );

				if (getKeyboard()->isKeyDown(input::KC_PGUP))
				{
					model::complex::vehicle::Wheel* pWheel = mCar->getWheel("lf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring(),
						pWheel->getSuspensionDamping() + timeElapsed );
					pWheel = mCar->getWheel("rf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring(),
						pWheel->getSuspensionDamping() + timeElapsed );
				}
				if (getKeyboard()->isKeyDown(input::KC_PGDOWN))
				{
					model::complex::vehicle::Wheel* pWheel = mCar->getWheel("lf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring(),
						pWheel->getSuspensionDamping() - timeElapsed );
					pWheel = mCar->getWheel("rf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring(),
						pWheel->getSuspensionDamping() - timeElapsed );
					std::cout << "spring= " << pWheel->getSuspensionSpring() <<
						"  damping= " << pWheel->getSuspensionDamping() << std::endl;
				}
				if (getKeyboard()->isKeyDown(input::KC_HOME))
				{
					model::complex::vehicle::Wheel* pWheel = mCar->getWheel("lf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring() + timeElapsed,
						pWheel->getSuspensionDamping() );
					pWheel = mCar->getWheel("rf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring() + timeElapsed,
						pWheel->getSuspensionDamping() );
				}
				if (getKeyboard()->isKeyDown(input::KC_END))
				{
					model::complex::vehicle::Wheel* pWheel = mCar->getWheel("lf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring() - timeElapsed,
						pWheel->getSuspensionDamping() );
					pWheel = mCar->getWheel("rf");
					YAKE_ASSERT( pWheel );
					pWheel->setSuspension(
						pWheel->getSuspensionSpring() - timeElapsed,
						pWheel->getSuspensionDamping() );
					std::cout << "spring= " << pWheel->getSuspensionSpring() <<
						"  damping= " << pWheel->getSuspensionDamping() << std::endl;
				}

				if (getKeyboard()->isKeyDown(input::KC_A))
					mCar->setSteer( -0.7 );
				else if (getKeyboard()->isKeyDown(input::KC_D))
					mCar->setSteer( 0.7 );
				else
					mCar->setSteer( 0 );

				if (getKeyboard()->isKeyDown(input::KC_W))
				{
					mCar->getEngine()->setThrottle(
						mCar->getEngine()->getThrottle() + timeElapsed );
				}
				else
					mCar->getEngine()->setThrottle(
						mCar->getEngine()->getThrottle() - 2. * timeElapsed );
				static real last = 0.;
				last -= timeElapsed;
				if (last < 0. && getKeyboard()->isKeyDown(input::KC_RETURN))
				{
					mCar->getEngine()->shiftGearUp();
					last = 0.2;
				}
				if (last < 0. && getKeyboard()->isKeyDown(input::KC_RSHIFT))
				{
					mCar->getEngine()->shiftGearDown();
					last = 0.2;
				}
				if (getKeyboard()->isKeyDown(input::KC_SPACE))
					mCar->brake( 0, 1. );
				else
					mCar->brake( 0, 0. );
			}
			PROFILE_END( input );

			// process physics
			PROFILE_BEGIN( physics );
			mPWorld->update( timeElapsed );
			PROFILE_END( physics );

			// update visual box representation from the physical one
			PROFILE_BEGIN( miscUpdates );
			PROFILE_BEGIN( box );
			if (mpBox)
				mpBox->update( timeElapsed );
			PROFILE_END( box );
			PROFILE_BEGIN( sphere );
			if (mpSphere)
				mpSphere->update( timeElapsed );
			PROFILE_END( sphere );
			PROFILE_BEGIN( car );
			mCar->update( timeElapsed );
			real vel = dbgChassis->getBody()->getLinearVelocity().length();
			std::cout << "\nRPM: " << mCar->getEngine()->getRPM() <<
						"\n T: " << mCar->getEngine()->getThrottle() <<
						"\n TQ:" << mCar->getEngine()->getDriveTorque() <<
						"\n D: " << mCar->getEngine()->getGear() <<
						//"\n S: " << mCar->getSteer();
						"\n V: " << vel << " | " << vel * 3.6 << " km/h";
			//mCarModel->update( timeElapsed );
			mLightOneNode->setPosition(
				dbgChassis->getPosition() + Vector3(0,10,0) );
			PROFILE_END( car );

			cfc.update( timeElapsed );

			PROFILE_END( miscUpdates );

			// render the scene
			PROFILE_BEGIN( graphics );
			mGWorld->render( timeElapsed );
			PROFILE_END( graphics );

			PROFILE_END( frame );

			PROFILER_ENDFRAME;
		}

		// clean up (FIXME: still incomplete)
		// TODO: use a yake::base::State for handling all scene specific stuff...

		YAKE_SAFE_DELETE( mLightOneNode );

		YAKE_SAFE_DELETE( mCar );
		//YAKE_SAFE_DELETE( mCarModel ); is done by destructor of mCar

		YAKE_SAFE_DELETE( mpBox );
		YAKE_SAFE_DELETE( mpSphere );

		YAKE_SAFE_DELETE( mNinja.pSN );

		YAKE_SAFE_DELETE( mPWorld );
		mGWorld.reset();

		cleanUpAudio();

		profiler::ProfilerManager::instance().getProfiler("default").logToCout();
		PROFILER_DESTROY;
		PROFILERMANAGER_DESTROY;
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