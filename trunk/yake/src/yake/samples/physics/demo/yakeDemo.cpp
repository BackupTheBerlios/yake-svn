#include <yake/samples/physics/demo/pch.h>
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yake/physics/yakePhysics.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;

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
	//typedef std::pair< SharedPtr<graphics::IViewport>, SharedPtr<graphics::ICamera> > ViewportCameraPair;
	typedef Deque< SharedPtr<graphics::ISceneNode> > SceneNodes;
	typedef Deque< SharedPtr<graphics::IEntity> > Entities;
	typedef Deque< SharedPtr<graphics::ILight> > Lights;
	//typedef Deque< SharedPtr<ViewportCameraPair> > Views;
	typedef Deque< SharedPtr<View> > Views;

	SharedPtr< physics::IMaterial >	mBouncyPhysicsMaterial;

	struct Simple
	{
		SharedPtr<graphics::ISceneNode>	pSN;
		SharedPtr<graphics::IEntity>	pE;
		SharedPtr<physics::IActor>		pActor;
		void update()
		{
			YAKE_ASSERT( pSN.get() );
			YAKE_ASSERT( pActor.get() );
			pSN->setPosition( pActor->getPosition() );
			pSN->setOrientation( pActor->getOrientation() );
		}
	};
private:
	// variables
	bool						mShutdownRequested;

	Views						mViews;;
	SceneNodes					mSceneNodes;
	Entities					mEntities;
	Lights						mLights;

	Deque< SharedPtr<Library> >	mLibs;

	SharedPtr<physics::IPhysicsSystem>	mPhysics;
	SharedPtr<graphics::IGraphicsSystem> mGraphics;
	SharedPtr<physics::IWorld>	mPWorld;
	SharedPtr<graphics::IWorld>	mGWorld;

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
	void createSimpleActor(	Simple & rSimple, 
							physics::IShape::Desc & rkDesc,
							const Vector3 & rkPosition,
							const real mass = 1 )
	{
		rSimple.pActor = mPWorld->createActor();
		YAKE_ASSERT( rSimple.pActor );
		rSimple.pActor->createShape( rkDesc );
		YAKE_ASSERT( rSimple.pActor->getShapes().size() == 1 );

		rSimple.pActor->createBody();
		YAKE_ASSERT( rSimple.pActor->hasBody() );
		rSimple.pActor->getBody()->setMass( mass );
		rSimple.pActor->setPosition( rkPosition );
	}
	bool createBall( Simple & rSimple, const Vector3 & rkPosition, const real radius )
	{
		// graphics
		createSimpleGraphicalObject( rSimple, "sphere_d1.mesh", Vector3(0,0,0), radius*Vector3(2,2,2) );

		// physics
		createSimpleActor( rSimple, physics::IShape::SphereDesc(real(radius), mBouncyPhysicsMaterial.get()), rkPosition );

		return true;
	}
	bool createBox( Simple & rSimple, const Vector3 & rkPosition, const Vector3 & rkDimensions )
	{
		// graphics
		createSimpleGraphicalObject( rSimple, "box_1x1x1.mesh", Vector3(0,0,0), 2*rkDimensions );

		// physics
		createSimpleActor( rSimple, physics::IShape::BoxDesc(rkDimensions, mBouncyPhysicsMaterial.get()), rkPosition );

		return true;
	}
	SharedPtr<physics::IJoint> createFixedJoint(Simple & rSimple1, Simple & rSimple2)
	{
		YAKE_ASSERT( rSimple1.pActor );
		YAKE_ASSERT( rSimple2.pActor );
		SharedPtr<physics::IJoint> pJoint = mPWorld->createJoint(
			physics::IJoint::DescFixed( *rSimple1.pActor.get(), *rSimple2.pActor.get() ) );
		return pJoint;
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
		mSceneNodes.back()->setScale(Vector3(70,1,70));
		mEntities.back()->setMaterial("Examples/BumpyMetal");
		mEntities.back()->setCastsShadow(false);
	}
	void requestShutdown()
	{
		mShutdownRequested = true;
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

	// physics

	SharedPtr<base::Library> pLib = loadLib("physicsNX" );
	YAKE_ASSERT( pLib ).debug("Cannot load physics plugin.");

	mPhysics = create< physics::IPhysicsSystem >();
	YAKE_ASSERT( mPhysics ).debug("Cannot create physics system.");

	mPWorld = mPhysics->createWorld();
	YAKE_ASSERT( mPWorld ).debug("Cannot create world.");

	// graphics

	pLib = loadLib("graphicsOgre" );
	YAKE_ASSERT( pLib ).debug("Cannot load graphics plugin.");

	mGraphics = create< graphics::IGraphicsSystem >();
	YAKE_ASSERT( mGraphics ).debug("Cannot create graphics system.");

	mGWorld = mGraphics->createWorld();
	YAKE_ASSERT( mGWorld ).debug("Cannot create graphics world.");

	mGWorld->setShadowsEnabled( true );

	mGraphics->subscribeToShutdownSignal( boost::bind(MiniApp::requestShutdown,this) );

	// main 3d view
	{
		/*
	SharedPtr<ViewportCameraPair> vpPair( new ViewportCameraPair );
	vpPair->second.reset( mGWorld->createCamera() );
	vpPair->second->setNearClipDistance( 1. );
	vpPair->second->setFarClipDistance( 2000. );
	vpPair->second->setPosition( Vector3(50,50,50) );
	vpPair->second->lookAt( Vector3(0,2,0) );
	vpPair->first.reset( mGWorld->createViewport( vpPair->second.get() ) );
	vpPair->first->setDimensions(0,0,1,1);
	vpPair->first->setZ( 100 );
	mViews.push_back( vpPair );*/
	}

	//
	{
//#define VP_1
#ifdef VP_1
		graphics::ICamera* pC = mGWorld->createCamera();
		pC->setNearClipDistance( 1 );
		pC->setFarClipDistance( 1000 );
		pC->setPosition( Vector3(0,4,50) );
		pC->lookAt( Vector3(0,1,0) );
		graphics::IViewport* pV = mGWorld->createViewport( pC );
		pV->setDimensions( 0, 0, 0.3, 0.3 );
		pV->setZ( 101 );

		mViews.push_back( SharedPtr<View>( new View(pC, pV) ) );
#endif
	/*
	SharedPtr<ViewportCameraPair> vpPair( new ViewportCameraPair );
	vpPair->second.reset( mGWorld->createCamera() );
	vpPair->second->setNearClipDistance( 1. );
	vpPair->second->setFarClipDistance( 2000. );
	vpPair->second->setPosition( Vector3(50,50,50) );
	vpPair->second->lookAt( Vector3(0,2,0) );
	vpPair->first.reset( mGWorld->createViewport( vpPair->second.get() ) );
	vpPair->first->setDimensions(0,0,0.2,0.2);
	vpPair->first->setZ( 110 );
	mViews.push_back( vpPair );
	*/
	}
	{
#define VP_2
#ifdef VP_2
		graphics::ICamera* pC = mGWorld->createCamera();
		pC->setNearClipDistance( 1 );
		pC->setFarClipDistance( 1000 );
		pC->setPosition( Vector3(50,50,50) );
		pC->lookAt( Vector3(0,1,0) );
		graphics::IViewport* pV = mGWorld->createViewport( pC );
		pV->setDimensions( 0, 0, 1, 1 );
		pV->setZ( 100 );

		mViews.push_back( SharedPtr<View>( new View(pC, pV) ) );
#endif
	}
}
void MiniApp::cleanUp()
{
	mLights.clear();
	mEntities.clear();
	mSceneNodes.clear();
	mBouncyPhysicsMaterial.reset();
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
	mPWorld.reset();
	mGWorld.reset();
	//
	mGraphics.reset();
	mPhysics.reset();
	// unload plugins
	mLibs.clear();
}

std::ostream& operator << (std::ostream & o, const physics::IActor & rhs)
{
	Vector3 pos = rhs.getPosition();
	o << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
	return o;
}

void MiniApp::run()
{
	using namespace physics;
	YAKE_ASSERT( mPWorld );
	//
	mBouncyPhysicsMaterial = mPWorld->createMaterial(
									IMaterial::Desc( 0.1, 1.5 )
								);
	YAKE_ASSERT( mBouncyPhysicsMaterial );

	// static environment
	SharedPtr<IActor> pStaticWorldActor = mPWorld->createActor();
	YAKE_ASSERT( pStaticWorldActor );
//#define TRI
#ifdef TRI
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
	TriangleMeshId groundMeshId = mPWorld->createTriangleMesh(groundMeshDesc);
	YAKE_ASSERT( kTriangleMeshIdNone != groundMeshId );

	pStaticWorldActor->createShape( IShape::TriMeshDesc( groundMeshId ) );
#else
	pStaticWorldActor->createShape( IShape::PlaneDesc( Vector3(0,1,0), 0 ) );
#endif
	YAKE_ASSERT( pStaticWorldActor->getShapes().size() == 1 )(pStaticWorldActor->getShapes().size());

	// two very simple entities (if we dare call them that...)
	Simple s,ball2;
	createBall( s, Vector3(10,40,0), 1 );
	createBox( ball2, Vector3(10,20,0), Vector3(5,5,2) );

#define TEST_FIXED_JOINT
#ifdef TEST_FIXED_JOINT
	SharedPtr<physics::IJoint> pJ = createFixedJoint(s, ball2);
#endif

	// create a line up of boxes
	const size_t kNumBoxes = 12;
	Simple boxes[kNumBoxes];
	const real kBoxZ = 2;
	for (size_t i=0; i<kNumBoxes; ++i)
	{
		createBox( boxes[i], Vector3(-10,1,-kBoxZ*4*i), Vector3(4,8,kBoxZ) );
	}

#ifndef TEST_FIXED_JOINT
	// push the sphere against the line up of boxes
	s.pActor->setPosition( Vector3(-11,2,2) );
	s.pActor->getBody()->setMass( 30 );
	s.pActor->getBody()->setLinearVelocity( Vector3(0,1,-20) );
#endif

	// eye candy :P
	createGroundPlane();
	//createNinja();
	setupLights();

	real lastTime = base::native::getTime();
	while (!mShutdownRequested)
	{
		static uint32 siFrame = 0;
		// timing
		real time = base::native::getTime();
		real timeElapsed = time-lastTime;//timer->getSeconds();
		lastTime = time;
		std::cout << "frame " << siFrame++ << " fps " << ((timeElapsed > 0) ? (1./timeElapsed) : 0) << std::endl;

		//
		mPWorld->step( timeElapsed );
		std::cout << "ball at " << *(s.pActor) << std::endl;
		s.update();
		ball2.update();
		for (size_t i=0; i<kNumBoxes; ++i)
			boxes[i].update();

		mGWorld->render( timeElapsed );
	}
	/*
	pStaticWorldActor.reset();
	*/
}

int main()
{
	try
	{
		std::cout << std::endl << "A simple demo :) provided for YAKE by Stephan Kaiser" << std::endl;

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
		std::cout << std::endl << "YAKE catched an unhandled exception." << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
