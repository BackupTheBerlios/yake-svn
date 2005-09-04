//****************************************************************************
//**
//**    Sample
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <yake/samples/rx/basics/yakePCH.h>
#include <yake/rx/yakeRx.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::templates;
using namespace yake::base;
using namespace yake;

#define RXO_ATTR_NONE() \
	private: \
	static void rxInitAttributePrototypes() \
{}

#define RXO_ATTR_BEGIN() \
	private: \
	static void rxInitAttributePrototypes() \
{

#define RXO_ATTR_END() \
}

#define RXO_ATTR_DEF( NAME, CPPNAME, VTTYPE, CPPTYPE ) \
	ms_attrProto.push_back( new rx::AttributeProto(NAME, VTTYPE, new rx::MemPtr<ThisClass,CPPTYPE>(&ThisClass::CPPNAME) ) );

namespace game {

	//-------------------------------------------------------------

	//-- Entity
	//- declaration
	class Entity : public rx::RxObject
	{
	protected:
		Entity() : mView(432.1)
		{
		}
	public:
		real	mView;
	public:
		RXO_DECLARE( Entity );
		RXO_ATTR_BEGIN()
			RXO_ATTR_DEF( "view", mView, rx::VT_REAL, real )
		RXO_ATTR_END()
	};
	//- implementation
	RXO_DEFINE( Entity, rx::RxObject );

	//-------------------------------------------------------------

	//-- Player
	//- declaration
	class Player : public Entity
	{
	protected:
		Player() : health(static_cast<real>(1.1))
		{
		}
	public:
		RXO_DECLARE( Player );
		RXO_ATTR_BEGIN()
			RXO_ATTR_DEF( "health", health, rx::VT_REAL, real )
		RXO_ATTR_END()

	public:
		real	health;
	};

	//- implementation
	RXO_DEFINE( Player, Entity );

	//-------------------------------------------------------------

	//-- Door
	//- declaration
	class Door : public Entity
	{
	protected:
		Door()
		{
		}
	public:
		RXO_DECLARE( Door );
		RXO_ATTR_NONE()
	};

	//- implementation
	RXO_DEFINE( Door, Entity );
}
//============================================================================
//    Application
//============================================================================

#define COUTLN( x ) std::cout << x << std::endl;

#define SIMPLETEST( CONDITION, RESULT ) \
{ \
	bool testRes = CONDITION; \
	COUTLN( #CONDITION << " == " << #RESULT << " -> " << ((testRes == RESULT) ? "YES" : "FAILED") ); \
}

template<class T>
T* rxCreate()
{
	::yake::rx::RxClass* pClass = T::rxGetClass();
	if (!pClass)
		return 0;
	return static_cast<T*>(pClass->createObject());
}

class TheApp
{
public:
	TheApp()
	{
	}
	void run()
	{
		using namespace game;

		//- register classes in hierarchy
		//RxClass::rxInit();
		//RxObject::rxInit();
		Entity::rxInit();
		Player::rxInit();

		//NOTE: From now on we have to define instances dynamically as the creator function does
		//		some very important post-processing!
		//Entity e1;
		//Player p1;
		Entity* e1 = rxCreate<Entity>();
		Player* p1 = rxCreate<Player>();

		// list attributes
		rx::AttrPtrList attr = p1->getAttributes();
		attr.back()->setValueString("2.2");
		COUTLN ("p1 attr: " << attr );

		// cast a bit
		/* working. but i don't need to test it right now.
		SIMPLETEST( p1->isA() == Player::rxGetClass(), true );
		SIMPLETEST( Entity::safeCast(p1) != 0, true );
		Entity* pE1fromP1 = Entity::safeCast(p1);
		SIMPLETEST( Player::safeCast(pE1fromP1) != 0, true ); //NOTE that this is possible!!

		COUTLN( "classOf(p1) = " << rxClassOf(p1)->getName() );
		COUTLN( "classOf(e1) = " << rxClassOf(e1)->getName() );
		*/

		// add attributes to player instance
		COUTLN("");
		COUTLN("add attribute 'isDead' to player instance:");
		p1->addDynamicAttribute( new rx::Attribute("isDead",rx::VT_BOOL) );
		rx::AttrPtrList list = p1->getAttributes();
		list.back()->setValueString("true");
		COUTLN( list );

		// add attribute proto type
		//TODO: should be done via RxClass ...

		// dynamic class attribute test. RxClass is a rx class like any other else.
		rx::RxClass* rxClass = Player::rxGetClass();
		COUTLN( "class: name = " << rxClass->getName() << " id = " << rxClass->getId() );
		rxClass->addDynamicAttribute( new rx::Attribute("aBool",rx::VT_BOOL) );
		rxClass->addDynamicAttribute( new rx::Attribute("aBoolTrue",rx::VT_BOOL) );
		list = rxClass->getAttributes();
		list.back()->setValueString("true");
		COUTLN( list );

		//
		Player* p2 = rxCreate<Player>();
		attr = p2->getAttributes();
		COUTLN ("p2 attr: " << attr );

		// more tests...
		//INFO: ALTERNATIVE FORMEN DES SPEICHERNS VON WERTEN...
		rx::ValueBool propBool;
		propBool.setValueString("true");
		SIMPLETEST( propBool.getValueString() != "false", true );
		SIMPLETEST( propBool.getValueString() == "true", true );

		/*FUNKTIONIERT. WAR MIR ABER GERADE ZU VIEL AUF DEM SCREEN.
		bool b2 = false;
		rx::ValueBool propBool2(&b2);
		SIMPLETEST( propBool2.getValueString() != "true", true );
		SIMPLETEST( propBool2.getValueString() == "false", true );
		COUTLN( "setting wrapped bool to 'true'" );
		propBool2.setValueString("true");
		//propBool2 = "true";
		SIMPLETEST( (propBool2.getValueString() == "true") && (b2 == true), true );

		b2 = false;
		rx::Attribute attrBool("attrBool", rx::VT_BOOL, &b2);
		SIMPLETEST( b2 == false, true );
		SIMPLETEST( attrBool.getValueAsString() != "true", true );
		attrBool.setValueString("true");
		SIMPLETEST( b2 == true, true );
		*/
	}
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

YAKE_DECLARE_GLOBAL
int main()
{
	try 
	{
		YAKE_DECLARE_FUNCTION( main )

		{
			TheApp theApp;
			theApp.run();
		}

#if defined( YAKE_DEBUG_BUILD )
		std::cout << std::endl << "Waiting for you...";
		std::cin.get();
#endif
	}
	catch ( yake::base::Exception & e )
	{
		std::cout << "YAKE EXCEPTION: " << e.getSource() << std::endl;
		std::cout << "File: " << e.getFile() << " @ " << e.getLine() << std::endl;
		std::cout << "Reason: " << e.getMessage() << std::endl << std::endl;
	}

	return 0;
}
