//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// std
#include <memory>
#include <string>
#include <cassert>
// yake
#include <yake/base/yake.h>
#include <yake/base/mpl/abstract_factory_pars.h>
#include <yake/base/mpl/sequences.h>
// boost
#include <boost/mpl/begin.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/next.hpp>

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

struct Soldier { virtual ~Soldier() {} };
struct Monster { virtual ~Monster() {} };
struct SuperMonster { virtual ~SuperMonster() {} };

struct SillySoldier : Soldier { SillySoldier(int i) {std::cout << i << std::endl;} };
struct SillyMonster : Monster { SillyMonster(bool b, long l) {std::cout << l << std::endl;} };
struct SillySuperMonster : SuperMonster { SillySuperMonster(std::string s) {std::cout << s << std::endl;} };

typedef abstract_factory
< 
	boost::mpl::list<Soldier, Monster, SuperMonster>,
	abstract_factory_unit_pars,
	boost::mpl::list
	<
		boost::mpl::list<int>,
		boost::mpl::list<bool, long>,
		boost::mpl::list<std::string>
	>
> AbstractEnemyFactory;

typedef concrete_factory
< 
	AbstractEnemyFactory,
	list<SillySoldier, SillyMonster, SillySuperMonster> 
> EnemyFactory;

} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	// abstract factory
	{
		using namespace yake::testsuite;

		std::auto_ptr<AbstractEnemyFactory> factory(new EnemyFactory);
		Soldier * s = factory->create<Soldier>(12345);  
		Monster * m = factory->create<Monster>(true, 54321);  
		SuperMonster * sm = factory->create<SuperMonster>("hello");   
		bool type_check = !!(typeid(*sm) == typeid(SillySuperMonster));
		assert(type_check);
		delete s, m, sm;
	}

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}