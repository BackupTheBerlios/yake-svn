#pragma warning(disable: 4305) // int to unsigned short
#pragma warning(disable: 4309) // contraction const value
#pragma warning(disable: 4244) // __w64 int to int
#pragma warning(disable: 4267) // site_t to unsigned int
#pragma warning(disable: 4800) // bits AND => bool

// stl
#include <cassert>
#include <iostream>
#include <fstream>
// mop
#include "meta_class.h"
#include "class_registry.h"
// serialization
#include "typed_field_serialized.h"
#include "meta_object_serialized.h"
// scripting
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <luabind/luabind.hpp>
// repl
#include "Multiplayer.h"
#include "DistributedNetworkObjectHeader.h"
#include "RakServerInterface.h"
#include "RakClientInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"

using namespace rx;

int main()
{
	// runtime class and object defining
	{
		std::cout << "[ meta operations ]" << std::endl;	

		// define class with default value
		meta_class test_class( "test_class" );
		test_class.add_field<bool>( "hello_bool", true );
		test_class.add_field<std::string>( "hello_string" );
	
		// create an instance of that just defined class
		meta_object & test_object = test_class.create_object( "test_object" );

		// set and stream some values
		test_object.field<bool>( "hello_bool" ) = false;
		assert( !test_object.field<bool>( "hello_bool" ) );
		std::cout << "test_object::hello_bool=" << ( test_object.field<bool>( 
			"hello_bool" ) ? "true" : "false" ) << std::endl;		

		// append the objects field list
		test_object.add_field<bool>( "dead", false );
		assert( !test_object.field<bool>( "dead" ) );
		test_object.field<bool>( "dead" ) = true;
		assert( test_object.field<bool>( "dead" ) );
		std::cout << "test_object::dead=" << test_object.field<bool>( 
			"dead" ) << std::endl;

		// try another object
		meta_object & test_object_2 = test_class.create_object( "test_object_2" );
		assert( test_object_2.field<bool>( "hello_bool" ) );
		std::cout << "test_object_2::hello_bool=" << ( test_object_2.field<bool>( 
			"hello_bool" ) ? "true" : "false" ) << std::endl;
	}


	/* todo

<psy|code> if i want to replicate a position, do i _have_ to use typed_field<Vector3> or sth like that?
<psy|code> and now i have to tie this to the actual Movable...

<metaX> I think there are two possibilities : 
<metaX> 1. a simply class-to-member-ptr wrapper
<metaX> 2. used typed_field<vector> ;P
<metaX> -d
* FuSiON has joined #yake
<metaX> how would you do it with rx1?
<metaX> well, would be possible to do this:
<metaX> struct player : public movable, rxobject {}; and then add a class-member-ptr to movable::position ...
<metaX> but there would be no sychronization
==><metaX> perhaps it's possible to do typed_field<vector&>( "blah", this->myvector );?



	*/

	// reflect a c++ object
	/*{
		std::cout << std::endl << "[ c++/meta cooperation ]" << std::endl;	

		// create the c++ object	
		cpp_class obj( "cpp_object" );
		obj.test_float = 3.3f;
		obj.test_string = "test_string_";

		// create the meta object this or that way
		//object_ & meta_obj = get_object( "cpp_class", "cpp_object");  
		//meta_object & meta_obj = get_object<cpp_class>( "cpp_object" ); 
		meta_object & meta_obj = obj.get_object(); 

		// set field using c++ interface and check for meta reflection
		obj.test_int = 54321;	
		assert( meta_obj.field<int>( "test_int" ) == obj.test_int );
		std::cout << meta_obj.field<int>( "test_int" ) << std::endl;		

		// set same field using mop and check for c++ reflection
		meta_obj.field<int>( "test_int" ) = 12345;
		assert( meta_obj.field<int>( "test_int" ) == obj.test_int );
		std::cout << obj.test_int << std::endl;	

		// copy value
		int test_int = meta_obj.field<int>( "test_int" );
		std::cout << test_int << std::endl;
		assert( test_int == 12345 );

		// change field directly and check for reflection
		typed_field<int> & meta_test_int = meta_obj.field<int>( "test_int" );
		std::cout << meta_test_int << std::endl;
		meta_test_int = 123456;
		assert( meta_obj.field<int>( "test_int" ) == 123456 );
		assert( obj.test_int == 123456 );

		// output fields
		for( meta_object::fields_map::const_iterator iter = meta_obj.begin(); 
			iter != meta_obj.end(); iter++ )
		{
			std::cout << iter->second->as_string() << std::endl;
		}
	}*/

	// serialize meta objects
	{
		std::cout << std::endl << "[ serialization ]" << std::endl;	

		// serialize
		{
			// create and open a character archive for output
			std::ofstream ofs("filename");
			boost::archive::text_oarchive oa(ofs);
			// define class with default value
			meta_class simple_class( 
				"simple_class", *new typed_field<std::string>( 
					"hello_string", "hello_string_", save | load ) );
			// create an instance of that just defined class
			meta_object & simple_object = simple_class.create_object( "simple_object" );    
			// write class instance to archive
			oa << simple_object;
			// close archive
			ofs.close();
		}

		// deserialize
		{
			// ... some time later restore the class instance to its orginal state
			// create and open an archive for input
			std::ifstream ifs("filename", std::ios::binary);
			boost::archive::text_iarchive ia(ifs);
			// read class state from archive
			meta_object simple_object;
			ia >> simple_object;
			std::cout << simple_object.field<std::string>( "hello_string" ).as_string() << std::endl;
			assert( simple_object.field<std::string>( "hello_string" ).get() == "hello_string_" ); // todo
			// close archive
			ifs.close();
		}
	}

	// script bindings
	{
		std::cout << std::endl << "[ binding mop to lua ]" << std::endl;	

		using namespace luabind;

		lua_State* L = lua_open();
		luaopen_io(L);
		open(L);

		// todo register function
		// todo use concrete fields inside lua and write converter between lua<>cpp?
		// todo lua supports meta-mechanisms?
		module(L)
		[ 
			class_<meta_field>("meta_field")
				.enum_("flags")
				[
					value("none",					1),
					value("save",					2),
					value("load",					4),
					value("replicate",		8),
					value("uncompressed", 16),
					value("compressed",		32),
					value("copy",					64),
					value("interpolate",	128),
					value("server",				256),
					value("client",				512)
				],
			class_<int_field>("int_field")
				.def("get", &int_field::get )
				.def("set", &int_field::set ),
			class_<lua_hook_object>("lua_hook_object")
				.def("add_field_int", (void(lua_hook_object::*)(std::string)) &lua_hook_object::add_field_int)
				.def("add_field_int", (void(lua_hook_object::*)(std::string, int)) &lua_hook_object::add_field_int) 
				.def("add_field_int", (void(lua_hook_object::*)(std::string, int, int)) &lua_hook_object::add_field_int)
				.def("field_int",	&lua_hook_object::field_int ),
			class_<meta_object, lua_hook_object>("meta_object") 	
				.def(constructor<std::string>()),
			class_<meta_class>("meta_class") 	
				.def(constructor<std::string>())
				.def("create_object", &meta_class::create_object )
				.def("add_field_int", (void(meta_class::*)(std::string, int)) &meta_class::add_field<int>)
				.def("add_field_int", (void(meta_class::*)(std::string, int, int)) &meta_class::add_field<int>)
				.def("field_int",	(int_field&(meta_class::*)(std::string)) &meta_class::field<int> )
		];

		lua_dostring(	L, "meta_cl = meta_class('hello_class')" );
		lua_dostring(	L, "meta_cl:add_field_int( 'hello_int', meta_field.none )" );
		lua_dostring(	L, "meta_cl:field_int( 'hello_int' ):set( 12345 )" );
		lua_dostring(	L, "meta_obj = meta_cl:create_object('hello_object')" );
		lua_dostring(	L, "meta_obj:field_int( 'hello_int' ):set( 1234 )" );

		std::cout << get_class( "hello_class" ).get_object( 
			"hello_object" ).field<int>( "hello_int" ).as_string() << std::endl;

		assert( get_class( "hello_class" ).get_object( "hello_object" ).field<int>( "hello_int" ) == 1234 );

		lua_close(L);
	}

	// field flags and replication
	{
		std::cout << std::endl << "[ field flags and replication ]" << std::endl;	

		// init raknet
		RakClientInterface *rakClient=RakNetworkFactory::GetRakClientInterface();
		RakServerInterface *rakServer=RakNetworkFactory::GetRakServerInterface();
		Multiplayer<RakClientInterface> rakClientMultiplayer;
		Multiplayer<RakServerInterface> rakServerMultiplayer;

		bool is_server = false;

		std::cout << "select (c)lient or (s)erver" << std::endl; 
		if( std::cin.get() == 'c' )
		{
			if( !rakClient->Connect("127.0.0.1", 12345, 123456, 0, 30) )
				std::cout << "client error" << std::endl;
			DistributedNetworkObjectManager::Instance()->RegisterRakClientInterface(rakClient);
		}
		else
		{
			is_server = true;
			if( !rakServer->Start(2, 0, 30, 12345) )
				std::cout << "server error" << std::endl;
			DistributedNetworkObjectManager::Instance()->RegisterRakServerInterface(rakServer);
		}

		// define class with default value
		meta_class replica_class;
		replica_class.add_field<bool, replicate | server>( "hello_bool", false );
	
		// create an instance of that just defined class
		meta_object & replica_object = replica_class.create_object( "replica_object" );

		// main loop
		bool old_value = replica_object.field<bool>( "hello_bool" );
		while(true)
		{
				rakClientMultiplayer.ProcessPackets(rakClient);
				rakServerMultiplayer.ProcessPackets(rakServer);

				if(is_server)
				{
						std::cout << "(t)rue or (f)alse?" << std::endl;
						replica_object.field<bool>( "hello_bool" ) = ( std::cin.get() == 't' );
						std::cin.ignore( 80, '\n' );
				}				
				else 
				{
					if( old_value != replica_object.field<bool>( "hello_bool" ) )
					{
						std::cout << replica_object.field<bool>( "hello_bool" ) << std::endl;
						old_value = replica_object.field<bool>( "hello_bool" );
					}
				}      
		}

		// destroy raknet
		RakNetworkFactory::DestroyRakClientInterface(rakClient);
		RakNetworkFactory::DestroyRakServerInterface(rakServer);
	}

	std::cin.get();

	return 0;
}

