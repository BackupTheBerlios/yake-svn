#ifndef meta_hooks_h
#define meta_hooks_h

// todo split this file: replication, sql, script and add serialization

#include "DistributedNetworkObjectHeader.h"
#include <string>
#include "concrete_fields.h"

namespace rx
{

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
	virtual ~raknet_hook() {}

	template< typename T >
	inline void on_add_field( T & field )
	{
		if( field.flags_ & replicate )
			SynchronizeMemory( 
				field_number_++, reinterpret_cast<char*>( 
					&field.value_ ), 
						sizeof( field.value_ ), 
							field.flags_ & server );
	}

	template< typename T >
	inline void on_change_field( T & field )
	{
		if( field.flags_ & replicate )
			UpdateDistributedObject(
				const_cast<char*>( 
					field.get_object().get_name().c_str() ) ); // todo check
					
		// todo del
		if( field.flags_ & replicate )
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

// meta class and object derive from this class, because
// luabing cannot handle template functions 
/*template< class T >
struct lua_hook
{
	lua_hook() {}
	virtual ~lua_hook(){}

	void add_field_int( std::string field_name )
	{
		dynamic_cast< T* >( this )->add_field<int, none>( field_name );
	}

	void add_field_int( std::string field_name, int flags = none )
	{
		dynamic_cast< T* >( this )->add_field<int>( field_name, flags );
  }

	void add_field_int( std::string field_name, int default_value = int(), int flags = none )
	{
		dynamic_cast< T* >( this )->add_field<int>( field_name, default_value, flags );
	}

	int_field & field_int( std::string field_name )
	{
		return dynamic_cast< T* >( this )->field<int>( field_name );
	}
};*/

// User defines the hook here
class meta_object;
class meta_class;

//typedef lua_hook< meta_object > lua_hook_object;
//typedef lua_hook< meta_class > lua_hook_class;

//typedef inherit< raknet_hook, lua_hook_object >::type meta_object_hooks;
typedef raknet_hook meta_object_hooks;
//typedef lua_hook_class meta_class_hooks;

} // namespace rx

#endif