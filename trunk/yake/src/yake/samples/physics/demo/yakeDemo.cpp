#include <yake/samples/physics/demo/pch.h>
#include <yake/physics/yakePhysicsSystem.h>

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
	Deque< SharedPtr<Library> >	mLibs;

	SharedPtr<physics::IWorld>	mWorld;

	SharedPtr<base::Library> loadLib( const base::String & file )
	{
		SharedPtr<base::Library> pDynLib( new base::Library( file ) );
		YAKE_ASSERT( pDynLib ).debug( "Out of memory." );
		mLibs.push_back( pDynLib );
		return pDynLib;
	}
};
void MiniApp::init()
{
	SharedPtr<base::Library> pLib = loadLib("physicsNX" );
	YAKE_ASSERT( pLib ).debug("Cannot load physics plugin.");

	SharedPtr<physics::IPhysicsSystem> pPhysics = create< physics::IPhysicsSystem >();
	YAKE_ASSERT( pPhysics ).debug("Cannot create physics system.");

	mWorld = pPhysics->createWorld();
	YAKE_ASSERT( mWorld ).debug("Cannot create world.");
}
void MiniApp::cleanUp()
{
	mWorld.reset();
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
	YAKE_ASSERT( mWorld );

	SharedPtr<IActor> pStaticWorldActor = mWorld->createActor();
	YAKE_ASSERT( pStaticWorldActor );

	pStaticWorldActor->createShape( IShape::PlaneDesc( Vector3(0,1,0), 0 ) );
	YAKE_ASSERT( pStaticWorldActor->getShapes().size() == 1 )(pStaticWorldActor->getShapes().size());

	SharedPtr<IActor> pBall = mWorld->createActor();
	YAKE_ASSERT( pBall );
	pBall->createShape( IShape::SphereDesc(real(1)) );
	YAKE_ASSERT( pBall->getShapes().size() == 1 );

	pBall->createBody();
	YAKE_ASSERT( pBall->hasBody() );
	pBall->getBody()->setMass( 5 );
	pBall->setPosition( Vector3(0,10,0) );
	pBall->getBody()->setLinearVelocity( Vector3(1,0,0) );

	for (size_t i = 0; i<100; ++i)
	{
		mWorld->step( 1./60. );
		std::cout << "ball at " << *pBall << std::endl;
	}
	/*
	pBall.reset();
	pStaticWorldActor.reset();
	mWorld.reset();
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
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
