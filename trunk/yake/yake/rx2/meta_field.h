#ifndef meta_field_h
#define meta_field_h

#include <typeinfo>
#include <string>

namespace rx
{

class meta_object;

enum flags
{
	none			= 1,
	save			= 2,
	load			= 4,
	replicate	= 8
};

struct meta_field
{	
	meta_field( int flags = none )
		: object_(0), 
			flags_(flags)
	{}

	meta_field( meta_object & object, 
		std::string field_name, int flags = none )
		: object_( &object ), field_name_( field_name ),
			flags_(flags)
	{}

	meta_field( std::string field_name,	int flags = none )
		: field_name_( field_name ), 
		object_(0), flags_(flags)
	{}

	virtual ~meta_field() {}

	virtual std::string type_as_string() = 0;

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

	meta_object * object_;
	std::string field_name_;
	int flags_;
};

} // namespace rx

#endif