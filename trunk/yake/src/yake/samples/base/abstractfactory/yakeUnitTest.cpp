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

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/ice.hpp>
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

namespace inheritance
{

/* linear */
struct semi_init_system_holder_root
{
	void load_systems() {std::cout << "load_systems(): root" << std::endl;}

	std::string get_data()
	{ return typeid(semi_init_system_holder_root).name(); }
};

template <class Base, class System>
struct semi_init_system_holder : Base
{
	void load_systems() 
	{ std::cout << "load_systems(): " << typeid(System).name() << std::endl; Base::load_systems(); }

	std::string get_data()
	{ return typeid(System).name(); }
};

struct A {};
struct B {};
struct C {};

typedef list<A, B, C> system_list;

struct test_linear : 
	private InheritLinear
	< 
		system_list, 
		typename lambda< semi_init_system_holder<_, _> >::type,
		semi_init_system_holder_root
	>::type
{
	test_linear()
	{	load_systems();	}

	template <class System>
	void print_name()
	{
		// create list of bases
		typedef boost::mpl::find<system_list, System>::type last;
		typedef boost::mpl::iterator_range<typename boost::mpl::begin<system_list>::type, next<last>::type>::type bases;
		// cast to base
		InheritLinear
		< 
			bases, 
			typename lambda< semi_init_system_holder<_, _> >::type,
			semi_init_system_holder_root
		>::type & base = *this;
		// print base data
		std::cout << "print_name<" << typeid(System).name() << ">(): " << base.get_data() << std::endl;
	}
};

/* multiple */
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
	// Linear
	{
		using namespace yake::testsuite::inheritance;	
		test_linear linear;
		linear.print_name<A>();
	}

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