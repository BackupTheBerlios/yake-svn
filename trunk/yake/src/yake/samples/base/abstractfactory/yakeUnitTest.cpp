//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Std
#include <memory>
#include <string>
// Yake
#include <yake/base/yake.h>
#include <yake/base/mpl/yakeAbstractFactory.h>
#include <yake/base/mpl/sequences.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base;
using namespace yake::base::mpl;
using namespace yake::base::mpl::lambda;
using namespace yake::base::mpl::sequences;

namespace yake
{
namespace testsuite
{

namespace inheritance
{

struct Widget {};

template< class T >
struct Holder { T value; };

typedef InheritMultiple
	<
		list< int, std::string, Widget >,
		lambda< Holder< _ > >::type
	>::type myWidgetInfo;

} // inheritance


namespace abstractfactory
{

class Soldier { public: virtual ~Soldier() {} };
class Monster { public: virtual ~Monster() {} };
class SuperMonster { public: virtual ~SuperMonster() {} };

class SillySoldier : public Soldier {};
class SillyMonster : public Monster {};
class SillySuperMonster : public SuperMonster {};

class BadSoldier : public Soldier {};
class BadMonster : public Monster {};
class BadSuperMonster : public SuperMonster {};

typedef AbstractFactory< list< Soldier, Monster, SuperMonster > > AbstractEnemyFactory; 

typedef ConcreteFactory< AbstractEnemyFactory, OpNewFactoryUnit,
	list< SillySoldier, SillyMonster, SillySuperMonster > > EasyLevelEnemyFactory;

typedef ConcreteFactory< AbstractEnemyFactory, OpNewFactoryUnit,
	list< BadSoldier, BadMonster, BadSuperMonster > > HardLevelEnemyFactory;

} // abstractfactory

} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	// Scattering
	{
		using namespace yake::testsuite::inheritance;		
		myWidgetInfo widgetInfo;
		std::string info = ( static_cast< Holder< std::string >& >( widgetInfo ) ).value;
	}

	// Abstract factory
	{
		using namespace yake::testsuite::abstractfactory;		

		std::auto_ptr<AbstractEnemyFactory> easyFactory(new EasyLevelEnemyFactory);
		std::auto_ptr<AbstractEnemyFactory> hardFactory(new HardLevelEnemyFactory);

		Soldier* s = easyFactory->create<Soldier>();   
		bool r = !!( typeid( *s ) == typeid( SillySoldier ) ); //SGB !! eliminates bool-to-int performance warning
		delete s;
	}

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}




