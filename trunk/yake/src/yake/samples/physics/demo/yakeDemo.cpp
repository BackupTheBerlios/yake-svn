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
	typedef std::pair< SharedPtr<graphics::IViewport>, SharedPtr<graphics::ICamera> > ViewportCameraPair;
	typedef Deque< SharedPtr<graphics::ISceneNode> > SceneNodes;
	typedef Deque< SharedPtr<graphics::IEntity> > Entities;
	typedef Deque< SharedPtr<graphics::ILight> > Lights;

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
		}
	};
private:
	// variables
	bool						mShutdownRequested;
	ViewportCameraPair			mVpCamPair;
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
	bool createBall( Simple & rSimple )
	{
		// graphics
		rSimple.pSN.reset( mGWorld->createSceneNode() );
		rSimple.pE.reset( mGWorld->createEntity("sphere_d1.mesh") );
		rSimple.pSN->attachEntity( rSimple.pE.get() );
		rSimple.pSN->setScale(Vector3(2,2,2));

		// physics
		rSimple.pActor = mPWorld->createActor();
		YAKE_ASSERT( rSimple.pActor );
		rSimple.pActor->createShape( physics::IShape::SphereDesc(real(2)) );
		YAKE_ASSERT( rSimple.pActor->getShapes().size() == 1 );

		rSimple.pActor->createBody();
		YAKE_ASSERT( rSimple.pActor->hasBody() );
		rSimple.pActor->getBody()->setMass( 5 );
		rSimple.pActor->setPosition( Vector3(0,40,0) );
		rSimple.pActor->getBody()->setLinearVelocity( Vector3(1,0,0) );

		return true;
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
		mSceneNodes.back()->setScale(Vector3(50,1,50));
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

		// movable light 1
		/*
		pNode.reset( mGWorld->createSceneNode() );
		YAKE_ASSERT( pNode );
		mSceneNodes.push_back(pNode);

		pL.reset( mGWorld->createLight() );
		YAKE_ASSERT( pL );
		mLights.push_back( pL );
		pNode->attachLight( pL.get() );
		pL->setDiffuseColour( Color(0.6,0.7,0.8) );
		pL->setSpecularColour( Color(1,1,1) );
		pL->setAttenuation( 8000, 1, 0.0005, 0 );

//		pL->setType( graphics::ILight::LT_POINT );
		pL->setType( graphics::ILight::LT_SPOT );
		pL->setDirection( Vector3(0,0,1) );
		pL->setSpotlightRange(80,100,1);
		pL->setCastsShadow( true );

        pNode->setPosition(Vector3(300,250,-300));

		// movable light 2
		pNode.reset( mGWorld->createSceneNode() );
		YAKE_ASSERT( pNode );
		mSceneNodes.push_back(pNode);
		pL.reset( mGWorld->createLight() );
		YAKE_ASSERT( pL );
		mLights.push_back( pL );
		pNode->attachLight( pL.get() );
		pL->setDiffuseColour( Color(0.6,0.7,0.8) );
		pL->setSpecularColour( Color(1,1,1) );
		pL->setAttenuation( 8000, 1, 0.0005, 0 );

		pL->setType( graphics::ILight::LT_SPOT );
		Vector3 n(0,-1,-1);
		n.normalise();
		pL->setDirection( n );
		pL->setSpotlightRange(80,100,1);
		pL->setCastsShadow( true );

		pNode->setPosition(Vector3(0,150,0));
		*/
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

	mGWorld->setShadowsEnabled( false );

	mGraphics->subscribeToShutdownSignal( boost::bind(MiniApp::requestShutdown,this) );

	mVpCamPair.second.reset( mGWorld->createCamera() );
	mVpCamPair.second->setNearClipDistance( 1. );
	mVpCamPair.second->setFarClipDistance( 2000. );
	mVpCamPair.second->setPosition( Vector3(100,10,100) );
	mVpCamPair.second->lookAt( Vector3(0,2,0) );
	mVpCamPair.first.reset( mGWorld->createViewport( mVpCamPair.second.get() ) );
	mVpCamPair.first->setDimensions(0,0,1,1);
	mVpCamPair.first->setZ( 100 );
}
void MiniApp::cleanUp()
{
	mLights.clear();
	mEntities.clear();
	mSceneNodes.clear();
	//
	mVpCamPair.first.reset();
	mVpCamPair.second.reset();
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

	SharedPtr<IActor> pStaticWorldActor = mPWorld->createActor();
	YAKE_ASSERT( pStaticWorldActor );

	pStaticWorldActor->createShape( IShape::PlaneDesc( Vector3(0,1,0), 0 ) );
	YAKE_ASSERT( pStaticWorldActor->getShapes().size() == 1 )(pStaticWorldActor->getShapes().size());

	Simple s;
	createBall( s );

	createSimpleEntity("sphere_d1.mesh");
	createGroundPlane();
	createNinja();
	setupLights();

	real lastTime = base::native::getTime();
	while (!mShutdownRequested)
	{
		// timing
		real time = base::native::getTime();
		real timeElapsed = time-lastTime;//timer->getSeconds();
		lastTime = time;

		//
		mPWorld->step( timeElapsed );
		std::cout << "ball at " << *(s.pActor) << std::endl;
		s.update();

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
