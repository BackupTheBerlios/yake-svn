#include <yake/samples/physics/demo/pch.h>
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yake/physics/yakePhysics.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::math;

class MiniApp
{
public:
	MiniApp() {}
	~MiniApp() {}

	void init();
	void run();
	void cleanUp();
private:
	// types
	struct View
	{
		graphics::IViewport*	pViewport;
		graphics::ICamera*		pCamera;

		View(graphics::ICamera* pC, graphics::IViewport* pV) : pViewport(pV), pCamera(pC)
		{}
	};
	typedef Deque< SharedPtr<graphics::ISceneNode> > SceneNodes;
	typedef Deque< SharedPtr<graphics::IEntity> > Entities;
	typedef Deque< SharedPtr<graphics::ILight> > Lights;
	typedef Deque< SharedPtr<View> > Views;

	physics::WeakIMaterialPtr	mBouncyPhysicsMaterial1;
	physics::WeakIMaterialPtr	mBouncyPhysicsMaterial2;

	struct Simple
	{
		SharedPtr<graphics::ISceneNode>		pSN;
		SharedPtr<graphics::IEntity>		pE;
		physics::WeakIDynamicActorPtr		pActor;
		void update()
		{
			YAKE_ASSERT( pSN.get() );
			YAKE_ASSERT( !pActor.expired() );
			pSN->setPosition( pActor.lock()->getPosition() );
			pSN->setOrientation( pActor.lock()->getOrientation() );
		}
		Simple(SharedPtr<physics::IWorld>& pPWorld_) : pPWorld( pPWorld_ )
		{}
		~Simple()
		{
			pPWorld->destroyActor( physics::WeakIActorPtr( pActor ) );
		}
		SharedPtr<physics::IWorld>			pPWorld;
	};
private:
	// variables
	bool						mShutdownRequested;

	Views						mViews;;
	SceneNodes					mSceneNodes;
	Entities					mEntities;
	Lights						mLights;

	Deque< SharedPtr<Library> >	mLibs;

	SharedPtr<graphics::IGraphicsSystem> mGraphics;
	SharedPtr<graphics::IWorld>	mGWorld;

	SharedPtr<physics::IPhysicsSystem>	mPhysics1;
	SharedPtr<physics::IPhysicsSystem>	mPhysics2;
	SharedPtr<physics::IWorld>	mPWorld1;
	SharedPtr<physics::IWorld>	mPWorld2;

	// private methods
	void removeSceneNode( SharedPtr<graphics::ISceneNode> & pSceneNode )
	{
		SceneNodes::iterator itFind = std::find(mSceneNodes.begin(),mSceneNodes.end(),pSceneNode);
		if (itFind != mSceneNodes.end())
			mSceneNodes.erase( itFind );
	}
	void removeEntity( SharedPtr<graphics::IEntity> & pEntity )
	{
		Entities::iterator itFind = std::find(mEntities.begin(),mEntities.end(),pEntity);
		if (itFind != mEntities.end())
			mEntities.erase( itFind );
	}
	void createSimpleGraphicalObject(	Simple & rSimple,
										const String & rkMesh,
										const Vector3 & rkPosition = Vector3::kZero,
										const Vector3 & rkScale = Vector3(1,1,1))
	{
		rSimple.pSN.reset( mGWorld->createSceneNode() );
		rSimple.pE.reset( mGWorld->createEntity(rkMesh) );
		rSimple.pE->setMaterial("Examples/BumpyMetal");
		rSimple.pSN->attachEntity( rSimple.pE.get() );
		rSimple.pSN->setPosition( rkPosition );
		rSimple.pSN->setScale( rkScale );
	}
	void createSimpleActor(	SharedPtr<physics::IWorld> pPWorld,
							Simple & rSimple, 
							physics::IShape::Desc & rkDesc,
							const Vector3 & rkPosition,
							const real mass = 2 )
	{
		rSimple.pActor = pPWorld->createDynamicActor();
		YAKE_ASSERT( !rSimple.pActor.expired() );
		rSimple.pActor.lock()->createShape( rkDesc );
		YAKE_ASSERT( rSimple.pActor.lock()->getShapes().size() == 1 );

		rSimple.pActor.lock()->getBody().setMass( mass );
		rSimple.pActor.lock()->setPosition( rkPosition );

		rSimple.pActor.lock()->subscribeToCollisionEnteredSignal( boost::bind(MiniApp::onCollisionEntered,this));
	}
	bool createBall( SharedPtr<physics::IWorld> pPWorld, Simple & rSimple, const Vector3 & rkPosition, const real radius )
	{
		// graphics
		createSimpleGraphicalObject( rSimple, "sphere_d1.mesh", Vector3(0,0,0), radius*Vector3(2,2,2) );

		// physics
		createSimpleActor( pPWorld, rSimple, physics::IShape::SphereDesc(real(radius)), rkPosition );

		return true;
	}
	bool createCapsule( SharedPtr<physics::IWorld> pPWorld, Simple & rSimple, const Vector3 & rkPosition, const real height, const real radius )
	{
		// graphics
		createSimpleGraphicalObject( rSimple, "box_1x1x1.mesh", Vector3(0,0,0), Vector3(radius,height,radius) );

		// physics
		createSimpleActor( pPWorld, rSimple, physics::IShape::CapsuleDesc(real(height), real(radius)), rkPosition );

		return true;
	}
	bool createBox( SharedPtr<physics::IWorld> pPWorld, Simple & rSimple, const Vector3 & rkPosition, const Vector3 & rkDimensions )
	{
		// graphics
		createSimpleGraphicalObject( rSimple, "box_1x1x1.mesh", Vector3(0,0,0), 2*rkDimensions );

		// physics
		createSimpleActor( pPWorld, rSimple, physics::IShape::BoxDesc(rkDimensions), rkPosition );

		return true;
	}
	physics::WeakIJointPtr createFixedJoint(SharedPtr<physics::IWorld> pPWorld, Simple & rSimple1, Simple & rSimple2)
	{
		YAKE_ASSERT( !rSimple1.pActor.expired() );
		YAKE_ASSERT( !rSimple2.pActor.expired() );
		return pPWorld->createJoint(
			physics::IJoint::DescFixed( *rSimple1.pActor.lock(), *rSimple2.pActor.lock() ) );
	}
	SharedPtr<base::Library> loadLib( const base::String & file )
	{
		SharedPtr<base::Library> pDynLib( new base::Library( file ) );
		YAKE_ASSERT( pDynLib ).debug( "Out of memory." );
		mLibs.push_back( pDynLib );
		return pDynLib;
	}
	void createSimpleEntity( const String & rkMesh )
	{
		SharedPtr<graphics::ISceneNode> pSN( mGWorld->createSceneNode() );
		SharedPtr<graphics::IEntity> pE( mGWorld->createEntity( rkMesh ) );
		pSN->attachEntity( pE.get() );
		mSceneNodes.push_back( pSN );
		mEntities.push_back( pE );
		pE->setCastsShadow( true );
	}
	void createNinja()
	{
		SharedPtr<graphics::ISceneNode> pSN( mGWorld->createSceneNode() );
		SharedPtr<graphics::IEntity> pE( mGWorld->createEntity( "ninja.mesh" ) );
		mSceneNodes.push_back( pSN );
		mEntities.push_back( pE );
		pSN->attachEntity( pE.get() );

		pE->setCastsShadow( true );

		pSN->setPosition( Vector3(0,0,-100) );
	}
	void createPositionedLight()
	{
		SharedPtr<graphics::ISceneNode> pSN( mGWorld->createSceneNode() );
		SharedPtr<graphics::ILight> pL( mGWorld->createLight() );
		pSN->attachLight( pL.get() );
		mSceneNodes.push_back( pSN );
		mLights.push_back( pL );
		pL->setType( graphics::ILight::LT_POINT );
		pL->setDiffuseColour( Color(1,1,1,1) );
		pSN->setPosition( Vector3(0,50,0) );
        pL->setDiffuseColour(Color(0.35, 0.35, 0.38));
        pL->setSpecularColour(Color(0.9, 0.9, 1));
		pL->setAttenuation( 8000, 1, 0.0005, 0 );
		pL->setCastsShadow( true );
	}
	void createGroundPlane()
	{
		createSimpleEntity("plane_1x1.mesh");
		mSceneNodes.back()->setScale(Vector3(30,1,30));
		mEntities.back()->setMaterial("Examples/BumpyMetal");
		mEntities.back()->setCastsShadow(false);
	}
	void requestShutdown()
	{
		mShutdownRequested = true;
	}
	void onCollisionEntered()
	{
		YAKE_LOG("demo: collision!");
	}
	void setupLights()
	{
		using namespace graphics;
		// fixed light (sun)
		SharedPtr<graphics::ILight> pL( mGWorld->createLight() );
		SharedPtr<graphics::ISceneNode> pNode( mGWorld->createSceneNode() );
		pNode->attachLight( pL.get() );
		
		mLights.push_back( pL );
		mSceneNodes.push_back(pNode);

		pL->setType(ILight::LT_DIRECTIONAL);
        pNode->setPosition(Vector3(-1000,1250,500));
        Vector3 dir;
        dir = -pNode->getPosition();
        dir.normalise();
        pL->setDirection(dir);
        pL->setDiffuseColour(Color(0.35, 0.35, 0.38));
        pL->setSpecularColour(Color(0.9, 0.9, 1));
		pL->setCastsShadow( true );
	}
};
void MiniApp::init()
{
	mShutdownRequested = false;

	// physics 1

	SharedPtr<base::Library> pLib = loadLib("physicsNX" );
	YAKE_ASSERT( pLib ).debug("Cannot load physics plugin 1.");

	mPhysics1 = create< physics::IPhysicsSystem >("nx");
	YAKE_ASSERT( mPhysics1 ).debug("Cannot create physics system 1.");

	mPWorld1 = mPhysics1->createWorld();
	YAKE_ASSERT( mPWorld1 ).debug("Cannot create world 1.");

	// physics 2

	pLib = loadLib("physicsOde" );
	YAKE_ASSERT( pLib ).debug("Cannot load physics plugin 2.");

	mPhysics2 = create< physics::IPhysicsSystem >("ode");
	YAKE_ASSERT( mPhysics2 ).debug("Cannot create physics system 2.");

	mPWorld2 = mPhysics2->createWorld();
	YAKE_ASSERT( mPWorld2 ).debug("Cannot create world 2.");

	// graphics

	pLib = loadLib("graphicsOgre" );
	YAKE_ASSERT( pLib ).debug("Cannot load graphics plugin.");

	mGraphics = create< graphics::IGraphicsSystem >();
	YAKE_ASSERT( mGraphics ).debug("Cannot create graphics system.");

	mGWorld = mGraphics->createWorld();
	YAKE_ASSERT( mGWorld ).debug("Cannot create graphics world.");

	mGWorld->setShadowsEnabled( true );

	mGraphics->subscribeToShutdownSignal( boost::bind(MiniApp::requestShutdown,this) );

	{
		graphics::ICamera* pC = mGWorld->createCamera();
		pC->setNearClipDistance( 1 );
		pC->setFarClipDistance( 1000 );
		pC->setPosition( Vector3(0,2,10) );
		pC->lookAt( Vector3(0,4,0) );
		graphics::IViewport* pV = mGWorld->createViewport( pC );
		pV->setDimensions( 0.7, 0, 0.3, 0.3 );
		pV->setZ( 102 );

		mViews.push_back( SharedPtr<View>( new View(pC, pV) ) );
	}

#define VP_1
#ifdef VP_1
	{
		graphics::ICamera* pC = mGWorld->createCamera();
		pC->setNearClipDistance( 1 );
		pC->setFarClipDistance( 1000 );
		pC->setPosition( Vector3(0,10,50) );
		pC->lookAt( Vector3(0,1,0) );
		graphics::IViewport* pV = mGWorld->createViewport( pC );
		pV->setDimensions( 0, 0, 0.3, 0.3 );
		pV->setZ( 101 );

		mViews.push_back( SharedPtr<View>( new View(pC, pV) ) );
	}
#endif

#define VP_2
#ifdef VP_2
	{
		graphics::ICamera* pC = mGWorld->createCamera();
		pC->setNearClipDistance( 1 );
		pC->setFarClipDistance( 1000 );
		pC->setPosition( Vector3(25,20,25) );
		pC->lookAt( Vector3(0,1,0) );
		graphics::IViewport* pV = mGWorld->createViewport( pC );
		pV->setDimensions( 0, 0, 1, 1 );
		pV->setZ( 100 );

		mViews.push_back( SharedPtr<View>( new View(pC, pV) ) );
	}
#endif
}
void MiniApp::cleanUp()
{
	mLights.clear();
	mEntities.clear();
	mSceneNodes.clear();
	if (!mBouncyPhysicsMaterial1.expired())
		mPWorld1->destroyMaterial( mBouncyPhysicsMaterial1 );
	if (!mBouncyPhysicsMaterial2.expired())
		mPWorld1->destroyMaterial( mBouncyPhysicsMaterial2 );
	//
	DequeIterator<Views> it(mViews.begin(), mViews.end());
	while (it.hasMoreElements())
	{
		SharedPtr<View> & pView = it.getNext();
		YAKE_ASSERT( pView );
		YAKE_SAFE_DELETE( pView->pViewport );
		YAKE_SAFE_DELETE( pView->pCamera );
	}
	mViews.clear();
	// destroy worlds before we destroy the underlying system plugins
	mPWorld1.reset();
	mPWorld2.reset();
	mGWorld.reset();
	//
	mGraphics.reset();
	mPhysics1.reset();
	mPhysics2.reset();
	// unload plugins
	mLibs.clear();
}

std::ostream& operator << (std::ostream & o, const physics::IDynamicActor & rhs)
{
	Vector3 pos = rhs.getPosition();
	o << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
	return o;
}

void MiniApp::run()
{
	using namespace physics;
	YAKE_ASSERT( mPWorld1 );
	YAKE_ASSERT( mPWorld2 );

	// setup physics materials
/*	mBouncyPhysicsMaterial = mPWorld->createMaterial( IMaterial::Desc( 0.1, 0.1 ) );
	YAKE_ASSERT( mBouncyPhysicsMaterial );
	*/

	// create a simple static environment - world 1
	WeakIStaticActorPtr pStaticWorldActor1 = mPWorld1->createStaticActor();
	YAKE_ASSERT( pStaticWorldActor1.lock() );
	/*
	  3-2
	  |/|
	  0-1
	*/
	TriangleMeshDesc groundMeshDesc;
	groundMeshDesc.vertices.push_back( Vector3(-100,0, 100) );
	groundMeshDesc.vertices.push_back( Vector3( 100,0, 100) );
	groundMeshDesc.vertices.push_back( Vector3( 100,0,-100) );
	groundMeshDesc.vertices.push_back( Vector3(-100,0,-100) );
	groundMeshDesc.indices.push_back( 0 ); // tri #1
	groundMeshDesc.indices.push_back( 1 );
	groundMeshDesc.indices.push_back( 2 );
	groundMeshDesc.indices.push_back( 0 ); // tri #2
	groundMeshDesc.indices.push_back( 2 );
	groundMeshDesc.indices.push_back( 3 );
	TriangleMeshId groundMeshId = mPWorld1->createTriangleMesh(groundMeshDesc);
	YAKE_ASSERT( kTriangleMeshIdNone != groundMeshId );

	IShape* pStaticWorldShape1 = pStaticWorldActor1.lock()->createShape( IShape::TriMeshDesc( groundMeshId ) );
	YAKE_ASSERT( pStaticWorldActor1.lock()->getShapes().size() == 1 )(pStaticWorldActor1.lock()->getShapes().size()).warning("no world shape!");

	// create a simple static environment - world 2
	WeakIStaticActorPtr pStaticWorldActor2 = mPWorld2->createStaticActor();
	YAKE_ASSERT( pStaticWorldActor2.lock() );
	pStaticWorldActor2.lock()->createShape( IShape::PlaneDesc( Vector3(0,1,0), 0 ) );
	YAKE_ASSERT( pStaticWorldActor2.lock()->getShapes().size() == 1 )(pStaticWorldActor2.lock()->getShapes().size()).warning("no world shape!");

	// eye candy :P
	createGroundPlane();
	//createNinja();
	setupLights();

	typedef Deque< SharedPtr<Simple> > ObjectList;
	ObjectList	objs;

	RandomNumberGeneratorMT randomiser;

	real lastTime = base::native::getTime();
	while (!mShutdownRequested)
	{
		static uint32 siFrame = 0;
		// timing
		real time = base::native::getTime();
		real timeElapsed = time-lastTime;//timer->getSeconds();
		lastTime = time;

		// spawn randomised objects in intervals
		// @todo add compound objects, i.e. objects with compound collision geometry.
		static real timeSinceLastSpawn = 0;
		timeSinceLastSpawn -= timeElapsed;
		if (timeSinceLastSpawn <= 0.f)
		{
			timeSinceLastSpawn = 1.5;
			//timeSinceLastSpawn = 1000000.;

			// decide whether to spawn object in physics world 1 or two
			static bool bUseWorldOne = true;
			bUseWorldOne = !bUseWorldOne;
			SharedPtr<physics::IWorld> pPWorld = bUseWorldOne ? mPWorld1 : mPWorld2;

			// spawn the object
			const Vector3 spawnOffset = bUseWorldOne ? Vector3(-5,0,0) : Vector3(5,0,0);
			const Vector3 spawnPos = spawnOffset + Vector3(randomiser.randReal()*0.5,10,randomiser.randReal()*0.5);
			objs.push_back( SharedPtr<Simple>( new Simple( pPWorld ) ) );

			if (randomiser.randReal() < 0.5)
				createBox( pPWorld, *objs.back(), 
								spawnPos, // position
								Vector3(randomiser.randReal()*3,1,randomiser.randReal()*2) ); // dimensions
			else
				createBall( pPWorld, *objs.back(), 
								spawnPos,  // position
								randomiser.randReal()*2 ); //dimension/radius
		}

		// step the worlds
		mPWorld1->step( timeElapsed );
		mPWorld2->step( timeElapsed );

		// update graphics objects
		ConstVectorIterator< ObjectList > itObj( objs.begin(), objs.end() );
		while (itObj.hasMoreElements())
			itObj.getNext()->update();

		// render the scene
		mGWorld->render( timeElapsed );
	}
	mPWorld1->destroyActor( physics::WeakIActorPtr( pStaticWorldActor1 ) );
	YAKE_ASSERT( pStaticWorldActor1.expired() );

	mPWorld2->destroyActor( physics::WeakIActorPtr( pStaticWorldActor2 ) );
	YAKE_ASSERT( pStaticWorldActor2.expired() );
}

int main()
{
	try
	{
		std::cout << std::endl << "A simple demo :) provided for YAKE by Stephan Kaiser" << std::endl;
		std::cout << "For a more sophisticated demo have a look at the Yapp demos," << std::endl << "'sync1' and 'dotLink' and 'dotModel'." << std::endl;

		MiniApp app;
		app.init();
		app.run();
		app.cleanUp();
	}
	catch (const yake::base::Exception & e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
	catch (...)
	{
		//YAKE_LOG_ERROR("YAKE catched an unhandled exception");
		std::cout << std::endl << "YAKE catched an unhandled exception." << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
