#ifndef meta_field_h
#define meta_field_h

#include <typeinfo>
#include <string>

class meta_object;

enum flags
{
	none			= 1,
  save			= 2,
	load			= 4,
	replicate = 8
};

struct meta_field
{	
	meta_field( std::string type_name )
		: type_name_( type_name ), object_(0)
	{}

	meta_field( meta_object & object, 
		std::string field_name, std::string type_name )
		: object_( &object ), field_name_( field_name ),
			type_name_( type_name )
	{}

	meta_field( std::string field_name,	std::string type_name )
		: field_name_( field_name ), type_name_( type_name ), object_(0)
	{}

	virtual ~meta_field() {}

  virtual std::string value_as_string() = 0;

	std::string as_string();

	virtual void add_to_object( meta_object & object ) = 0;
	virtual void add_clone_to_object( meta_object & object ) = 0;

	meta_object & get_object()
	{
		if( !object_ ) throw exception(); // todo
		return *object_;
	}

	std::string & get_name()
	{
		return field_name_;
	}

	std::string type_name_;
	meta_object * object_;
	std::string field_name_;	
};

#endif