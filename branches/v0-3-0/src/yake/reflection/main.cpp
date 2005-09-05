/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#include "bind_lua/bind_lua.h"
#include "bind_network/bind_network.h"
#include "bind_serialization/bind_serialization.h"

#include "reflection.h"
#include "events.h"

#include <iostream>
#include <fstream>

using namespace yake::reflection;
using namespace std;

// -----------------------------------------
// property
struct property_test
{
	CLASS(property_test, NullClass);
	PROPERTY(const char *, caption);
	const char * get_caption() const { return m_caption.c_str(); }
	void set_caption(const char * caption) { m_caption = caption; }
	std::string m_caption;
};

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
struct manual_event
{
	struct my_event 
	{	
		void attach() { std::cout << "manual_event::attach(): hello\n"; } 
		bool operator()(int i, int j) const { std::cout << "manual_event::operator(): hallo" << i << std::endl; return true; } 
	};
	my_event e;
};

// -----------------------------------------
// lua event bindings

struct abstract_class
{	
	virtual void do_pure_virtual() = 0; 
};

struct rx_event
{
	CLASS(rx_event, NullClass, lua);
	EVENT(public, f, (const abstract_class &), lua);
	EVENT(public, e, (int, int), lua);
};
IMPLEMENT_CLASS(rx_event, lua)

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
struct mi
{
	virtual ~mi() {}
};

// rx class has to be the first subclass when using mi
struct EventTest : public Registered, public mi
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
	// test type traits for yake::reflection::event
	{
		assert(boost::is_abstract<abstract_class>::value);

		typedef const abstract_class & T1;
		bool b =
			boost::type_traits::ice_and
			<
				boost::is_reference<T1>::value, 
				boost::is_abstract<typename boost::remove_reference<T1>::type>::value
			>::value;
		assert(b);

		yake::reflection::event<const abstract_class &> h;
	}
	

	// events
	{
		// todo: what happens with ClassRegistry::createInstance(); for serialization etc. ?
		// should be ClassRegistry::createInstance( "EventTest" );
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

		dobj->getClass().getProperty("value").get(value, dobj);
		cout << value << endl;
		assert(value == 100);
	}

	// porperty tests for the polished event bindings
	{
		using namespace luabind;
		module(L)
		[
			class_<manual_event::my_event>("my_event")
				.def("attach", &manual_event::my_event::attach)
				.def(get_const_signature<int, int>::signature()),
			class_<manual_event>("manual_event")
				.def(constructor<>())
				.def_readwrite("e", &manual_event::e)
		];
		dostring(L, "m_e = manual_event(); m_e.e:attach(); m_e.e(123, 456);");
	}

	// todo: delete this ...
	{
    using namespace luabind;

		lua_State * L = lua_open();
		lua_baselibopen(L);
		open(L);

		// class_::def(self(int())); where self::operator() returns call_operatorN
		// call_operator<self_type, class A1, class A2, ..., class AN> where self_type = self_type || const_self_type
		typedef operators::call_operator2<self_type, int, int> op_type;
		op_type op = 0;

		module(L)
		[
			class_<manual_event::my_event>("my_event")
				.def("attach", &manual_event::my_event::attach)
				.def(op), /* same as def(self(int(), int()) */
			class_<manual_event>("manual_event")
				.def(constructor<>())
				.def_readwrite("e", &manual_event::e)
		];

		dostring(L, "m_e = manual_event(); m_e.e:attach(); m_e.e(123, 456);");

		std::cin.get();
	}

	/*
	// lua bindings
	{
		// properties and methods
		cout << "[ lua ]\n";
		dostring(L, "o = LuaTest(); o.a=5; o:foo(); print(o.a);");
		LuaTest o; o.a = 589;	o.foo(); cout << o.a << std::endl;

		// events
		dostring(L, "function do_it_in_script(arg1, arg2) print('do_it_in_script(' .. arg1 .. ',' .. arg2 .. ')') end");
		dostring(L, "rx_e = rx_event();"); 
		dostring(L, "rx_e.e:attach_handler(do_it_in_script);"); 
		dostring(L, "rx_e.e(123, 456);");
	}*/

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
