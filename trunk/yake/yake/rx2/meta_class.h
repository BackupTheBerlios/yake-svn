#ifndef meta_class_h
#define meta_class_h

#include <vector>
#include "class_registry.h"
#include "meta_object.h"
#include "meta_hooks.h"

namespace rx
{

// is used for the instance an meta object resp. its typed fields
template<class T>
struct attach_field
{
	typedef typed_field<T> typed;

	static void attach( meta_object & obj, meta_field & field, bool clone )
	{
		if( clone )
		{
			typed & this_field = dynamic_cast< typed& >( field );
			obj.add_field< typed >( *new typed( obj, this_field.name_, this_field.value_, this_field.flags_ ) );
		}
		else
		{
			field.object_ = &obj;
			obj.add_field< typed >( dynamic_cast< typed& >( field ) );
		}
	}
};

class meta_class
{
public:
	typedef std::vector< std::pair< meta_field*, void(*)(meta_object&, meta_field&, bool) > > fields_list;
	typedef std::map< std::string, meta_object* > objects;

public:
	meta_class() 
	{
    register_class( *this );
	}

	meta_class( std::string class_name )
		: class_name_( class_name )
	{
		register_class( *this );
	}

	// todo free function stuff
	template< typename T1 >
	meta_class( std::string class_name, T1 & t1 )
		: class_name_( class_name )
	{
		register_class( *this );
		add_field( t1 );
	}
    
	~meta_class()
	{
		for( fields_list::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		{ delete iter->first; }
		unregister_class( class_name_ );
	}

	template< typename T >
	void add_field( std::string field_name, T default_value = T() )
	{
		add_field( field_name, default_value, none );
	}

	template< typename T, int flags >
	void add_field( std::string field_name, T default_value = T() )
	{
		add_field( field_name, default_value, flags );
	}

	// used by script hook
	template< typename T >
	void add_field( std::string field_name, T default_value, int flags )
	{
		// add field to container with typed attach method
		fields_.push_back( std::make_pair(
			new typed_field<T>( field_name, default_value, flags ),
			attach_field<T>::attach ) );
	}

	template< typename T >
	void add_field( T & field )
	{
		fields_.push_back( std::make_pair(
			&field,
			attach_field<T::value_type>::attach ) );
	}

	template< typename T >
	typed_field<T> & field( std::string name )
	{
		for( fields_list::iterator iter = fields_.begin();
			iter != fields_.end(); iter++ )
		{
      if( iter->first->name_ == name )
				return static_cast< typed_field<T>& >( *iter->first );
		}
		throw exception(); // todo
	}

	// used by regular meta object instancing
	meta_object & create_object( std::string object_name );

	// used by c++ classes
	// todo use free function stuff
	template< typename T1, typename T2, typename T3 >
	meta_object & create_object( std::string object_name, T1 & f1,
		T2 & f2, T3 & f3 )
	{
		meta_object * obj = new meta_object( object_name );

		// set name
		f1.field_name_ = fields_[0].first->get_name();
		// call the according attach method for this field ( using reference )
		( fields_[0].second )( *obj, f1 );
		f1.field_name_ = fields_[1].first->get_name();
		( fields_[1].second )( *obj, f2 );
		f1.field_name_ = fields_[2].first->get_name();
		( fields_[2].second )( *obj, f3 );

		objects_.insert( objects::value_type( object_name, obj ) ); 

		return *obj;
	}

	std::string & get_class_name()
	{
		return class_name_;
	}

	meta_object & get_object( std::string object_name )
	{
		return *objects_.find( object_name )->second;
	}

private:
	std::string class_name_;
	fields_list fields_;
	objects objects_;	
};

} // namespace rx

#endif