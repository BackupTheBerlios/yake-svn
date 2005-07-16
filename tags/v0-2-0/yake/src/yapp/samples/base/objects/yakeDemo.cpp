//****************************************************************************
//**
//**    Sample - EXPERIMENTAL CODE
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
//#include <yapp/samples/base/objects/yakePCH.h>
#include <yake/rx/yakePCH.h>
#include <yake/rx/yakeRx.h>


#include <yapp/base/yapp.h>
#include <yapp/base/event/yakeEvent.h>

using namespace yake;
using namespace yake::base;
using namespace yake::base::templates;
using namespace yake::base::math;

namespace yake {

	/**
		@todo Hierarchy, Inheritance, ...
	*/

	class RxObject;
	class RxClass;
	class RxObjectId;

	class RxObjectId
	{
	private:
		RxObjectId();
		RxObjectId(const RxObjectId & id);
	public:
		RxObjectId(RxObject* pObj);
		~RxObjectId();
		RxClass* getClass() const;
		RxObject* getObject() const;

		bool isA( RxClass* pClass ) const;
	private:
		RxObject*	mObj;
		RxClass*	mClass;
	};
	
#define RXO_DECLARE_COMMON( RXNAME ) \
		private: \
			static RxClass msStaticRxClass; \
			typedef RXNAME ThisClass; \
		public: \
			static RxClass* rxGetClass(); \
			virtual RxClass* isA() const; \
			static ThisClass* safeCast(RxObject* pObject);

#define RXO_DECLARE_FACTORY( RXNAME ) \
			static RXNAME* create();

#define	RXO_DECLARE_BASE( RXNAME ) \
		RXO_DECLARE_COMMON( RXNAME ) \
		RXO_DECLARE_FACTORY( RXNAME )

#define	RXO_DECLARE( RXNAME ) \
		RXO_DECLARE_COMMON( RXNAME ) \
		RXO_DECLARE_FACTORY( RXNAME ) \
		static void rxInit();

#define	RXO_DECLARE_NOFACTORY( RXNAME ) \
		RXO_DECLARE_COMMON( RXNAME ) \
		static void rxInit();

#define RXO_DEFINE_COMMON( RXNAME ) \
		RxClass RXNAME::msStaticRxClass(#RXNAME, (FN_CREATERXOBJECT)RXNAME::create); \
		RxClass* RXNAME::isA() const \
		{ \
			return rxGetClass(); \
		} \
		RxClass* RXNAME::rxGetClass() \
		{ \
			return &msStaticRxClass; \
		} \
		RXNAME* RXNAME::safeCast(RxObject* pObject) \
		{ \
			YAKE_ASSERT( pObject ); \
			if (&msStaticRxClass == pObject->isA()) \
				return dynamic_cast<RXNAME*>(pObject); \
			else \
			{ \
				if (pObject->isA()->isDerivedFrom(&msStaticRxClass)) \
					return static_cast<RXNAME*>(pObject); \
				else \
					return 0; \
			} \
		} 

#define	RXO_DEFINE_FACTORY( RXNAME ) \
		RXNAME* RXNAME::create() \
		{ \
			return new RXNAME(); \
		}

#define	RXO_DEFINE_BASE( RXNAME ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		RXO_DEFINE_FACTORY( RXNAME )

#define	RXO_DEFINE( RXNAME, RXSUPER ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		RXO_DEFINE_FACTORY( RXNAME ) \
		void RXNAME::rxInit() \
		{ \
			msStaticRxClass.addSuper( RXSUPER::rxGetClass() ); \
		}
#define	RXO_DEFINE_NOFACTORY( RXNAME, RXSUPER ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		void RXNAME::rxInit() \
		{ \
			msStaticRxClass.addSuper( RXSUPER::rxGetClass() ); \
		}

#define	RXO_DEFINE2( RXNAME, RXSUPER0, RXSUPER1 ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		void RXNAME::rxInit() \
		{ \
			msStaticRxClass.addSuper( RXSUPER0::rxGetClass() ); \
			msStaticRxClass.addSuper( RXSUPER1::rxGetClass() ); \
		}

	class RxObject
	{
	public:
		virtual ~RxObject() {}
		RXO_DECLARE_BASE( RxObject );
	};

	typedef RxObject*(*FN_CREATERXOBJECT)();

	typedef uint32 RxClassId;
	class RxClass : public RxObject
	{
	public:
		RxClass(const String & name = "", FN_CREATERXOBJECT fnCreate = 0);
		~RxClass();
		RxClassId getId() const;
		void addSuper(RxClass* pClass);
		bool isDerivedFrom(RxClass* pClass);
		String getName() const;
		RxObject* createObject();

		bool operator == (const RxClass & rhs);

		RXO_DECLARE_BASE( RxClass );
	private:
		RxClassId			mId;
		typedef Deque<RxClass*> ClassList;
		ClassList			mParents;
		String				mName;
		FN_CREATERXOBJECT	mFnCreate;
	};
	RXO_DEFINE_BASE( RxClass );

	RXO_DEFINE_BASE( RxObject );

	String RxClass::getName() const
	{
		return mName;
	}
	void RxClass::addSuper(RxClass* pClass)
	{
		YAKE_ASSERT( pClass );
		mParents.push_back( pClass );
	}
	bool RxClass::isDerivedFrom(RxClass* pClass)
	{
		YAKE_ASSERT( pClass );
		for (ClassList::const_iterator it = mParents.begin(); it != mParents.end(); ++it)
		{
			RxClass* pSuper = *it;
			YAKE_ASSERT( pSuper );
			if (pSuper == pClass)
				return true;
			if (pSuper->isDerivedFrom( pClass ))
				return true;
		}
		return false;
	}
	RxClass::RxClass(const String & name, FN_CREATERXOBJECT fnCreate) : 
					mName(name), mFnCreate(fnCreate)
	{
		YAKE_ASSERT( mFnCreate );
		static int sIds = 0;
		mId = sIds++;
	}
	RxClass::~RxClass()
	{}
	RxObject* RxClass::createObject()
	{
		YAKE_ASSERT( mFnCreate );
		if (mFnCreate)
			return mFnCreate();
		else
			return 0;
	}
	RxClassId RxClass::getId() const
	{ return mId; }
	bool RxClass::operator == (const RxClass & rhs)
	{ return mId == rhs.getId(); }

	class RxClassManager
	{
	public:
		RxClassManager();
		~RxClassManager();
		void add(RxClass* pClass);
		void buildHierarchy();
	private:
		typedef Deque<RxClass*> ClassList;
		ClassList	mClasses;
	};
	RxClassManager::RxClassManager()
	{
	}
	RxClassManager::~RxClassManager()
	{
	}
	void RxClassManager::add(RxClass* pClass)
	{
		YAKE_ASSERT( pClass );
		mClasses.push_back( pClass );
	}
	void RxClassManager::buildHierarchy()
	{
	}

	//-- utilities

	RxClassId rxClassIdOf(const RxObject & o)
	{ 
		YAKE_ASSERT( o.isA() );
		return o.isA()->getId(); 
	}
	RxClassId rxClassIdOf(const RxObject * o)
	{ 
		YAKE_ASSERT( o );
		YAKE_ASSERT( o->isA() );
		return o->isA()->getId(); 
	}
	RxClass* rxClassOf(const RxObject & o)
	{
		return o.isA();
	}
	RxClass* rxClassOf(const RxObject * o)
	{
		YAKE_ASSERT( o );
		return o->isA();
	}

	//-- Entity
	//- declaration
	class Entity : public RxObject
	{
	public:
		RXO_DECLARE( Entity );
	};
	//- implementation
	RXO_DEFINE( Entity, RxObject );

	//-- Player
	//- declaration
	class Player : public Entity
	{
	public:
		RXO_DECLARE( Player );
	};
	//- implementation
	RXO_DEFINE( Player, Entity );

	class PropTypeBase : public RxObject
	{
	public:
		RXO_DECLARE_NOFACTORY( PropTypeBase );

		virtual void setValueString( const String & value ) = 0;
		virtual String getValueString() = 0;
	};
	RXO_DEFINE_NOFACTORY( PropTypeBase, RxObject );

	class PropTypeBool : public PropTypeBase
	{
	public:
		PropTypeBool(bool value);
		PropTypeBool(bool* pWrappedValue = 0);
		RXO_DECLARE( PropTypeBool );
		virtual void setValueString( const String & value );
		virtual String getValueString();
	private:
		bool	mValue;
		bool*	mWrapped;
	};
	RXO_DEFINE( PropTypeBool, PropTypeBase );
	PropTypeBool::PropTypeBool(bool value) : mWrapped(0), mValue(value)
	{
		mWrapped = &mValue;
	}
	PropTypeBool::PropTypeBool(bool* pWrappedValue) : mWrapped(pWrappedValue), mValue(false)
	{
		if (mWrapped == 0)
			mWrapped = &mValue;
	}
	void PropTypeBool::setValueString(const String & value)
	{
		if (value == "yes" || value == "1" || value == "true")
		{
			*mWrapped = true;
		}
		else
			*mWrapped = false;
	}
	String PropTypeBool::getValueString()
	{
		return (*mWrapped ? "1" : "0");
	}

	typedef Deque<PropTypeBase*> PropTypePtrList;
	class IObjectPropertyQuery
	{
	public:
		virtual ~IObjectPropertyQuery() {}
		virtual PropTypePtrList getProperties( bool bStatic, bool bDynamic, bool bIncludeSuperClasses ) = 0;
		virtual PropTypeBase* getProperty( const String & name ) = 0;
	};
	class IObjectDynamicPropertyInsert
	{
	public:
		virtual ~IObjectDynamicPropertyInsert() {}
		virtual void addDynamicProperty( PropTypeBase* pProp ) = 0;
	};
	class IObjectDynamicPropertyErase
	{
	public:
		virtual ~IObjectDynamicPropertyErase() {}
		virtual void eraseDynamicProperty( PropTypeBase* pProp ) = 0;
	};

}

#define COUTLN( x ) std::cout << x << std::endl;

class TheApp
{
public:
	void run()
	{
		RxClass* rxClass = RxClass::rxGetClass();
		COUTLN( "class: name = " << rxClass->getName() << " id = " << rxClass->getId() );
		rxClass = RxObject::rxGetClass();
		COUTLN( "class: name = " << rxClass->getName() << " id = " << rxClass->getId() );
		rxClass = Entity::rxGetClass();
		COUTLN( "class: name = " << rxClass->getName() << " id = " << rxClass->getId() );
		rxClass = Player::rxGetClass();
		COUTLN( "class: name = " << rxClass->getName() << " id = " << rxClass->getId() );

		//- register classes in hierarchy
		//RxClass::rxInit();
		//RxObject::rxInit();
		Entity::rxInit();
		Player::rxInit();

		PropTypeBase::rxInit();
		PropTypeBool::rxInit();

		Entity e1;
		Player p1;

#define SIMPLETEST( CONDITION, RESULT ) \
		{ \
			bool testRes = CONDITION; \
			COUTLN( #CONDITION << " == " << #RESULT << " -> " << ((testRes == RESULT) ? "YES" : "FAILED") ); \
		}

		SIMPLETEST( e1.isA()->isDerivedFrom(RxObject::rxGetClass()), true );
		SIMPLETEST( e1.isA()->isDerivedFrom(Entity::rxGetClass()), false );
		SIMPLETEST( e1.isA()->isDerivedFrom(Player::rxGetClass()), false );

		SIMPLETEST( p1.isA()->isDerivedFrom(RxObject::rxGetClass()), true );
		SIMPLETEST( p1.isA()->isDerivedFrom(Entity::rxGetClass()), true );
		SIMPLETEST( p1.isA()->isDerivedFrom(Player::rxGetClass()), false );

		SIMPLETEST( Entity::safeCast(&p1), true );
		SIMPLETEST( Player::safeCast(&e1), false );

		Player* pP1 = Player::create();
		YAKE_ASSERT( pP1 );
		SIMPLETEST( pP1->isA() == Player::rxGetClass(), true );
		SIMPLETEST( Entity::safeCast(pP1), true );

		Entity* pE1fromP1 = Entity::safeCast(pP1);
		SIMPLETEST( Player::safeCast(pE1fromP1), true ); //NOTE that this is possible!!
		YAKE_SAFE_DELETE( pP1 );

		PropTypeBool propBool;
		propBool.setValueString("true");
		SIMPLETEST( propBool.getValueString() != "false", true );
		SIMPLETEST( propBool.getValueString() == "1", true );

		bool b2 = false;
		PropTypeBool propBool2(&b2);
		SIMPLETEST( propBool2.getValueString() != "true", true );
		SIMPLETEST( propBool2.getValueString() == "0", true );
		COUTLN( "setting wrapped bool to 'true'" );
		propBool2.setValueString("true");
		SIMPLETEST( (propBool2.getValueString() == "1") && (b2 == true), true );

		/*
		Entity e1;
		Entity e2;
		bool bRet = e1.isA() == e2.isA();
		COUTLN( "e1.isA() == e2.isA() -> " << bRet );
		YAKE_ASSERT( bRet );

		Player p1;
		bRet = (p1.isA() == e1.isA());
		COUTLN( "p1.isA() == e1.isA() -> " << bRet );
		YAKE_ASSERT( !bRet );

		//COUTLN( "classIdOf(e1) = " << e1.isA()->getId() );
		//COUTLN( "classIdOf(p1) = " << p1.isA()->getId() );
		COUTLN( "classIdOf(e1) = " << rxClassIdOf(e1) );
		COUTLN( "classIdOf(p1) = " << rxClassIdOf(p1) );

		RxClass* c1 = 0;
		RxClass* c2 = 0;

		c1 = e1.isA();
		c2 = e2.isA();
		bRet = (*c1 == *c2);
		COUTLN( "classOf(e1) == classOf(e2) -> " << bRet );
		YAKE_ASSERT( bRet );

		c1 = e1.isA();
		c2 = p1.isA();
		bRet = (*c1 == *c2);
		COUTLN( "classOf(p1) == classOf(e1) -> " << bRet );
		YAKE_ASSERT( !bRet );

		RxObject* o = 0;
		o = Entity::cast( &p1 );
		bRet = o!=0;
		COUTLN( "Entity::cast(p1) -> " << bRet );
		YAKE_ASSERT( bRet );

		o = Player::cast( &e1 );
		bRet = o==0;
		COUTLN( "Player::cast(e1) -> " << bRet );
		YAKE_ASSERT( bRet );
		*/
	}
};

int main()
{
	try
	{
		std::cout << std::endl << "Demo provided for YAKE by Stephan Kaiser" << std::endl << std::endl;
		TheApp theApp;
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