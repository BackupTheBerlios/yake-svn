#ifndef meta_object_hooks_h
#define meta_object_hooks_h

#include "DistributedNetworkObjectHeader.h"
#include <string>
#include "concrete_fields.h"


template< class A, class B >
struct inherit_container : public A, public B
{
};

template< class A, class V >
struct inherit
{
	typedef inherit_container< A, V > type;
};



template< class A, class B >
struct mix_container : inherit< A, B >
{
	template< typename T >
	void on_add_field( T & field )
	{
		A::on_add_field< T >( field );
		B::on_add_field< T >( field );
	}

	template< typename T >
	void on_change_field( T & field )
	{
		A::on_change_field< T >( field );
		B::on_change_field< T >( field );
	}

};

template< class A, class B >
struct mix
{
	typedef mix_container< A, B > type;
};


struct empty_hook;
struct raknet_hook;
struct sql_hook;
struct lua_hook;


// User defines the hook here
typedef inherit< raknet_hook, lua_hook >::type meta_object_hooks;



struct empty_hook
{
	template< typename T >
	inline void on_add_field( T & field )
	{
	}

	template< typename T >
	inline void on_change_field( T & field )
	{
	}
};

struct raknet_hook : public DistributedNetworkObject
{
	raknet_hook() : field_number_(0) {}
	virtual ~raknet_hook(){}

	template< typename T >
	inline void on_add_field( T & field )
	{
		if( field.bit_flags_ & replicate )
			SynchronizeMemory( 
				field_number_++, reinterpret_cast<char*>( 
					&field.value_ ), 
						sizeof( field.value_ ), true ); // todo authority, interpolation, update interval etc.
	}

	template< typename T >
	inline void on_change_field( T & field )
	{
		if( field.bit_flags_ & replicate )
			UpdateDistributedObject(
				const_cast<char*>( 
					field.get_object().get_name().c_str() ) ); // todo check
					

		// todo del
		if( field.bit_flags_ & replicate )
			std::cout << "replication: " << field.get_name() << "=" << field.value_ << std::endl;
      
	}

	int field_number_;
};

struct sql_hook
{
	sql_hook() {}
	virtual ~sql_hook(){}

	template< typename T >
	void on_add_field( T & field )
	{
	}

	template< typename T >
	void on_change_field( T & field )
	{
	}
};

struct lua_hook
{
	lua_hook() {}
	virtual ~lua_hook(){}

	void add_field_int( std::string field_name/*, int flags = none*/ );
	void add_field_int( std::string field_name, int default_value = int()/*, int flags = none*/ );


	int_field & field_int( std::string field_name );
};

#endif