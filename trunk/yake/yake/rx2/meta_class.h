#ifndef meta_class_h
#define meta_class_h

#include <vector>
#include "meta_object.h"

class meta_class
{
public:
	typedef std::vector< meta_field * > fields_list; // todo smart ptrs
	typedef std::map< std::string, meta_object * > objects;

public:
	meta_class() 
	{}

	meta_class( std::string class_name )
		: class_name_( class_name )
	{
	}

	// todo free function stuff
	template< typename T1 >
	meta_class( std::string class_name, T1 & t1 )
		: class_name_( class_name )
	{
		add_field( t1 );
	}
    
	~meta_class()
	{
		for( fields_list::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		{ delete *iter; }
	}

	template< typename T, int flags >
	void add_field( std::string field_name, T default_value = T() )
	{
		fields_.push_back( new typed_field<T, flags>( 
			field_name, default_value ) );
	}

	template< typename T >
	void add_field( T & field )
	{
		fields_.push_back( &field );
	}

	// used by regular meta object instancing
	meta_object & create_object( std::string object_name )
	{
		meta_object * obj = new meta_object( object_name );

		for( fields_list::iterator iter = fields_.begin(); 
			iter != fields_.end(); iter++ )
		{ 
			( *iter )->add_clone_to_object( *obj );
		}

		return *obj;
	}

	// used by c++ classes
	// todo use free function stuff
	template< typename T1, typename T2, typename T3 >
	meta_object & create_object( std::string object_name, T1 & f1,
		T2 & f2, T3 & f3 )
	{
		meta_object * obj = new meta_object( object_name );

		f1.field_name_ = fields_[0]->get_name();
		f1.add_to_object( *obj );
		f2.field_name_ = fields_[1]->get_name();
		f2.add_to_object( *obj );
		f3.field_name_ = fields_[2]->get_name();
		f3.add_to_object( *obj );

		objects_.insert( objects::value_type( object_name, obj ) ); 

		return *obj;
	}

	std::string get_class_name()
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

#endif