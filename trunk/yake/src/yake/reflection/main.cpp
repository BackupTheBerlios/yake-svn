#include "bind_serialization/bind_serialization.h"
#include "bind_lua/bind_lua.h"
#include "bind_network/bind_network.h"

#include "reflection.h"
#include "events.h"

#include <iostream>
#include <fstream>

using namespace reflection;
using namespace std;

// -----------------------------------------
// serialization
struct SerializationTest : public Object
{
	CLASS(SerializationTest, Object);
	DEFAULT_PROPERTY(int, value);
	FIELD(public, double, field1);
	METHOD(public, void, action0, ()) 
	{ cout << "SerializationTest::action0();\n"; }
};
IMPLEMENT_CLASS(SerializationTest, serialize)

// -----------------------------------------
// lua
struct LuaTest
{
	CLASS(LuaTest, NullClass, lua);
	DEFAULT_PROPERTY(int, a, lua);
	METHOD(public, void, foo, (), lua)
	{	cout << "LuaTest::foo()" << " a=" << a << endl;	}
};
IMPLEMENT_CLASS(LuaTest, lua)

// todo optionale geniersche script erweiterung hier (scipt wrapper benutzen)


// -----------------------------------------
// lua property

struct null {};

// accepts any number of arguments and returns the according type selected by the holder
template< template<int, typename, typename, typename> class holder, typename T1 = null, typename T2 = null, typename T3 = null>
struct construct_type_from_arbitrary_args
{
	// signature dispatching according to the number of arguments
	typedef typename boost::mpl::if_
	< 
		boost::is_same<T3, null>, 
		typename holder<2, T1, T2, T3>::type,
		typename boost::mpl::if_
		< 
			boost::is_same<T2, null>, 
			typename holder<1, T1, T2, T3>::type,
			typename boost::mpl::if_
			< 
				boost::is_same<T1, null>, 
				typename holder<0, T1, T2, T3>::type,
				typename holder<3, T1, T2, T3>::type
			>
		>
	>::type type;
};

// lua operator signatures
// The get function is returning a null pointer of a specific type,
// this type is used as function signature (constructor<> holds the signature
// and the second parameter serves as const/non-const flag). 
// The overloaded template function .def() of class_ accepts the
// signature and automagically resolves the paramater types, because
// it's a template function.
template<int, typename T1, typename T2, typename T3>
struct signature_holder {};

template<typename T1, typename T2, typename T3>
struct signature_holder<0, T1, T2, T3> 
{
  static luabind::detail::application_operator
	< 
		luabind::constructor<>, 
		true
	> * signature()
	{ return 0; }
};

template<typename T1, typename T2, typename T3>
struct signature_holder<1, T1, T2, T3> 
{
		static luabind::detail::application_operator
		< 
			luabind::constructor<	luabind::other<T1> >, 
			true
		> * signature()
		{ return 0; }
};

template<typename T1, typename T2, typename T3>
struct signature_holder<2, T1, T2, T3> 
{
	static luabind::detail::application_operator
	< 
		luabind::constructor
		<
			luabind::other<T1>, 
			luabind::other<T2>
		>, 
		true
	> * signature()
	{ return 0; }
};

template<typename T1, typename T2, typename T3>
struct signature_holder<3, T1, T2, T3> 
{
	static luabind::detail::application_operator
	< 
		luabind::constructor
		<
			luabind::other<T1>, 
			luabind::other<T2>,
			luabind::other<T3>
		>, 
		true
	> * signature()
	{ return 0; }
};

// selects a signature holding template according to the number of arguments
template<int num_args, typename T1, typename T2, typename T3>
struct get_signature_holder 
{
	typedef typename signature_holder<num_args, T1, T2, T3> type;
};

template<typename T1 = null, typename T2 = null, typename T3 = null>
struct get_signature : construct_type_from_arbitrary_args<get_signature_holder, T1, T2, T3>::type {};

struct A
{
	struct my_event 
	{	
		void attach() { std::cout << "hello\n"; } 
		bool operator()(int i, int j) const { std::cout << "hallo" << i << std::endl; return true; } 
	};
	my_event e;
};

// -----------------------------------------
// lua event bindings
struct rx_A
{
	CLASS(rx_A, NullClass, lua);
	EVENT(public, e, (int));

	rx_A() { std::cout << "rx_A\n"; }

private:
	struct register_event_e
	{
		struct initor
		{
			initor() 
			{ 
				// events
				using namespace luabind;
				module(L)
				[ 
					class_<boost::function_base>("function_base"),
					class_<reflection::lua_event_base>("lua_event_base")
						.def("attach_handler", &reflection::lua_event_base::attach_handler)
				];

				/* todo: hold list of event typeids and check whether this event is registered or not */ 
				using namespace luabind; 
				module(L) 
				[ 
					class_<reflection::event<int>, reflection::lua_event_base>("e") 
						.def(self(other<int>())) 
				]; 
				get_lua_class().def_readonly("e", &rx_A::e); 
				commit_lua_properties(); 
			} 
		}; 

		register_event_e() 
		{ static initor init; } 
	} reg_event_e;

};
IMPLEMENT_CLASS(rx_A, lua)




// -----------------------------------------
// network
struct NetworkPlayerTest;
NetworkPlayerTest * ClientScopeObject = NULL;
std::list<NetworkPlayerTest*> AllPlayers;

struct NetworkPlayerTest : public Replicatable
{
	NetworkPlayerTest() { cout << "NetworkPlayerTest()\n"; AllPlayers.push_back(this); }

	CLASS(NetworkPlayerTest, NullClass, net, lua);
	DEFAULT_PROPERTY(int, a, net, net_parms(), lua);
	DEFAULT_PROPERTY(int, b, net);
	DEFAULT_PROPERTY(int, c, net, net_parms(123));
	DEFAULT_PROPERTY(int, d, net, net_parms(123, 456));

	bool onGhostAdd(yake::net::GhostConnection*)
	{ if(isClientScopeObject()) ClientScopeObject = this; return true; }

	void performScopeQuery(yake::net::GhostConnection * connection_)
	{
		for( std::list<NetworkPlayerTest*>::const_iterator iter = AllPlayers.begin();
			iter != AllPlayers.end(); iter++ )
			connection_->objectInScope(*iter);
	}
};
IMPLEMENT_CLASS(NetworkPlayerTest, net, lua)

class NetworkConnectionTest : public yake::net::GhostConnection
{
public:
	TNL::RefPtr<NetworkPlayerTest> myPlayer;
	void onConnectionEstablished()
	{
		TNL::GhostConnection::onConnectionEstablished();   
		if(isInitiator())
		{ // client
			setGhostFrom(false);
			setGhostTo(true);
		}
		else
		{ // server
			myPlayer = new NetworkPlayerTest(); // a player for each client
			setScopeObject(myPlayer);
			setGhostFrom(true);
			setGhostTo(false);
			activateGhosting();
		}
	}
TNL_DECLARE_NETCONNECTION(NetworkConnectionTest);
};
TNL_IMPLEMENT_NETCONNECTION(NetworkConnectionTest, TNL::NetClassGroupGame, true);

class NetworkInterfaceTest : public yake::net::NetInterface
{
public:
	NetworkInterfaceTest(const yake::net::Address & bindAddress) : yake::net::NetInterface(bindAddress) {}
	void processConnections()
	{
		yake::net::NetInterface::processConnections();
		// only the server allows connections
		if(doesAllowConnections())
		{
			cout << "Ghosting done.\n";
			// after we send all ghost updates we can clear the dirty lists
			for( std::list<NetworkPlayerTest*>::const_iterator iter = AllPlayers.begin();
				iter != AllPlayers.end(); iter++ )
				(*iter)->clear();
		}
	}
};

// -----------------------------------------
// event
struct boo
{
	virtual ~boo() {}
};

// rx class has to be the first subclass when using mi
struct EventTest : public Registered, public boo
{
	EventTest() {}
	CLASS(EventTest, Registered, reg);
	EVENT(public, foo, (int));
	METHOD(public, void, handler, (int fara))
	{ std::cout << "EventTest::handler(" << fara << ")\n"; }
};

struct Handler
{
	CLASS(Handler, NullClass);
	METHOD(public, void, handler, (int fara))
	{ std::cout << "Handler::handler(" << fara << ")\n"; }
};

void handler(int fara)
{ std::cout << "handler(" << fara << ")\n"; }


// -----------------------------------------
// main
int main()
{
	// events
	{
		// todo ClassRegistry::createInstance( "EventTest" ); ?
		EventTest create_reg(test); // test("test");

		// attach handler using test object
		test.foo.attach_handler(&handler);
		test.getClass().getEvent("foo").attach_handler(&test, boost::function<void (int)>(boost::bind(&handler, _1)));

		// attach handler using registered object
		Registered * obj = Registered::instances_map["test"];

		obj->getClass().getEvent("foo").attach_handler(obj, boost::function<void (int)>(boost::bind(&handler, _1)));
		obj->getClass().getEvent("foo").attach_handler(obj, boost::function<void (int)>(boost::bind(&EventTest::handler, &test, _1)));

		// attach rx method and rebinding with 'this argument' argument
		const Method & m = obj->getClass().getMethod("handler");	
		boost::function<void (void*, int)> * fnc = static_cast<boost::function<void (void*, int)>*>(&m.get_function_base());
		boost::function<void (int)> fnc_rebind = boost::bind(*fnc, &test, _1);		
		obj->getClass().getEvent("foo").attach_handler(obj, fnc_rebind);

		// attach rx method with automagically 'this argument' rebinding
		obj->getClass().getEvent("foo").attach_handler(obj, m, obj);

		Handler hdl;
		const Method & m2 = hdl.getClass().getMethod("handler");
		obj->getClass().getEvent("foo").attach_handler(obj, m2, &hdl);

		// fire
		test.foo(100);
		test.getClass().getEvent("foo").fire(&test, 100);
	}

	{	// serialization
		register_SerializationTest();
		cout << "[ serialization ]\n";

		Object * obj = ClassRegistry::createInstance( "SerializationTest" );

		for(Class::PropertyList::const_iterator it = obj->getClass().getProperties().begin();
			it != obj->getClass().getProperties().end();
			++it)
		{
			const Property & prop = *it;
			cout << prop.getType() << " " << prop.getName() << endl;
			prop.set(obj, 100);
		}

		int value;
		obj->getClass().getProperty("value").get(value, obj);
		cout << value << endl;

		// serialize
		ofstream ofs( "filename" );
		boost::archive::text_oarchive oa( ofs );
		oa << obj;
		ofs.close();

		// deserialize
		ifstream ifs( "filename", std::ios::binary );
		boost::archive::text_iarchive ia( ifs );
		Object * dobj = NULL;
		ia >> dobj;
		ifs.close();

		int dvalue;
		dobj->getClass().getProperty("value").get(value, dobj);
		cout << value << endl;
		assert(value == 100);
	}

	// porperty tests for the polished event bindings
	{
		using namespace luabind;
		module(L)
		[
			class_<A::my_event>("my_event")
				.def("attach", &A::my_event::attach)
				.def(get_signature<int, int>::signature()),
			class_<A>("A")
				.def(constructor<>())
				.def_readwrite("e", &A::e)
		];
		dostring(L, "a = A(); a.e:attach(); a.e(123, 456);");
	}

	// lua bindings
	{
		// properties and methods
		cout << "[ lua ]\n";
		dostring(L, "o = LuaTest(); o.a=5; o:foo(); print(o.a);");
		LuaTest o; o.a = 589;	o.foo(); cout << o.a << std::endl;

		// events
		dostring(L, "function do_it_in_script(arg1) print('do_it_in_script(' .. arg1 .. ')') end");
		dostring(L, "rx_a = rx_A();"); 
		dostring(L, "rx_a.e:attach_handler(do_it_in_script);"); 
		dostring(L, "rx_a.e(123);");
	}

	// networking
	{
		cout << "[ networking ]\n";
		bool isServer = false;
		bool sendUpdates = false;
		TNL::RefPtr<NetworkInterfaceTest> theNetInterface;
		char c = std::cin.get();
		if(c == 'c' || c == 'd')
    { // client
			std::cout << "CLIENT\n";
      theNetInterface = new NetworkInterfaceTest(TNL::Address(TNL::IPProtocol, TNL::Address::Any, 0));
			theNetInterface->setAllowsConnections(false);
      NetworkConnectionTest * newConnection = new NetworkConnectionTest();
      newConnection->connect(theNetInterface, TNL::Address("127.0.0.1:27800"));
			sendUpdates = ( c == 'd' );
    }
    else
    { // server
			std::cout << "SERVER\n";
			theNetInterface = new NetworkInterfaceTest(TNL::Address("127.0.0.1:27800"));
      theNetInterface->setAllowsConnections(true);
			isServer = true;
    }
    while(true)
    {
			static int counter = 0;
      theNetInterface->checkIncomingPackets();
      theNetInterface->processConnections();
      TNL::Platform::sleep(1000);
			if(!isServer && ClientScopeObject && sendUpdates) ClientScopeObject->a = counter++;
    }
	}

	getchar();
  return 0;
}
